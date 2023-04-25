#ifndef AOCS_TEST_GEOMETRY_H
#define AOCS_TEST_GEOMETRY_H

#endif //AOCS_TEST_GEOMETRY_H


typedef struct {
    int x,y;
} Point2D_int;

typedef struct {
    double x,y;
} Point2D;

typedef struct {
    double x,y,z;
} Point3D;

typedef struct {
    double x,y,z;
} Vector;


double rad2deg(double rad);

void print_vector(Vector v);

void print_point(Point3D point);

void print_point2D(Point2D point);

Vector getVector(Point3D p1, Point3D p2);

double get_vector_length(Vector v);

double dot_product(Vector v1, Vector v2);

Vector cross_product(Vector v1, Vector v2);

Vector scalar_multiplication(double s, Vector v);

Vector get_vector_projection(Vector v1, Vector v2);

Vector normalize_vector(Vector v);

double get_vector_angle(Vector v1, Vector v2);