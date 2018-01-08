#include "headers/brick_client_bt.h"
#include "headers/brick_client_utils.h"
//#include "../spam/common/headers/bt_comm.h"

int main (int argc, char *argv[])
{
  int brick_socket = -1, res;
  sim_client_options options;
  uint8_t sim_config_pkt[MAX_CONF_SIZE];
  data_record_t incoming_data[MAX_DATA_ITEMS];
  uint8_t incoming_data_pkt[MAX_DATA_SIZE];

  memset(&options, 0, sizeof(sim_client_options));
  memset(sim_config_pkt, 0, MAX_CONF_SIZE*sizeof(uint8_t));
  memset(incoming_data, 0, sizeof(data_record_t)*MAX_DATA_ITEMS);
  memset(incoming_data_pkt, 0, MAX_DATA_SIZE*sizeof(uint8_t));

  if (opts_parse_config(&options, argc, argv) == -1)
  {
    return 0;
  };

  brick_bt_connect_device(&brick_socket, options.mac);

  printf("Filename: %s\n", options.filename);
  FILE *data_file = fopen(options.filename, "w");

  encode_sim_config_msg(sim_config_pkt, &options.config);
  brick_bt_send(brick_socket, sim_config_pkt, MAX_CONF_SIZE);

  memset(sim_config_pkt, 0, MAX_CONF_SIZE*sizeof(uint8_t));
  encode_sim_start_msg(sim_config_pkt);
  brick_bt_send(brick_socket, sim_config_pkt, MAX_CONF_SIZE);


  do {
    res = brick_bt_recv(brick_socket, incoming_data_pkt, MAX_DATA_SIZE);
    if (res >= 0)
    {
      decode_sim_data_msg(incoming_data_pkt, incoming_data);
      res = process_data(data_file, incoming_data);
    }
  } while (res >= 0);

  printf("Simulation terminated\n");

  brick_bt_close_connection(brick_socket);
  fclose(data_file);
  return 0;
}
