/** @file BRO_spam_client.h */
#ifndef __bro_headers_spam_client_h
#define __bro_headers_spam_client_h

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/** Function to stop the motor by setting its velocity to 0. */
void stop_motor();

/** Function to start the simulation.
 *
 *  It resets the motor count, the buffer, the initial time of the simulation,
 *  and enables the alarms for both the task responsible to set the motor speed 
 *  during the simulation, and the task storing and sending the data to the PC.
 */
void start_sim(sim_config_t *config);

/** Function to pause the configuration task during the execution of the simulation.
 *
 *  It waits for the notification of a SimTermEvt event.
 */
void wait_sim_termination();

/** Function to terminate a simulation after it has been completed.
 *
 *  It is responsible to reset the application to the initial state, to allow it to 
 *  receive requests for new simulations, and to inform the PC that the current 
 *  simulation is complete.
 */
void terminate_sim();

/** Function to accumulate the collected data into a buffer, and to send all the information
 *  to the PC when the buffer is full.
 */
void send_buffered_data(uint32_t elapsed_time, int value);

/** Function to flush and send all the information currently stored in the buffer to the PC. */
void flush_buffer();

/** Function to reset the buffer to its initial state. */
void clear_buffer();

/** Function to send an array of size bytes to the PC over the Bluetooth connection. */
void send_packet(uint8_t data[], int size);

/** Function to receive an array of size bytes from the PC over the Bluetooth connection. */
void recv_packet(uint8_t data[], int size);

/** Function to send a special message to the PC indicating that the current simulaton is complete. */
void send_sim_end();

#endif
