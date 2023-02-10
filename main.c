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
    double x,y;
} Point2D;

typedef struct {
    int length;
    double pitch, yaw, roll;
    Point3D p;
} Cube;


gboolean key_pressed[6] = {FALSE,FALSE,FALSE,FALSE, FALSE, FALSE};

int xg = 1,yg = 1,wg = 1,hg = 1;
int WINDOW_WIDTH = 2000, WINDOW_HEIGHT = 2000;
double DISTANCE = 2000;

Cube cube = {100,0,0, 0, 400,0,0};

Point3D origin = {0,0,0};

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

        double rotatedX = R[0][0] * x + R[0][1] * y + R[0][2] * z;
        double rotatedY = R[1][0] * x + R[1][1] * y + R[1][2] * z;
        double rotatedZ = R[2][0] * x + R[2][1] * y + R[2][2] * z;

        corners[i].x = center.x + rotatedX;
        corners[i].y = center.y + rotatedY;
        corners[i].z = center.z + rotatedZ;
    }

    return corners;
}



//void draw_face(GtkWidget *widget, cairo_t *cr, gpointer data, double roll) {
//    if(roll < 0) roll += 2*M_PI;
//    double cw = sin(roll+M_PI/16)*0.2;
//    if(cw<0) cw=0;
//    cairo_set_source_rgb(cr, cw+0.2, cw+0.2, cw+0.2);
//    double x1,x2,x3,x4,y1,y2,y3,y4,ay1,ay2,ay3,ay4;
//
//    x1 = sin(roll+5*M_PI/4)* sqrt(2)/2*cube.length +400;
//    x2 = sin(roll+3*M_PI/4)* sqrt(2)/2*cube.length +400;
//    x3 = x2;
//    x4 = x1;
//
//    double amax = atan((cube.length/2) / (d + cube.length * ((sqrt(2)+1)/2)));
//    double amin = atan((cube.length/2) / (d + cube.length * ((sqrt(2)-1)/2)));
//    ay1 = cos(roll+M_PI/4-M_PI)*(amax-amin)/2+(amin+amax)/2;
//    ay2 = cos(roll-M_PI/4-M_PI)*(amax-amin)/2+(amin+amax)/2;
//    ay3 = cos(roll-M_PI/4-M_PI)*(amax-amin)/2+(amin+amax)/2;
//    ay4 = cos(roll+M_PI/4-M_PI)*(amax-amin)/2+(amin+amax)/2;
//
//    y1 = 400-tan(ay1)*d;
//    y2 = 400-tan(ay2)*d;
//    y3 = 400+tan(ay3)*d;
//    y4 = 400+tan(ay4)*d;
//
//    cairo_move_to(cr, x1, y1);
//    cairo_line_to(cr, x2, y2);
//    cairo_line_to(cr, x3, y3);
//    cairo_line_to(cr, x4, y4);
//    cairo_close_path(cr);
//    cairo_fill(cr);
//}

void draw_stroke(cairo_t *cr, Point2D p1, Point2D p2) {
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_set_line_width(cr, 2);

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

    for(int i = 0; i < 8; i++) {
        for(int j = i; j < 8; j++) {
            if(i == j) continue;
            if(i+j==7) continue;
            draw_stroke(cr,p2d[i],p2d[j]);
        }
    }
};

/*void draw_cube(GtkWidget *widget, cairo_t *cr, gpointer data) {
    Point3D * points = getCubePoints(cube.p,cube.length,cube.pitch,cube.yaw,cube.roll);
    if(cube.roll < M_PI/2 || cube.roll > 3*M_PI/2) {
        draw_face(widget, cr, data, cube.roll);
    }
    if(cube.roll < M_PI) {
        draw_face(widget, cr, data, cube.roll-M_PI/2);
    }
    if(cube.roll > M_PI/2 && cube.roll < 3*M_PI/2) {
        draw_face(widget, cr, data, cube.roll-M_PI);
    }
    if(cube.roll > M_PI) {
        draw_face(widget, cr, data, cube.roll-3*M_PI/2);
    }
}*/static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
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

    cube.roll=M_PI/4;
    cube.yaw += M_PI/200; //M_PI*sin(cube.roll);
    //cube.pitch +=M_PI/200;// M_PI*2*cos(cube.roll);
    if(cube.roll > M_PI*2) cube.roll -= M_PI*2;
    //draw_cube(widget, cr, data);
    draw_skeleton(widget, cr, data);
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
    g_timeout_add(15, on_timeout, drawing_area);
    gtk_main();

    return 0;
}
