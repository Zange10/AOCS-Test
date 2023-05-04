#ifndef AOCS_TEST_INPUTSTATUS_H
#define AOCS_TEST_INPUTSTATUS_H

#endif //AOCS_TEST_INPUTSTATUS_H

#include <gtk/gtk.h>
#include "geometry.h"

// sets the index of the pressed key inside key_pressed to true
void get_key_pressed(GdkEventKey *event);

// sets the index of the pressed key inside key_pressed to false
void get_key_released(GdkEventKey *event);

// sets the index of the pressed key inside mouse_pressed to true
void get_mouse_pressed(GdkEventButton *event);

// sets the index of the pressed key inside mouse_pressed to false
void get_mouse_released(GdkEventButton *event);

// returns the mouse's position in the window as 2D-point with integer values
Point2D_int get_mouse_pos(GtkWidget *widget);

// change the observer's location stored in observer according to the pressed keys
void move(Vector *looking, Vector *observer);

// change the observer's looking direction stored in looking according to the pressed keys, pressed mouse buttons and mouse position
void rotate(GtkWidget *widget, Vector *looking, int WINDOW_WIDTH, int WINDOW_HEIGHT);