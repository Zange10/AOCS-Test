#ifndef AOCS_TEST_INPUTSTATUS_H
#define AOCS_TEST_INPUTSTATUS_H

#endif //AOCS_TEST_INPUTSTATUS_H

#include <gtk/gtk.h>
#include "geometry.h"


void get_key_pressed(GdkEventKey *event);

void get_key_released(GdkEventKey *event);

void get_mouse_pressed(GdkEventButton *event);

void get_mouse_released(GdkEventButton *event);

Point2D_int get_mouse_pos(GtkWidget *widget);


void move(Vector *looking, Vector *observer);

void rotate(GtkWidget *widget, Vector *looking, int WINDOW_WIDTH, int WINDOW_HEIGHT);