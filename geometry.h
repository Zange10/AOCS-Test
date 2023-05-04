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

// convert radians to degrees
double rad2deg(double rad);

// print vector parameters into console: [x; y; z]
void print_vector(Vector v);

// print 3D point parameters into console: [x; y; z]
void print_point(Point3D point);

// print 2D point parameters into console: [x; y]
void print_point2D(Point2D point);

// returns a vector pointing from point p1 to point p2
Vector getVector(Point3D p1, Point3D p2);

// returns the length of vector v
double get_vector_length(Vector v);

// returns the result of the dot product of vectors v1 and v2
double dot_product(Vector v1, Vector v2);

// returns the result of the cross product v1 x v2
Vector cross_product(Vector v1, Vector v2);

// returns the vector v multiplied by the scalar s
Vector scalar_multiplication(double s, Vector v);

// returns the vector v that is the projection of v1 onto v2
Vector get_vector_projection(Vector v1, Vector v2);

// returns a vector with the direction of v and the length of 1
Vector normalize_vector(Vector v);

// returns the angle between vectors v1 and v2
double get_vector_angle(Vector v1, Vector v2);