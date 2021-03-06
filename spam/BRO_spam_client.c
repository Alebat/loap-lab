#include <stdio.h>
#include <math.h>
#include "common/headers/simul_config.h"
#include "common/headers/bt_comm.h"
#include "headers/BRO_spam_client.h"

#define DELAY 10
#define MOTOR_TASK_PERIOD 1
#define PORT_IN_USE NXT_PORT_S4

int sonar_read_ms = 100;
int sonar_ms_count = 0;

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(SysTimerCnt);
DeclareResource(Lcd);
DeclareResource(SimConfigGlobal);
DeclareTask(DisplayTask);
DeclareTask(SimulTask);
DeclareTask(ConfigTask);
DeclareEvent(SimTermEvt);
DeclareAlarm(SimulTaskAlarm);
DeclareAlarm(MotorTaskAlarm);

const uint32_t MOTOR_PORT = NXT_PORT_B;
const uint32_t MOTOR_PORT2 = NXT_PORT_A;

sim_status_t sim_status;
sim_config_t sim_config;

uint32_t init_time;

/** buffer to store the sampled data */
data_record_t data_msg[MAX_DATA_ITEMS];
uint32_t data_idx;


/** ------------------------------------------- */
const float EXPMEAN_FACTOR = 0.6;
const float SNR_R = 0.056f/2.0f;
const float SONAR_SAMPLING = 0.2f;

float sonar = 0;
float left_ct = 10;
float right_ct = 10;

float updateExpMean(float expmean, float newval) {
    return expmean*(1-EXPMEAN_FACTOR) + newval*EXPMEAN_FACTOR;
}

float saturation(float val, float min, float max)
{
    if(val > max)
        val = max;
    else if(val < min)
        val = min;
    return val;
}

#ifndef TASK
#define TASK(x) void x()
#endif


struct MotorControllerData {
    // Previous values
    float last_err;
    float last_out;
    int last_revcount;

    //Current metrics
    float speed_deg;
    float speed_rad;
    float expmean;
    float err;
    float out;
}mcd1, mcd2;

void mcd_init(struct MotorControllerData *mcd)
{
    mcd->last_err = 0;
    mcd->last_out = 0;
    mcd->last_revcount = 0;

    mcd->speed_deg = 0;
    mcd->speed_rad = 0;
    mcd->expmean = 0;
    mcd->err = 0;
    mcd->out = 0;
}

int mcd_getSpeed(struct MotorControllerData mcd, int speedval) {
    if(speedval > 0)
        return (int)saturation(mcd.out, 0, 100);
    return 0;
}

void mcd_onGetSpeed(struct MotorControllerData *mcd, int rev_count, float ref, float etime5)
{
    // Getting speed in rad/s
    mcd->speed_deg = (rev_count - mcd->last_revcount)/etime5;
    mcd->last_revcount = rev_count;
    mcd->speed_rad = mcd->speed_deg * 3.14159265359f/180.0f;

    // Exponential mean filtering
    mcd->expmean = updateExpMean(mcd->expmean, mcd->speed_rad);

    // Calculating the current error (input of the controller)
    mcd->err = ref - mcd->expmean;

    // Calculating the controller output
    mcd->out = mcd->last_out + 5.25*mcd->err - 4.75*mcd->last_err;
    mcd->last_err = mcd->err;
    mcd->last_out = mcd->out;
}


/** ------------------------------------------- */


/*--------------------------------------------------------------------------*/
/* LEJOS OSEK hooks                                                         */
/*--------------------------------------------------------------------------*/
void ecrobot_device_initialize() {
	ecrobot_init_sonar_sensor(PORT_IN_USE);
}
void ecrobot_device_terminate()
{
	ecrobot_term_sonar_sensor(PORT_IN_USE);
    stop_motor();
    ecrobot_term_bt_connection();
}

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(void) {
    StatusType ercd;

    /*
     *  Increment OSEK Alarm System Timer Count
     */
    ercd = SignalCounter(SysTimerCnt);
    if (ercd != E_OK) {
        ShutdownOS(ercd);
    }
}


/** Task started automatically when the application starts. This task initializes the Bluetooth connection,
 *  receives the configuration messages to set all the parameters for the simulation, and starts the simulation
 *  when a SIM_START message is received from the PC.
 */
TASK(ConfigTask) {

    mcd_init(&mcd1);
    mcd_init(&mcd2);

    uint8_t in_packet[MAX_CONF_SIZE];

    sim_status = STATUS_CONFIG;

    while (1)
    {
        while (ecrobot_get_bt_status() != BT_STREAM) // waits for a master device to establish a connection
        {
            ecrobot_init_bt_slave("1234");
            systick_wait_ms(50);
        }

        recv_packet(in_packet, MAX_CONF_SIZE);

        GetResource(SimConfigGlobal);
        sim_status = decode_config_msg(in_packet, &sim_config);
        ReleaseResource(SimConfigGlobal);

        if (sim_status == STATUS_SIMUL) {
            start_sim(&sim_config);
            wait_sim_termination();
        }
    }
}


/** Task setting the motor speed at each step according to the input function specific for the current simulation,
 *  and the elapsed time
 */
TASK(MotorTask) {
    uint32_t elapsed_time = systick_get_ms()-init_time;
    int speed_val = get_speed(&sim_config, elapsed_time);

    int speed_m1 = mcd_getSpeed(mcd1, speed_val);
    int speed_m2 = mcd_getSpeed(mcd2, speed_val);

    //speed_m1 = speed_m2 =100;

    nxt_motor_set_speed(MOTOR_PORT, speed_m1, 1);
    nxt_motor_set_speed(MOTOR_PORT2, speed_m2, 1);
    TerminateTask();
}

uint32_t last_elapsed_time;
float etime100 = 0;
float dist_err = 0;
float dist_err_last = 0;
float u2 = 0;
float u2_last = 0;
/** Task responsible to sample the data regarding the motor during the simulation, and to send it to the PC.
 *  When the simulation is terminated, this task resets the status of the application and unlocks the Configuration
 *  Task.
 */
TASK(SimulTask) {
    uint32_t elapsed_time = systick_get_ms()-init_time;

    float etime5 = (elapsed_time - last_elapsed_time)/1000.0f;
    last_elapsed_time = elapsed_time;


    if (elapsed_time >= get_duration(&sim_config))
    {
        terminate_sim();
        TerminateTask();
    }
    else
    {
        // Read Motors & sonar ================================================
        int rev_count1 = nxt_motor_get_count(MOTOR_PORT);
        int rev_count2 = nxt_motor_get_count(MOTOR_PORT2);



        etime100 += etime5;
        if(etime100 >= SONAR_SAMPLING)
        {
            sonar =ecrobot_get_sonar_sensor(PORT_IN_USE)/100.0f;
            if(sonar > .6)
                sonar = .6;

            // Vehicle Controller ==================================================
            dist_err = 0.35 - sonar;
            u2 = ((etime100*7.8f + 52)*dist_err + (7.8f*etime100 - 52)*dist_err_last - (4*etime100 - 2.0f) * u2_last)/(4*etime100 + 2.0f);
            dist_err_last = dist_err;
            u2_last = u2;

            etime100 -= SONAR_SAMPLING;
        }


//        left_ct = 4;
//        right_ct = 4;
        left_ct = saturation((.2f - u2*.125f)/(2*SNR_R), 0, 12);
        right_ct = saturation((.2f + u2*.125f)/(2*SNR_R), 0, 12);


        // Motor Controllers ==================================================
        mcd_onGetSpeed(&mcd1, rev_count1, left_ct, etime5);
        mcd_onGetSpeed(&mcd2, rev_count2, right_ct, etime5);
        // Send Data
//        send_buffered_data(elapsed_time, rev_count1); // 0
//        send_buffered_data(elapsed_time, (int)mcd1.speed_rad); // 1
//        send_buffered_data(elapsed_time, (int)(100*mcd1.err)); // 2
//        send_buffered_data(elapsed_time, (int)(mcd1.out)); // 3
//
//        send_buffered_data(elapsed_time, rev_count2); // 4
//        send_buffered_data(elapsed_time, (int)mcd2.speed_rad); // 5
//        send_buffered_data(elapsed_time, (int)(100*mcd2.err)); // 6
//        send_buffered_data(elapsed_time, (int)(mcd2.out)); // 7

        send_buffered_data(elapsed_time, (int)(100*sonar)); // 8

        send_buffered_data(elapsed_time, (int)(100*left_ct)); // 12
        send_buffered_data(elapsed_time, (int)(100*right_ct)); // 13
        TerminateTask();
    }
}


/** Task responsible to show on the display some informations regarding the current status of the application. */
TASK(DisplayTask) {
                sim_status_t curr_status;
    sim_config_t curr_config;

    // copy the current status and config. to auxiliary variables
    GetResource(SimConfigGlobal);
    curr_status = sim_status;
    curr_config = sim_config;
    ReleaseResource(SimConfigGlobal);

    display_clear(0);
    switch (curr_status)
    {
        case STATUS_CONFIG:
            ecrobot_status_monitor("Configuration Mode");
        break;
        case STATUS_SIMUL:
            display_goto_xy(0, 0);
            display_string("Type: ");
            display_string(curr_config.sim_type == SIM_STEP ? "  Step" : "   Sin");
            display_goto_xy(0, 1);
            display_string("Amp:  ");
            //display_int((int) curr_config.data[AMP_IDX], 6);
            display_int((int)(sonar*100.0f), 6);
            display_goto_xy(0, 2);
            display_string("Off:  ");
            display_int((int) curr_config.data[OFF_IDX], 6);
            display_goto_xy(0, 3);
            display_string("Dur:  ");
            display_int(get_duration(&curr_config), 6);
            display_goto_xy(0, 4);
            display_string("Samp: ");
            display_int(get_sampling_time(&curr_config), 6);
            if (curr_config.sim_type == SIM_SIN) {
                display_goto_xy(0, 5);
                display_string("Freq: ");
                display_int(get_frequency(&curr_config), 6);
            }
            break;
    }
    display_update();

    TerminateTask();
}











































void stop_motor() {
    nxt_motor_set_speed(MOTOR_PORT, 0, 1);
    nxt_motor_set_speed(MOTOR_PORT2, 0, 1);
}

void start_sim(sim_config_t *config) {
    init_time = systick_get_ms();
    nxt_motor_set_count(MOTOR_PORT, 0);
    nxt_motor_set_count(MOTOR_PORT2, 0);
    clear_buffer();
    SetRelAlarm(MotorTaskAlarm, DELAY, MOTOR_TASK_PERIOD);
    SetRelAlarm(SimulTaskAlarm, DELAY, get_sampling_time(config));
}

void wait_sim_termination() {
    WaitEvent(SimTermEvt);
    ClearEvent(SimTermEvt);
}

void terminate_sim() {
    flush_buffer();
    send_sim_end();
    sim_status = STATUS_CONFIG;
    ecrobot_term_bt_connection();
    CancelAlarm(SimulTaskAlarm);
    CancelAlarm(MotorTaskAlarm);
    stop_motor();
    SetEvent(ConfigTask, SimTermEvt);
}

void send_buffered_data(uint32_t elapsed_time, int value) {

    data_msg[data_idx].time = elapsed_time;
    data_msg[data_idx++].value = value;
    if (data_idx == MAX_DATA_ITEMS)
        flush_buffer();
}

void flush_buffer() {
    uint8_t data_packet[MAX_DATA_SIZE];

    if (data_idx > 0) {
        memset(data_packet, 0, MAX_DATA_SIZE);
        encode_sim_data_msg(data_packet, data_msg);
        send_packet(data_packet, MAX_DATA_SIZE);
        clear_buffer();
    }
}

void clear_buffer() {
    data_idx = 0;
    memset(data_msg, 0, sizeof(data_record_t) * MAX_DATA_ITEMS);
}

void send_packet(uint8_t data[], int size) {
    uint32_t sent_bytes = 0;
    while (sent_bytes < size) {
        sent_bytes += ecrobot_send_bt(data, sent_bytes, size - sent_bytes);
    }
}

void recv_packet(uint8_t data[], int size) {
    int recv_bytes = 0;

    do {
        recv_bytes += ecrobot_read_bt(data + recv_bytes, 0, size - recv_bytes);
    } while (recv_bytes < size);
}

void send_sim_end() {
    data_msg[data_idx++] = END_SIMULATION;
    flush_buffer();
}
