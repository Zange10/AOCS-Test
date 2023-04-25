#include <gtk/gtk.h>
#include <time.h>
#include <math.h>
//#include "geometry.h"
#include "movement.h"
#include "drawing.h"


typedef struct {
    double length;
    double pitch, yaw, roll;
    Point3D p;
} Cube;

int xg = 1,yg = 1,wg = 1,hg = 1;
int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
int frames = 0;
gboolean run = TRUE;
const int FPS = 60;
double lasttime;
time_t rawtime;
double DISTANCE = 1000;

Cube cube1 = {100, 0, 0, 0, 400, 0, 0};
Cube cube2 = {100, M_PI/6, 0, 0, 1000, -1000, 0};
Cube cube3 = {100, M_PI/5, 0, 0, -1000, 300, 0};

//Point3D observer = {350,190,0};
Point3D observer = {0,0,0};
Point3D lightsource = {0,-150000000,0};
Vector looking = {1,0,0};
//Vector looking = {-0.392683,-0.91388,0};
//Vector looking = {1,0,0};




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

void draw_faces(cairo_t *cr, Cube cube) {
    Point3D * p3d = getCubePoints(cube.p,cube.length,cube.pitch,cube.yaw,cube.roll);
    Point3D p_temp[4];

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[i+j*2];
        }
        draw_face(cr,cube.p,p_temp, lightsource);
    }
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[j+i*4];
        }
        draw_face(cr,cube.p,p_temp, lightsource);
    }

    for(int i = 0; i < 2; i++) {
        p_temp[0] = p3d[2*i+0];
        p_temp[1] = p3d[2*i+1];
        p_temp[2] = p3d[2*i+4];
        p_temp[3] = p3d[2*i+5];
        draw_face(cr,cube.p,p_temp, lightsource);
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
    draw_lightsource(cr, lightsource);
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
    init_drawing(&looking, &observer, &WINDOW_WIDTH, &WINDOW_HEIGHT, &DISTANCE);

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
