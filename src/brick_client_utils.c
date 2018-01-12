#include "headers/brick_client_utils.h"


#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

const int DEFAULT_AMP = 50;
const int DEFAULT_OFFSET = 0;
const int DEFAULT_DURATION = 20000;
const int DEFAULT_SAMP_TIME = 4;
const int DEFAULT_FREQUENCY = 1;
const char DEFAULT_FILE[] = "data.txt";

int process_data(FILE *data_file, data_record_t incoming_data[])
{
  int i;

  // reached end of simulation
  if (incoming_data[0].time==END_SIMULATION.time && incoming_data[0].value==END_SIMULATION.value)
  {
    return -1;
  }
  else
  {
    for (i=0; i<MAX_DATA_ITEMS; ++i)
    {
      if (incoming_data[i].time == 0 && incoming_data[i].value == 0)
      {
		continue;
      }
      fprintf(data_file, "%d %d\n", incoming_data[i].time, incoming_data[i].value);
    }
    return 0;
  }
}

extern char *optarg;
extern int optind, opterr, optopt;

/*  This is bad. As far as I know these are 4 global library variables needed
 *  by getopt. We will leave these here.
 */
extern char *optarg;
extern int optind, opterr, optopt;

/*  This variable contains every option we might pass via command line and also
 *  if that option should have an argument or not (The ":" means that the
 *  precedent option needs an argument).
 */
static const char optstring[] = "m:hlt:a:o:d:s:f:F:";

/*  This is almost the same as optstring[], but it contains the extended options
 *  (The ones that can be called using "--" instead of "-") and what they mean.
 */
static const struct option longopts[] = {
  {"mac", 1, NULL, 'm'},            // MAC Address
  {"list-devices", 0, NULL, 'l'},   // BT Scan only?
  {"help", 0, NULL, 'h'},           // Display help
  {"type", 0, NULL, 't'},           // Simulation type
  {"amp", 0, NULL, 'a'},            // Amplitude
  {"offset", 0, NULL, 'o'},         // Offset
  {"duration", 0, NULL, 'd'},       // Duration
  {"samp", 0, NULL, 's'},           // Sampling time
  {"freq", 0, NULL, 'f'},           // Frequency
  {"file", 0, NULL, 'F'},           // File
  {NULL, 0, NULL, 0}
};

// The help description.
static const char help [] =
"\n" "Brick Client" "\n"
"Usage: %s [options]\n\n"
"Available options\n\n"
"  --mac={mac_address} | -m {mac_address}\n"
"        Specify MAC address of the NXT Brick to connect to\n\n"
"  --list-devices | -l\n"
"        Print a List of BT Devices in range\n\n"
"  --type={sim_type} | -t {sim_type}\n"
"        Specify the type of input function used for the simulation (b: step function, s: sin function)\n\n"
"  --amp={amp} | -a {amp}\n"
"        Specify the amplitude of the input function\n\n"
"  --offset={offset} | -o {offset}\n"
"        Specify the offset of the input function\n\n"
"  --duration={dur} | -d {dur}\n"
"        Specify the duration of the simulation\n\n"
"  --samp={samp} | -s {samp}\n"
"        Specify the sampling time of the simulation\n\n"
"  --freq={freq} | -f {freq}\n"
"        Specify the frequency of the input sinusoid in tenths of hertz (valid only when the input function is a sinusoid)\n\n"
"  --file={filename} | -F {filename}\n"
"        Specify the name of the file where the simulation data is stored\n\n"
"  --help  | -h\n"
"        Print this help\n";

/*  This function checks if a MAC address passed via command line is correct
 *  and then converts it to a bdaddr_t for future uses. (Like the
 *  bro_bt_connect_device)
 */
static
int chk_mac_addr (char * target, bdaddr_t * mac) {

    unsigned b0, b1, b2, b3, b4, b5;
    char buffer[18];

    if (sscanf(target, "%2x:%2x:%2x:%2x:%2x:%2x",
               &b0, &b1, &b2, &b3, &b4, &b5) == 6) {
        sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                b0, b1, b2, b3, b4, b5);
        return str2ba(buffer, mac);
    } else {
        return -1;
    }
};

static inline
void print_help (const char *progname)
{
    fprintf(stderr, help, progname);
}

int opts_parse_config(sim_client_options *options, int argc, char* argv[])
{
    int opt;
    brick_bt_device_t *devices[MAX_BT_DEVICES];
    size_t ndevs;
    int i;
    char conv_mac[18];

    int amp = DEFAULT_AMP, offset = DEFAULT_OFFSET, duration = DEFAULT_DURATION, samp_time = DEFAULT_SAMP_TIME;
    int freq = DEFAULT_FREQUENCY;

    char filename[FILENAME_MAX_LEN];
    strcpy(filename, DEFAULT_FILE);

    sim_type_t sim_type = SIM_STEP;

    while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch (opt) {
	case 'm':
	  if (chk_mac_addr(optarg, &options->mac) == -1){
	    return -1;
	  }
	  break;
	case 'l':
	  ndevs = brick_bt_scan_devices(devices);
	  fprintf(stderr, "Bluetooth Devices in Range:\n\n");
	  for (i = 0; i < ndevs; i++) {
	    ba2str(&devices[i]->mac, conv_mac);
	    fprintf(stderr, "%d: %s [%s]\n", i+1, devices[i]->name,
		    conv_mac);
	  };
	  return -1;
	case 't':
	  if (optarg[0] == 'b') { sim_type = SIM_STEP; }
	  else if (optarg[0] == 's') { sim_type = SIM_SIN; }
	  else
	  {
	    fprintf(stderr, "Unknown function type: %s\n", optarg);
	    return -1;
	  }
	  break;
	case 'a':
	  amp = atoi(optarg);
	  break;
	case 'o':
	  offset = atoi(optarg);
	  break;
	case 'd':
	  duration = atoi(optarg);
	  break;
	case 's':
	  samp_time = atoi(optarg);
	  break;
	case 'f':
	  freq = atof(optarg);
	  break;
    case 'F':
      strcpy(filename, optarg);
      break;
	case 'h':
	case '?':
	  print_help(argv[0]);
	  return -1;
        }
    }

    strcpy(options->filename, filename);

    switch (sim_type)
    {
    case SIM_STEP:
      create_step_config(&options->config, amp, offset, duration, samp_time);
      break;
    case SIM_SIN:
      create_sin_config(&options->config, amp, offset, duration, samp_time, freq);
      break;
    }

    return 0;
}
