#ifndef BT_COMM_H
#define BT_COMM_H

#include "simul_config.h"

/** max number of items in the buffer */ 
#define MAX_DATA_ITEMS 16

/** max size of a configuration message */
#define MAX_CONF_SIZE 128

/** max size of a data message */
#define MAX_DATA_SIZE MAX_DATA_ITEMS*sizeof(data_record_t)

/** structure of data items send from the brick to the PC, composed by the
 *  simulation time and the read value
 */
typedef struct
{
  int time;
  int value;
} data_record_t;

/** a configuration message can contain some configuration parameters, or 
 *  it can be used to send a special message to start the simulation after the
 *  configuration has been completed
 */
typedef enum {MSG_CONF, MSG_START_SIM} msg_type_t;

/** special data message indicating the end of the simulation. */
extern const data_record_t END_SIMULATION;

/** set of functions to encode and decode configuration messages to byte arrays that can be transferred
 *  between the brick and the PC.
 */
sim_status_t decode_config_msg(const uint8_t *conf_packet, sim_config_t* sim_config);
void encode_sim_start_msg(uint8_t conf_packet[MAX_CONF_SIZE]);
void encode_sim_config_msg(uint8_t conf_packet[MAX_CONF_SIZE], const sim_config_t* sim_config);
void decode_config(const uint8_t *conf_data, sim_config_t* sim_config);

void encode_int(uint8_t *conf_data, int *idx, int value);
void decode_int(const uint8_t *conf_data, int *idx, int *dst);

/** set of functions to encode and decode data messages to byte arrays that can be transferred
 *  between the brick and the PC.
 */
void encode_sim_data_msg(uint8_t data_packet[MAX_DATA_SIZE], const data_record_t data[MAX_DATA_ITEMS]);
void decode_sim_data_msg(const uint8_t data_packet[MAX_DATA_SIZE], data_record_t data[MAX_DATA_ITEMS]);

#endif
