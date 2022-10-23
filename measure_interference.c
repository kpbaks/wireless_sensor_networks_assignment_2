// REF: https://contiki-ng.readthedocs.io/en/develop/_api/group__radio.html

#include "contiki.h"

#include "sys/log.h"
#define LOG_MODULE "MCI" // MCI = Measure Channel Interference
#define LOG_LEVEL LOG_LEVEL_INFO

#include "dev/radio.h"
#include "net/netstack.h"

#define NUM_SAMPLES_PER_CHANNEL 100
#define TIMEOUT 2 // seconds


#define CHANNEL_MIN 11 // part of the IEEE 802.15.4 spec
#define CHANNEL_MAX 26 // part of the IEEE 802.15.4 spec
#define NUM_CHANNELS (CHANNEL_MAX - CHANNEL_MIN) + 1

typedef struct interference_measurement_t {
	uint8_t id;			// 11 - 26
	radio_value_t rssi; // dBm

} interference_measurement_t;

// lower dBm comparator
int lower_dbm(const interference_measurement_t* a, const interference_measurement_t* b) {
	uint8_t a_dbm = a->rssi;
	uint8_t b_dbm = b->rssi;

	// -1 if a < b (a is lower) 0 if a == b 1 if a > b (a is higher)
	return a_dbm < b_dbm ? -1 : a_dbm > b_dbm ? 1 : 0;
}
 
// sort with bubblesort based on lower dBm
void sort_measurements(interference_measurement_t* measurements, uint8_t size) {
	for (uint8_t i = 0; i < size; ++i) {
		for (uint8_t j = 0; j < size - i - 1; ++j) {
			if (lower_dbm(&measurements[j], &measurements[j + 1]) > 0) {
				interference_measurement_t temp = measurements[j];
				measurements[j] = measurements[j + 1];
				measurements[j + 1] = temp;
			}
		}
	}
}

PROCESS(assignment_2, "assignment_2");
AUTOSTART_PROCESSES(&assignment_2);

PROCESS_THREAD(assignment_2, ev, data) {

	PROCESS_BEGIN();

	// used to add a timeout between measuring each channel
	// static struct etimer et;
	// etimer_set(&et, CLOCK_SECOND * TIMEOUT);

	// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	NETSTACK_RADIO.on();

	// LOG_INFO("sizeof(int) = %d\n", sizeof(int));

	radio_value_t default_channel;
	NETSTACK_RADIO.get_value(RADIO_PARAM_CHANNEL, &default_channel);
	LOG_INFO("default channel %d\n", default_channel);

	radio_value_t channel_max;
	NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MAX, &channel_max);
	LOG_INFO("channel_max %d\n", channel_max);

	radio_value_t channel_min;
	NETSTACK_RADIO.get_value(RADIO_CONST_CHANNEL_MIN, &channel_min);
	LOG_INFO("channel_min %d\n", channel_min);

	interference_measurement_t measured_inteferences[NUM_CHANNELS] = {};

	for (uint8_t i = CHANNEL_MIN; i <= CHANNEL_MAX; ++i) {

		// set channel to the next one e.g. 11 -> 12 -> 13 -> ... 26
		NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, i);

		// etimer_reset(&et);

		// measure multiple samples and then average them to get a more precise
		// value
		radio_value_t samples[NUM_SAMPLES_PER_CHANNEL] = {0};
		for (int j = 0; j < NUM_SAMPLES_PER_CHANNEL; ++j) {
			// radio_result_t _ = NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &samples[j]);
			NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &samples[j]);
		}

		// wait for the timeout to expire
		// PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		// average samples
		radio_value_t sum = 0;
		for (int j = 0; j < NUM_SAMPLES_PER_CHANNEL; ++j) {
			sum += samples[j];
		}
		radio_value_t average = sum / NUM_SAMPLES_PER_CHANNEL;

		// add the average to the interference measurements
		interference_measurement_t measurement = {.id = i, .rssi = average};
		measured_inteferences[i - CHANNEL_MIN] = measurement;
	}

	// sort measures inteferences after lowest noise
	sort_measurements(measured_inteferences, NUM_CHANNELS);


	// print the results as csv
	LOG_INFO_("\n\n");
	LOG_INFO_("rank,id,rssi\n");
	for (uint8_t i = 0; i < NUM_CHANNELS; ++i) {
		interference_measurement_t measurement = measured_inteferences[i];
		LOG_INFO_("%d,%d,%d\n", i, measurement.id, measurement.rssi);
	}	

	PROCESS_END();
}
