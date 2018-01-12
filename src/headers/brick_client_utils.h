#ifndef BRICK_CLIENT_UTILS_H
#define BRICK_CLIENT_UTILS_H

#include "../../spam/common/headers/bt_comm.h"
#include "../../spam/common/headers/simul_config.h"
#include "brick_client_bt.h"

#define FILENAME_MAX_LEN 255

typedef struct
{
  /*
   *  This is the 6-bytes encoding variable type defined by the Libbluetooth.
   */
  bdaddr_t mac;
  char filename[FILENAME_MAX_LEN];
  sim_config_t config;

} sim_client_options;

extern const int DEFAULT_AMP, DEFAULT_OFFSET, DEFAULT_DURATION,DEFAULT_SAMP_TIME, DEFAULT_FREQUENCY;

int process_data(FILE *data_file, data_record_t incoming_data[]);

int opts_parse_config(sim_client_options *options, int argc, char* argv[]);


#endif
