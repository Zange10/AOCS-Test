#ifndef AOCS_TEST_DRAWING_H
#define AOCS_TEST_DRAWING_H

#endif //AOCS_TEST_DRAWING_H

#include <gtk/gtk.h>


void init_drawing(Vector *look, Point3D *obs, int *win_width, int *win_height, double *distance);

Point2D p3d_to_p2d(Point3D p3d);

void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2);


void draw_face(cairo_t *cr, Point3D CoM, Point3D points[4], Point3D lightsource);


void draw_skeleton(GtkWidget *widget, cairo_t *cr, gpointer data);

void draw_lightsource(cairo_t *cr, Point3D lightsource);