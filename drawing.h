#ifndef AOCS_TEST_DRAWING_H
#define AOCS_TEST_DRAWING_H

#endif //AOCS_TEST_DRAWING_H

#include <gtk/gtk.h>

typedef struct{
    double r,g,b;
} Color;

// get important variables' pointers for later calculations
void init_drawing(Vector *look, Point3D *obs, int *win_width, int *win_height, double *distance);

// convert the 3D-point p3d coordinates to the 2D coordinates on the screen
Point2D p3d_to_p2d(Point3D p3d);

// draw a stroke from p1 to p2 on the screen
void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2);

// draw the face of a cube with the given 3D-points of the face, light source and color on the 2D screen
void draw_face(cairo_t *cr, Point3D CoM, Point3D points[4], Point3D light_source, Color color);

// draw the light source on the screen
void draw_light_source(cairo_t *cr, Point3D light_source);