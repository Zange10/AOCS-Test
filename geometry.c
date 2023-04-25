#include <stdio.h>
#include <math.h>
#include "geometry.h"

double rad2deg(double rad) {
    return rad/(2*M_PI) * 360;
}

void print_vector(Vector v) {
    printf("[%lf; %lf; %lf]\n", v.x, v.y, v.z);
}
void print_point(Point3D point) {
    printf("[%lf; %lf; %lf]\n", point.x, point.y, point.z);
}
void print_point2D(Point2D point) {
    printf("[%lf; %lf]\n", point.x, point.y);
}

Vector getVector(Point3D p1, Point3D p2) {
    Vector v;
    v.x = p2.x-p1.x;
    v.y = p2.y-p1.y;
    v.z = p2.z-p1.z;
    return v;
}

double get_vector_length(Vector v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double dot_product(Vector v1, Vector v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector cross_product(Vector v1, Vector v2) {
    Vector v;
    v.x = v1.y*v2.z-v1.z*v2.y;
    v.y = v1.z*v2.x-v1.x*v2.z;
    v.z = v1.x*v2.y-v1.y*v2.x;
    return v;
}

Vector scalar_multiplication(double s, Vector v) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

// v1 on v2
Vector get_vector_projection(Vector v1, Vector v2) {
    double v2_length = get_vector_length(v2);
    double scalar = dot_product(v1, v2) / (v2_length*v2_length);
    return scalar_multiplication(scalar, v2);
}

Vector normalize_vector(Vector v) {
    double length = get_vector_length(v);
    v.x /= length;
    v.y /= length;
    v.z /= length;
    return v;
}

double get_vector_angle(Vector v1, Vector v2) {
    double quotient = dot_product(v1,v2) / (get_vector_length(v1)*get_vector_length(v2));
    if(quotient > 1) quotient = 1;  // catch 1.0000001...
    else if(quotient < -1) quotient = -1;   // catch -1.0000001...
    return acos( quotient );
}