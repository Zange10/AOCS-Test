#include "geometry.h"
#include <cairo.h>
#include <math.h>

Vector *p_looking;
Point3D *p_observer;
int *p_WINDOW_WIDTH;
int *p_WINDOW_HEIGHT;
double *p_DISTANCE;


void init_drawing(Vector *look, Point3D *obs, int *win_width, int *win_height, double *distance) {
    p_looking = look;
    p_observer = obs;
    p_WINDOW_WIDTH = win_width;
    p_WINDOW_HEIGHT = win_height;
    p_DISTANCE = distance;
}


Point2D p3d_to_p2d(Point3D p3d) {
    Vector looking = *p_looking;
    Point3D observer = *p_observer;
    int WINDOW_WIDTH = *p_WINDOW_WIDTH;
    int WINDOW_HEIGHT = *p_WINDOW_HEIGHT;

    Point2D p2d;
    Vector v3d = getVector(observer, p3d);
    Vector proj_p3d = get_vector_projection(v3d, looking);
    Vector observer_plane_xy = {0,0,0};
    if(looking.x != 0 && looking.y != 0) {
        observer_plane_xy.x = 1;
        observer_plane_xy.y = -looking.x/looking.y;
    } else if (looking.x == 0) {
        observer_plane_xy.x = 1;
    } else if (looking.y == 0) {
        observer_plane_xy.y = 1;
    }
    Vector observer_plane_xyz = cross_product(looking,observer_plane_xy);
    Vector nadir = {p3d.x-proj_p3d.x-observer.x,
                    p3d.y-proj_p3d.y-observer.y,
                    p3d.z-proj_p3d.z-observer.z};

    double s = 0;
    double nadir_length = get_vector_length(nadir);

    if(nadir_length != 0) {
        double a = get_vector_angle(v3d,looking);
        s = (*p_DISTANCE/cos(a))/get_vector_length(v3d);
    }


    Vector cross_point = scalar_multiplication(s,nadir);
    Vector plane_proj_x = get_vector_projection(cross_point,observer_plane_xy);
    Vector plane_proj_y = get_vector_projection(cross_point,observer_plane_xyz);

    if(get_vector_angle(observer_plane_xy,nadir) < M_PI/2)
        p2d.x = get_vector_length(plane_proj_x) + WINDOW_WIDTH/2;
    else
        p2d.x = -get_vector_length(plane_proj_x) + WINDOW_WIDTH/2;

    if(get_vector_angle(observer_plane_xyz,nadir) < M_PI/2)
        p2d.y = get_vector_length(plane_proj_y) + WINDOW_HEIGHT/2;
    else
        p2d.y = -get_vector_length(plane_proj_y) + WINDOW_HEIGHT/2;

    // if looking to right half everything mirrors because it does don't ask (looking vector...)
    if(looking.y > 0) {
        p2d.x = -(p2d.x - WINDOW_WIDTH/2) + WINDOW_WIDTH/2;
        p2d.y = -(p2d.y - WINDOW_HEIGHT/2) + WINDOW_HEIGHT/2;
    }

    return p2d;
}

void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2) {
    cairo_set_line_width(cr, 1);

    cairo_move_to(cr, p1.x, p1.y);
    cairo_line_to(cr, p2.x, p2.y);
    cairo_stroke(cr);
}


void draw_face(cairo_t *cr, Point3D CoM, Point3D points[4], Point3D lightsource) {
    Vector looking = *p_looking;
    Point3D observer = *p_observer;

    if(get_vector_length(getVector(points[0], points[1])) > 100 * 1.1) {
        Point3D tempP = points[1];
        points[1] = points[2];
        points[2] = tempP;
    }
    if(get_vector_length(getVector(points[1], points[2])) > 100 * 1.1) {
        Point3D tempP = points[2];
        points[2] = points[3];
        points[3] = tempP;
    }

    Point3D center;
    center.x = (points[0].x + points[1].x + points[2].x + points[3].x)/4;
    center.y = (points[0].y + points[1].y + points[2].y + points[3].y)/4;
    center.z = (points[0].z + points[1].z + points[2].z + points[3].z)/4;

    Vector CoM2c = getVector(CoM,center);
    Vector o2c = getVector(observer,center);
    Vector ls2c = getVector(lightsource,center);

    double a = get_vector_angle(CoM2c,o2c);
    double a_looking = get_vector_angle(o2c,looking);
    if(a < M_PI/2 || a_looking > M_PI/2) return;

    Point2D p2d[6];
    p2d[0] = p3d_to_p2d(center);
    p2d[1] = p3d_to_p2d(CoM);


    for(int i = 0; i < 4; i++) {
        Vector tempv = getVector(observer,points[i]);
        if(get_vector_angle(tempv,looking) > M_PI/2) return;
        p2d[i+2] = p3d_to_p2d(points[i]);
    }

    double a_ls = get_vector_angle(CoM2c, ls2c);
    double light = 0.5;
    if(a_ls > M_PI/2) {
        light += cos(a_ls+M_PI)*0.3;
    }

    cairo_set_source_rgb(cr, light/2-0.5, light, light/2-0.5);
    cairo_move_to(cr, p2d[5].x, p2d[5].y);
    for(int i = 0; i < 3; i++) {
        cairo_line_to(cr, p2d[i+2].x, p2d[i+2].y);
    }
    cairo_close_path(cr);
    cairo_fill(cr);


    cairo_set_source_rgb(cr, 0, 0.2, 0);
    draw_stroke(cr, p2d[5], p2d[2]);
    for(int i = 0; i < 3; i++) {
        draw_stroke(cr, p2d[2+i], p2d[3+i]);
    }
}

void draw_lightsource(cairo_t *cr, Point3D lightsource) {
    Vector looking = *p_looking;
    Point3D observer = *p_observer;

    double a_looking = get_vector_angle(getVector(observer,lightsource),looking);
    if(a_looking > M_PI/2) return;
    Point2D p2d = p3d_to_p2d(lightsource);
    double amt_of_rays = 20;
    for(int i = 0; i < amt_of_rays; i++) {
        cairo_set_source_rgba(cr, 0.8, 0.8, 0.8,(pow(i,4))/(pow(amt_of_rays,4)*1.4));
        double radius = (695508000.0*(-8*i/amt_of_rays+8.0))/(get_vector_length(getVector(observer,lightsource)));
        cairo_arc(cr,p2d.x,p2d.y,radius,0,M_PI*2);
        cairo_stroke(cr);
    }
    cairo_set_source_rgb(cr, 1, 1, 1);
    double radius = 695508000/(get_vector_length(getVector(observer,lightsource)));
    cairo_arc(cr,p2d.x,p2d.y,radius,0,M_PI*2);
    cairo_fill(cr);
}