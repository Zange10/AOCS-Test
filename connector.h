//
// Created by niklas on 04.05.23.
//

#ifndef AOCS_TEST_CONNECTOR_H
#define AOCS_TEST_CONNECTOR_H

#endif //AOCS_TEST_CONNECTOR_H

// initialize connector
void init_connector();

// closes shared memory
void destruct_connector();

// update the values from shared memory
void update_connector();

// returns 1 if connected and 0 if not
int is_connected();

// returns the pitch from the connector
double get_connector_pitch();

// returns the yaw from the connector
double get_connector_yaw();

// returns the roll from the connector
double get_connector_roll();