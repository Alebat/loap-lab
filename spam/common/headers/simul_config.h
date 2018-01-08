#ifndef SIMUL_CONFIG_H
#define SIMUL_CONFIG_H

#include <stdint.h>

/** the current status of the application running on the brick. It can be either
 *  in configuration mode, or in simulation mode.
 */
typedef enum {STATUS_CONFIG, STATUS_SIMUL} sim_status_t;

/** the function given as input to the motor can be either a step or a sinusoid. */
typedef enum {SIM_STEP, SIM_SIN} sim_type_t;

/** indices of the different configuration parameters within the configuration packet payload. */
#define AMP_IDX  0
#define OFF_IDX  1
#define DUR_IDX  2
#define SAM_IDX  3
#define FREQ_IDX 4

#define MAX_SIM_CONFIG_SIZE 5

/** the configuration for each simulation specifies the kind of input function to use,
 *  and the specific parameters.

 *  At the moment the application supports two different input functions:
 *  - step function, for which we can indicate the amplitude, the offset (delay of the step), and the duration and sampling time of the simulation
 *  - sin function, for which we can indicate the amplitude, the offset (delay of the step), the frequency (sin(wt)), the duration and sampling time
 *    of the simulation

 */
typedef struct
{
  sim_type_t sim_type;
  int data[MAX_SIM_CONFIG_SIZE];
} sim_config_t;


/** functions to get the parameters from the configuration */
int get_duration(sim_config_t *config);
int get_speed(sim_config_t *config, uint32_t elapsed_time);
int get_sampling_time(sim_config_t *config);
int get_frequency(sim_config_t *config);

/** functions to fill a configuration structure with all the required paramters */
void create_step_config(sim_config_t *config, int amplitude, int offset, int duration, int sampling_time);
void create_sin_config(sim_config_t *config, int amplitude, int offset, int duration, int sampling_time, int frequency);

#endif
