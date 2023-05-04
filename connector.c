//
// Created by niklas on 04.05.23.
//

#include "connector.h"
#include "geometry.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>


#define SHMSZ sizeof(double)

key_t key;
int shmid;
double *shm, *s;

int connected = 0;

double pitch, yaw, roll;

void init_connector() {
    connected = 0;
    pitch = 0;
    yaw = 0;
    roll = 0;
    // Generate the same key that was used in Project 1
    key = ftok("/temp/", 'a');

    // Locate the shared memory segment
    shmid = shmget(key, SHMSZ, 0666);
    if (shmid < 0) {
        perror("shmget");
        return;
    }

    // Attach the shared memory segment to our data space
    shm = (double *) shmat(shmid, NULL, 0);

    if(shm[0] != 1) return;

    connected = 1;
    printf("Connection\n");
}

void destruct_connector() {
    // Detach the shared memory segment from our data space
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    printf("Connector closed\n");
}

void update_connector() {
    if(shm[0] == 1) {
        pitch = shm[1];
        yaw = shm[2];
        roll = shm[3];
    } else {
        connected = 0;
    }
}

int is_connected() {
    return connected;
}

double get_connector_pitch() {
    return pitch;
}

double get_connector_yaw() {
    return yaw;
}

double get_connector_roll() {
    return roll;
}