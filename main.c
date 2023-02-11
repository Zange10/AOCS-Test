#include <gtk/gtk.h>
#include <cairo.h>
#include <time.h>
#include <math.h>

typedef struct {
    int x,y,height,width;
} DIM;

typedef struct {
    double x,y,z;
} Point3D;

typedef struct {
    double x,y,z;
} Vector;

typedef struct {
    double x,y;
} Point2D;

typedef struct {
    int length;
    double pitch, yaw, roll;
    Point3D p;
} Cube;


gboolean key_pressed[6] = {FALSE,FALSE,FALSE,FALSE, FALSE, FALSE};

int xg = 1,yg = 1,wg = 1,hg = 1;
int WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
double DISTANCE = 2000;
int frames = 0;
const int FPS = 60;
double lasttime;
time_t rawtime;

Cube cube = {100,0,0, 0, 400,0,0};

Point3D origin = {0,0,0};
Point3D lightsource = {0,-500,0};

void print_point(Point3D point) {
    printf("[%lf; %lf; %lf]\n", point.x, point.y, point.z);
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

Vector normalize_vector(Vector v) {
    double length = get_vector_length(v);
    v.x /= length;
    v.y /= length;
    v.z /= length;
    return v;
}

double get_vector_angle(Vector v1, Vector v2) {
    return acos( dot_product(v1,v2) / (get_vector_length(v1)*get_vector_length(v2)) );
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



void draw_face(cairo_t *cr, Point3D observer, Point3D CoM, Point3D points[4]) {
    if(get_vector_length(getVector(points[0], points[1])) > cube.length*1.1) {
        Point3D tempP = points[1];
        points[1] = points[2];
        points[2] = tempP;
    }
    if(get_vector_length(getVector(points[1], points[2])) > cube.length*1.1) {
        Point3D tempP = points[2];
        points[2] = points[3];
        points[3] = tempP;
    }

    Point3D center;
    center.x = (points[0].x + points[1].x + points[2].x + points[3].x)/4;
    center.y = (points[0].y + points[1].y + points[2].y + points[3].y)/4;
    center.z = (points[0].z + points[1].z + points[2].z + points[3].z)/4;

    Vector CoM2c = getVector(CoM,center);
    Vector o2c = getVector(origin,center);
    Vector ls2c = getVector(lightsource,center);

    double a = get_vector_angle(CoM2c,o2c);
    if(a < M_PI/2) return;
    
    Point2D p2d[6];
    p2d[0].x = (center.y-origin.y)/(center.x-origin.x) * DISTANCE + WINDOW_WIDTH/2;
    p2d[0].y = (center.z-origin.z)/(center.x-origin.x) * DISTANCE + WINDOW_HEIGHT/2;

    p2d[1].x = (CoM.y-origin.y)/(CoM.x-origin.x) * DISTANCE + WINDOW_WIDTH/2;
    p2d[1].y = (CoM.z-origin.z)/(CoM.x-origin.x) * DISTANCE + WINDOW_HEIGHT/2;

    for(int i = 0; i < 4; i++) {
        p2d[i+2].x = (points[i].y-origin.y)/(points[i].x-origin.x) * DISTANCE + WINDOW_WIDTH/2;
        p2d[i+2].y = (points[i].z-origin.z)/(points[i].x-origin.x) * DISTANCE + WINDOW_HEIGHT/2;
    }

    double a_ls = get_vector_angle(CoM2c, ls2c);
    double light = 0.5;
    if(a_ls > M_PI/2) {
        light += cos(a_ls+M_PI)*0.4;
    }

    cairo_set_source_rgb(cr, 0, light, 0);
    cairo_move_to(cr, p2d[5].x, p2d[5].y);
    for(int i = 0; i < 3; i++) {
        cairo_line_to(cr, p2d[i+2].x, p2d[i+2].y);
    }
    cairo_close_path(cr);
    cairo_fill(cr);

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

void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2) {
    cairo_set_line_width(cr, 1);

    cairo_move_to(cr, p1.x, p1.y);
    cairo_line_to(cr, p2.x, p2.y);
    cairo_stroke(cr);
}


void draw_skeleton(GtkWidget *widget, cairo_t *cr, gpointer data) {
    Point3D * p3d = getCubePoints(cube.p,cube.length,cube.pitch,cube.yaw,cube.roll);
    Point2D p2d[8];

    for(int i = 0; i < 8; i++) {
        p2d[i].x = (p3d[i].y-origin.y)/(p3d[i].x-origin.x) * DISTANCE + WINDOW_WIDTH/2;
        p2d[i].y = (p3d[i].z-origin.z)/(p3d[i].x-origin.x) * DISTANCE + WINDOW_HEIGHT/2;
    }

    cairo_set_source_rgb(cr, 1, 1, 1);
    for(int i = 0; i < 8; i++) {
        for(int j = i; j < 8; j++) {
            if(i == j) continue;
            if(i+j==7) continue;
            draw_stroke(cr,p2d[i],p2d[j]);
        }
    }
};

void draw_faces(cairo_t *cr, Cube cube) {
    Point3D * p3d = getCubePoints(cube.p,cube.length,cube.pitch,cube.yaw,cube.roll);
    Point2D p2d[8];
    Point3D p_temp[4];

    for(int i = 0; i < 8; i++) {
        p2d[i].x = (p3d[i].y-origin.y)/(p3d[i].x-origin.x) * DISTANCE + WINDOW_WIDTH/2;
        p2d[i].y = (p3d[i].z-origin.z)/(p3d[i].x-origin.x) * DISTANCE + WINDOW_HEIGHT/2;
    }

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[i+j*2];
        }
        draw_face(cr,origin,cube.p,p_temp);
    }

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            p_temp[j] = p3d[j+i*4];
        }
        draw_face(cr,origin,cube.p,p_temp);
    }
    
    for(int i = 0; i < 2; i++) {
            p_temp[0] = p3d[2*i+0];
            p_temp[1] = p3d[2*i+1];
            p_temp[2] = p3d[2*i+4];
            p_temp[3] = p3d[2*i+5];
        draw_face(cr,origin,cube.p,p_temp);
    }
}

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->button == 1) {
        g_print("Left mouse button pressed at (%f, %f)\n", event->x, event->y);
    }
    return FALSE;
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if(event->keyval == GDK_KEY_w) key_pressed[0] = TRUE;
    if(event->keyval == GDK_KEY_s) key_pressed[1] = TRUE;
    if(event->keyval == GDK_KEY_a) key_pressed[2] = TRUE;
    if(event->keyval == GDK_KEY_d) key_pressed[3] = TRUE;
    if(event->keyval == GDK_KEY_q) key_pressed[4] = TRUE;
    if(event->keyval == GDK_KEY_e) key_pressed[5] = TRUE;
    return TRUE;
}

gboolean key_release_callback(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if(event->keyval == GDK_KEY_w) key_pressed[0] = FALSE;
    if(event->keyval == GDK_KEY_s) key_pressed[1] = FALSE;
    if(event->keyval == GDK_KEY_a) key_pressed[2] = FALSE;
    if(event->keyval == GDK_KEY_d) key_pressed[3] = FALSE;
    if(event->keyval == GDK_KEY_q) key_pressed[4] = FALSE;
    if(event->keyval == GDK_KEY_e) key_pressed[5] = FALSE;
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

    //cube.pitch  += M_PI/200;
    cube.yaw    += M_PI/250;
    cube.roll   += M_PI/200;
    
    cube.pitch  = M_PI/12;
    //cube.yaw    = M_PI/4;
    //cube.roll   = M_PI/15;

    if(cube.roll > M_PI*2) cube.roll -= M_PI*2;
    //draw_skeleton(widget, cr, data);
    draw_faces(cr, cube);
    return FALSE;
}

static gboolean on_timeout(gpointer data)
{
    double move = 2;
    if(key_pressed[0]) origin.x += move;
    if(key_pressed[1]) origin.x -= move;
    if(key_pressed[2]) origin.y -= move;
    if(key_pressed[3]) origin.y += move;
    if(key_pressed[4]) origin.z -= move;
    if(key_pressed[5]) origin.z += move;


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

int main(int argc, char *argv[])
{
    GtkWidget *window, *drawing_area;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "My GTK Window");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press_event), NULL);
    g_signal_connect(G_OBJECT(window), "key-release-event", G_CALLBACK(key_release_callback), NULL);

    drawing_area = gtk_drawing_area_new();
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    gtk_widget_show_all(window);
    g_timeout_add(1.0/FPS*1000.0, on_timeout, drawing_area);
    gtk_main();

    return 0;
}
