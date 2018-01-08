#include "headers/bt_comm.h"
#include <string.h>

const data_record_t END_SIMULATION = {-1,-1};

/** A configuration message has the following format:
	- the first byte is used to encode the kind of message (config. or start simulation)
	- in the case of a configuration message, the second byte represents the kind of input function to use
	  and all the successive bytes are used to encode the parameters for the current simulation.
*/

void encode_sim_start_msg(uint8_t conf_packet[MAX_CONF_SIZE])
{
    conf_packet[0] = MSG_START_SIM;
}

void encode_sim_config_msg(uint8_t conf_packet[MAX_CONF_SIZE], const sim_config_t* sim_config)
{
    int idx = 0;
    conf_packet[idx++] = MSG_CONF;
    conf_packet[idx++] = sim_config->sim_type;
    encode_int(conf_packet, &idx, sim_config->data[AMP_IDX]);
    encode_int(conf_packet, &idx, sim_config->data[OFF_IDX]);
    encode_int(conf_packet, &idx, sim_config->data[DUR_IDX]);
    encode_int(conf_packet, &idx, sim_config->data[SAM_IDX]);
    if (sim_config->sim_type == SIM_SIN)
    {
        encode_int(conf_packet, &idx, sim_config->data[FREQ_IDX]);
    }
}

sim_status_t decode_config_msg(const uint8_t *conf_packet, sim_config_t* sim_config)
{
  sim_status_t res = STATUS_CONFIG;

  switch (conf_packet[0])
  {
  case MSG_CONF:
    res = STATUS_CONFIG;
    decode_config(conf_packet+1, sim_config);
    break;
  case MSG_START_SIM:
    res = STATUS_SIMUL;
    break;
  }

  return res;
}

void decode_config(const uint8_t *conf_data, sim_config_t* sim_config)
{
    int idx = 0;
    sim_config->sim_type = conf_data[idx++];
    decode_int(conf_data, &idx, &sim_config->data[AMP_IDX]);
    decode_int(conf_data, &idx, &sim_config->data[OFF_IDX]);
    decode_int(conf_data, &idx, &sim_config->data[DUR_IDX]);
    decode_int(conf_data, &idx, &sim_config->data[SAM_IDX]);
    if (sim_config->sim_type == SIM_SIN)
    {
        decode_int(conf_data, &idx, &sim_config->data[FREQ_IDX]);
    }
}

void encode_int(uint8_t *data, int *idx, int value)
{
    data[(*idx)++] = (value >> 24) & 0xFF;
    data[(*idx)++] = (value >> 16) & 0xFF;
    data[(*idx)++] = (value >>  8) & 0xFF;
    data[(*idx)++] = value & 0xFF;
}

void decode_int(const uint8_t *data, int *idx, int *dst)
{
    *dst  = (data[(*idx)++] & 0xFF) << 24;
    *dst |= (data[(*idx)++] & 0xFF) << 16;
    *dst |= (data[(*idx)++] & 0xFF) <<  8;
    *dst |= (data[(*idx)++] & 0xFF);
}

void encode_sim_data_msg(uint8_t data_packet[MAX_DATA_SIZE], const data_record_t data[MAX_DATA_ITEMS])
{
    int i, idx = 0;
    for (i=0; i<MAX_DATA_ITEMS; ++i)
    {
        encode_int(data_packet, &idx, data[i].time);
        encode_int(data_packet, &idx, data[i].value);
    }
}

void decode_sim_data_msg(const uint8_t data_packet[MAX_DATA_SIZE], data_record_t data[MAX_DATA_ITEMS])
{
    int i, idx = 0;
    for (i=0; i<MAX_DATA_ITEMS; ++i)
    {
        decode_int(data_packet, &idx, &data[i].time);
        decode_int(data_packet, &idx, &data[i].value);
    }
}
