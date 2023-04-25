#include <gtk/gtk.h>
#include <cairo.h>
#include <time.h>
#include <math.h>
//#include "geometry.h"
#include "movement.h"


typedef struct {
    double length;
    double pitch, yaw, roll;
    Point3D p;
} Cube;

int xg = 1,yg = 1,wg = 1,hg = 1;
int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
double DISTANCE = 1000;
int frames = 0;
gboolean run = TRUE;
const int FPS = 60;
double lasttime;
time_t rawtime;

Cube cube1 = {100, 0, 0, 0, 400, 0, 0};
Cube cube2 = {100, M_PI/6, 0, 0, 1000, -1000, 0};
Cube cube3 = {100, M_PI/5, 0, 0, -1000, 300, 0};

//Point3D observer = {350,190,0};
Point3D observer = {0,0,0};
Point3D lightsource = {0,-150000000,0};
Vector looking = {1,0,0};
//Vector looking = {-0.392683,-0.91388,0};
//Vector looking = {1,0,0};


Point2D p3d_to_p2d(Point3D p3d) {
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
        s = (DISTANCE/cos(a))/get_vector_length(v3d);
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

Point3D * getCubePoints(Point3D center, double size, double pitch, double yaw, double roll) {
    static Point3D corners[8];
    double cosPitch = cos(pitch), sinPitch = sin(pitch);
    double cosYaw = cos(yaw), sinYaw = sin(yaw);
    double cosRoll = cos(roll), sinRoll = sin(roll);

    double R[3][3] = {
            {cosYaw * cosRoll, cosYaw * sinRoll, -sinYaw},
            {sinPitch * sinYaw * cosRoll - cosPitch * sinRoll, sinPitch * sinYaw * sinRoll + cosPitch * cosRoll, cosYaw * sinPitch},
            {cosPitch * sinYaw * cosRoll + sinPitch * sinRoll, cosPitch * sinYaw * sinRoll - sinPitch * cosRoll, cosPitch * cosYaw}
    };

    double halfSize = size / 2;
    for (int i = 0; i < 8; i++) {
        double x = (i & 1) ? halfSize : -halfSize;
        double y = (i & 2) ? halfSize : -halfSize;
        double z = (i & 4) ? halfSize : -halfSize;

        double rotatedY = R[0][0] * x + R[0][1] * y + R[0][2] * z;
        double rotatedX = R[1][0] * x + R[1][1] * y + R[1][2] * z;
        double rotatedZ = R[2][0] * x + R[2][1] * y + R[2][2] * z;
 
        corners[i].x = center.x + rotatedX;
        corners[i].y = center.y + rotatedY;
        corners[i].z = center.z + rotatedZ;
    }

    return corners;
}


void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2) {
    cairo_set_line_width(cr, 1);

    cairo_move_to(cr, p1.x, p1.y);
    cairo_line_to(cr, p2.x, p2.y);
    cairo_stroke(cr);
}


void draw_face(cairo_t *cr, Point3D observer, Point3D CoM, Point3D points[4]) {
    if(get_vector_length(getVector(points[0], points[1])) > cube1.length * 1.1) {
        Point3D tempP = points[1];
        points[1] = points[2];
        points[2] = tempP;
    }
    if(get_vector_length(getVector(points[1], points[2])) > cube1.length * 1.1) {
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


void draw_skeleton(GtkWidget *widget, cairo_t *cr, gpointer data) {
    Point3D * p3d = getCubePoints(cube1.p, cube1.length, cube1.pitch, cube1.yaw, cube1.roll);
    Point2D p2d[8];

    for(int i = 0; i < 8; i++) {
        p2d[i] = p3d_to_p2d(p3d[i]);
    }

    cairo_set_source_rgb(cr, 1, 1, 1);
    for(int i = 0; i < 8; i++) {
        for(int j = i; j < 8; j++) {
            if(i == j || i+j==7) continue;
            draw_stroke(cr,p2d[i],p2d[j]);
        }
    }
};

void draw_faces(cairo_t *cr, Cube cube) {
    Point3D * p3d = getCubePoints(cube.p,cube.length,cube.pitch,cube.yaw,cube.roll);
    Point3D p_temp[4];

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[i+j*2];
        }
        draw_face(cr,observer,cube.p,p_temp);
    }
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[j+i*4];
        }
        draw_face(cr,observer,cube.p,p_temp);
    }

    for(int i = 0; i < 2; i++) {
            p_temp[0] = p3d[2*i+0];
            p_temp[1] = p3d[2*i+1];
            p_temp[2] = p3d[2*i+4];
            p_temp[3] = p3d[2*i+5];
        draw_face(cr,observer,cube.p,p_temp);
    }
}

void draw_lightsource(cairo_t *cr) {
    double a_looking = get_vector_angle(getVector(observer,lightsource),looking);
    if(a_looking > M_PI/2) return;
    Point2D p2d = p3d_to_p2d(lightsource);
    double amt_of_rays = 150;
    for(int i = 0; i < amt_of_rays; i++) {
        cairo_set_source_rgba(cr, 0.8, 0.8, 0.8,(pow(i,4))/(pow(amt_of_rays,4)*1.4));
        double radius = (695508000.0*(-7*i/amt_of_rays+8.0))/(get_vector_length(getVector(observer,lightsource)));
        cairo_arc(cr,p2d.x,p2d.y,radius,0,M_PI*2);
        cairo_stroke(cr);
    }
    cairo_set_source_rgb(cr, 1, 1, 1);
    double radius = 695508000/(get_vector_length(getVector(observer,lightsource)));
    cairo_arc(cr,p2d.x,p2d.y,radius,0,M_PI*2);
    cairo_fill(cr);
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    get_mouse_pressed(event);
    return FALSE;
}

void on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    get_mouse_released(event);
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    get_key_pressed(event);
    return TRUE;
}

gboolean key_release_callback(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    get_key_released(event);
    return TRUE;
}



static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    WINDOW_WIDTH = allocation.width;
    WINDOW_HEIGHT = allocation.height;
    if(WINDOW_WIDTH<WINDOW_HEIGHT) DISTANCE = WINDOW_WIDTH;
    else DISTANCE = WINDOW_HEIGHT;

    cairo_rectangle(cr,0,0,WINDOW_WIDTH,WINDOW_HEIGHT);
    cairo_set_source_rgb(cr, 0,0,0);
    cairo_fill(cr);

    //lightsource = observer;

    cube1.pitch  += M_PI/250;
    //cube1.roll   -= M_PI / 250;
    cube1.yaw    += M_PI / 250;
    //cube2.roll   += M_PI / 200;

    //cube1.pitch  = M_PI/6;
    //cube1.yaw    = M_PI/4;
    //cube1.roll   = M_PI/4;

    if(cube1.roll > M_PI * 2) cube1.roll -= M_PI * 2;
    //draw_skeleton(widget, cr, data);
    draw_lightsource(cr);
    draw_faces(cr, cube3);
    draw_faces(cr, cube2);
    draw_faces(cr, cube1);

    move(&looking, &observer);
    rotate(widget, &looking, WINDOW_WIDTH, WINDOW_HEIGHT);

    return FALSE;
}

static gboolean on_timeout(gpointer data) {
    rawtime = time(NULL);
    if(rawtime != lasttime) {
        printf("%d\n", frames);
        frames = 0;
    }
    frames++;
    lasttime = rawtime;
    GtkWidget *drawing_area = GTK_WIDGET(data);
    gtk_widget_queue_draw(drawing_area);
    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *drawing_area;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "My GTK Window");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press_event), NULL);
    g_signal_connect(G_OBJECT(window), "key-release-event", G_CALLBACK(key_release_callback), NULL);
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(window, "button-release-event", G_CALLBACK(on_button_release), NULL);

    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);
    if(run) g_timeout_add(1.0/FPS*1000.0, on_timeout, drawing_area);
    gtk_main();

    return 0;
}
