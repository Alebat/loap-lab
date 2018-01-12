#include "headers/simul_config.h"
#include <math.h>
#include <string.h>

#define PI_2 6.28318530718f

static inline int get_speed_step(sim_config_t* config, uint32_t elapsed_time);
static inline int get_speed_sin(sim_config_t* config, uint32_t elapsed_time);
static inline void create_base_config(sim_config_t* config, int amplitude, int offset, int duration, int sampling_time);

int get_duration(sim_config_t *config)
{
  int duration = 0;
  switch (config->sim_type)
  {
  case SIM_STEP:
  case SIM_SIN:
    duration = config->data[DUR_IDX];
    break;
  }
  return duration;
}

int get_speed(sim_config_t *config, uint32_t elapsed_time)
{
  int speed = 0;
  switch (config->sim_type)
  {
  case SIM_STEP:
    speed = get_speed_step(config, elapsed_time);
    break;
  case SIM_SIN:
    speed = get_speed_sin(config, elapsed_time);
    break;
  }
  return speed;
}

int get_sampling_time(sim_config_t *config)
{
  int sampling_time = 0;
  switch (config->sim_type)
  {
  case SIM_STEP:
  case SIM_SIN:
    sampling_time = config->data[SAM_IDX];
    break;
  }
  return sampling_time;
}

int get_frequency(sim_config_t *config)
{
  int frequency = 0;
  switch (config->sim_type)
  {
  case SIM_STEP:
    frequency = 0;
    break;
  case SIM_SIN:
    frequency = config->data[FREQ_IDX];
    break;
  }
  return frequency;
}

int get_speed_step(sim_config_t* config, uint32_t elapsed_time)
{
  return elapsed_time>=config->data[OFF_IDX] ? config->data[AMP_IDX] : 0;
}

int get_speed_sin(sim_config_t* config, uint32_t elapsed_time)
{
  float val = elapsed_time/1000.f * config->data[FREQ_IDX] * PI_2/10.;
  while (val > PI_2) val -= PI_2;
  return round(sinf(val)*config->data[AMP_IDX]);
}

void create_base_config(sim_config_t* config, int amplitude, int offset, int duration, int sampling_time)
{
  config->data[AMP_IDX] = amplitude;
  config->data[OFF_IDX] = offset;
  config->data[DUR_IDX] = duration;
  config->data[SAM_IDX] = sampling_time;
}

void create_step_config(sim_config_t *config, int amplitude, int offset, int duration, int sampling_time)
{
  create_base_config(config, amplitude, offset, duration, sampling_time);
  config->sim_type = SIM_STEP;
}

void create_sin_config(sim_config_t *config, int amplitude, int offset, int duration, int sampling_time, int frequency)
{
  create_base_config(config, amplitude, offset, duration, sampling_time);
  config->data[FREQ_IDX] = frequency;
  config->sim_type = SIM_SIN;
}
