#include <gtk/gtk.h>
#include <time.h>
#include <math.h>
//#include "geometry.h"
#include "movement.h"
#include "drawing.h"
#include "connector.h"


typedef struct{
    double height, width, length;
    double pitch, yaw, roll;
    Point3D p;
} Cube;

int xg = 1,yg = 1,wg = 1,hg = 1;
int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
int frames = 0;
gboolean run = TRUE;
const int FPS = 60;
double last_time;
time_t raw_time;
double DISTANCE = 1000;

Cube sat = {.3, .1, .1,  0, 0, 0, 2, 0, 0};
Color sat_color = {0.8, .4, 0.2};
Cube pan = { .3, .3, .002, 0,0,0,2,0,0};
Color pan_color = {0, 0.2, 0.7};

//Point3D observer = {350,190,0};
Point3D observer = {0,0,0};
Point3D light_source = {0, -150000000, 0};
Vector looking = {1,0,0};
//Vector looking = {-0.392683,-0.91388,0};
//Vector looking = {1,0,0};



// returns the 3D-points of the given cube
Point3D * get_cube_points(Cube cube) {
    static Point3D corners[8];
    double cos_pitch = cos(cube.pitch), sin_pitch = sin(cube.pitch);
    double cos_yaw = cos(cube.yaw), sin_yaw = sin(cube.yaw);
    double cos_roll = cos(cube.roll), sin_roll = sin(cube.roll);

    double R[3][3] = {
            {cos_yaw * cos_roll,                                    cos_yaw * sin_roll,                                    -sin_yaw},
            {sin_pitch * sin_yaw * cos_roll - cos_pitch * sin_roll, sin_pitch * sin_yaw * sin_roll + cos_pitch * cos_roll, cos_yaw * sin_pitch},
            {cos_pitch * sin_yaw * cos_roll + sin_pitch * sin_roll, cos_pitch * sin_yaw * sin_roll - sin_pitch * cos_roll, cos_pitch * cos_yaw}
    };

    for (int i = 0; i < 8; i++) {
        double x = (i & 1) ? cube.width : -cube.width;
        double y = (i & 2) ? cube.length : -cube.length;
        double z = (i & 4) ? cube.height : -cube.height;

        double rotated_y = R[0][0] * x + R[0][1] * y + R[0][2] * z;
        double rotated_x = R[1][0] * x + R[1][1] * y + R[1][2] * z;
        double rotated_z = R[2][0] * x + R[2][1] * y + R[2][2] * z;
 
        corners[i].x = cube.p.x + rotated_x;
        corners[i].y = cube.p.y + rotated_y;
        corners[i].z = cube.p.z + rotated_z;
    }

    return corners;
}

// draw the visible faces of the given cube in the given color
void draw_faces(cairo_t *cr, Cube cube, Color color) {
    Point3D * p3d = get_cube_points(cube);
    Point3D p_temp[4];

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[i+j*2];
        }
        draw_face(cr, cube.p, p_temp, light_source, color);
    }
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[j+i*4];
        }
        draw_face(cr, cube.p, p_temp, light_source, color);
    }

    for(int i = 0; i < 2; i++) {
        p_temp[0] = p3d[2*i+0];
        p_temp[1] = p3d[2*i+1];
        p_temp[2] = p3d[2*i+4];
        p_temp[3] = p3d[2*i+5];
        draw_face(cr, cube.p, p_temp, light_source, color);
    }
}

// draw lines between the given cube's points
void draw_skeleton(GtkWidget *widget, cairo_t *cr, gpointer data, Cube cube) {
    Point3D * p3d = get_cube_points(cube);
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

    //light_source = observer;

    sat.pitch = get_pitch();
    sat.yaw = get_yaw();
    sat.roll = get_roll();
    printf("%d \n", sat.pitch);

    Point3D * p3d = get_cube_points(sat);
    int offset = 2;
    pan.p.x = (p3d[0].x + p3d[1].x + p3d[2+offset].x + p3d[3+offset].x)/4;
    pan.p.y = (p3d[0].y + p3d[1].y + p3d[2+offset].y + p3d[3+offset].y)/4;
    pan.p.z = (p3d[0].z + p3d[1].z + p3d[2+offset].z + p3d[3+offset].z)/4;
    pan.pitch = sat.pitch;
    pan.yaw = sat.yaw;
    pan.roll = sat.roll;

    //draw_skeleton(widget, cr, data, sat);
    draw_light_source(cr, light_source);
    if(get_vector_length(getVector(observer, sat.p)) <
            get_vector_length(getVector(observer, pan.p))) {
        draw_faces(cr, pan, pan_color);
        draw_faces(cr, sat, sat_color);
    } else {
        draw_faces(cr, sat, sat_color);
        draw_faces(cr, pan, pan_color);
    }

    move(&looking, &observer);
    rotate(widget, &looking, WINDOW_WIDTH, WINDOW_HEIGHT);

    return FALSE;
}

static gboolean on_timeout(gpointer data) {
    raw_time = time(NULL);
    if(raw_time != last_time) {
        printf("%d\n", frames);
        frames = 0;
    }
    frames++;
    last_time = raw_time;
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
