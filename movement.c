#include "movement.h"
#include <math.h>

gboolean key_pressed[8] = {FALSE,FALSE,FALSE,FALSE, FALSE, FALSE, FALSE, FALSE};
gboolean mouse_pressed[3] = {FALSE,FALSE,FALSE};
gboolean mouse_visible = TRUE;
gboolean overrotation = FALSE;


void get_key_pressed(GdkEventKey *event) {
    if(event->keyval == GDK_KEY_w || event -> keyval == GDK_KEY_W) key_pressed[0] = TRUE;   // Forward
    if(event->keyval == GDK_KEY_s || event -> keyval == GDK_KEY_S) key_pressed[1] = TRUE;   // Backward
    if(event->keyval == GDK_KEY_a || event -> keyval == GDK_KEY_A) key_pressed[2] = TRUE;   // Left
    if(event->keyval == GDK_KEY_d || event -> keyval == GDK_KEY_D) key_pressed[3] = TRUE;   // Right
    if(event->keyval == GDK_KEY_q || event -> keyval == GDK_KEY_Q) key_pressed[4] = TRUE;   // rotate Left
    if(event->keyval == GDK_KEY_e || event -> keyval == GDK_KEY_E) key_pressed[5] = TRUE;   // rotate right
    if(event->keyval == GDK_KEY_Shift_L) key_pressed[6] = TRUE;                             // Up
    if(event->keyval == GDK_KEY_Control_L) key_pressed[7] = TRUE;                           // Down
}

void get_key_released(GdkEventKey *event) {
    if(event->keyval == GDK_KEY_w || event -> keyval == GDK_KEY_W) key_pressed[0] = FALSE;
    if(event->keyval == GDK_KEY_s || event -> keyval == GDK_KEY_S) key_pressed[1] = FALSE;
    if(event->keyval == GDK_KEY_a || event -> keyval == GDK_KEY_A) key_pressed[2] = FALSE;
    if(event->keyval == GDK_KEY_d || event -> keyval == GDK_KEY_D) key_pressed[3] = FALSE;
    if(event->keyval == GDK_KEY_q || event -> keyval == GDK_KEY_Q) key_pressed[4] = FALSE;
    if(event->keyval == GDK_KEY_e || event -> keyval == GDK_KEY_E) key_pressed[5] = FALSE;
    if(event->keyval == GDK_KEY_Shift_L) key_pressed[6] = FALSE;
    if(event->keyval == GDK_KEY_Control_L) key_pressed[7] = FALSE;
}

void get_mouse_pressed(GdkEventButton *event) {
    mouse_pressed[event->button-1] = TRUE;  // right-click
}

void get_mouse_released(GdkEventButton *event) {
    mouse_pressed[event->button-1] = FALSE; // right-click
}

Point2D_int get_mouse_pos(GtkWidget *widget) {
    int win_x, win_y;
    GtkWindow *gtk_window;
    gtk_window = GTK_WINDOW(gtk_widget_get_toplevel(widget));
    gtk_window_get_position(gtk_window, &win_x, &win_y);

    int x, y;
    GdkDisplay* display = gdk_display_get_default();
    GdkSeat* seat = gdk_display_get_default_seat(display);
    GdkDevice* pointer = gdk_seat_get_pointer(seat);
    gdk_device_get_position (pointer, NULL, &x, &y);
    x -= win_x;
    y -= win_y;
    Point2D_int p = {x, y};
    return p;
}


void move(Vector *looking, Vector *observer) {
    double a = acos(looking->x);
    if(overrotation == TRUE) a = 2*M_PI-a;

    double move = .02;
    if(key_pressed[0]) {
        observer->x += move*looking->x;
        observer->y += move*looking->y;
    }
    if(key_pressed[1]) {
        observer->x -= move*looking->x;
        observer->y -= move*looking->y;
    }
    if(key_pressed[2]) {
        observer->x += move*cos(a-M_PI/2);
        observer->y += move*sin(a-M_PI/2);
    }
    if(key_pressed[3]) {
        observer->x -= move*cos(a-M_PI/2);
        observer->y -= move*sin(a-M_PI/2);
    }
    if(key_pressed[6]) observer->z -= move;
    if(key_pressed[7]) observer->z += move;
}

void rotate(GtkWidget *widget, Vector *looking, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
    double rotate = M_PI/200;

    if(key_pressed[4]) {
        double a = acos((looking->x)/sqrt(looking->x*looking->x+looking->y*looking->y));
        if(overrotation == TRUE) a = 2*M_PI-a;
        if(a-rotate < 0) {overrotation=TRUE; a+=M_PI*2;}
        else if(overrotation == TRUE && a-rotate < M_PI) overrotation = FALSE;
        if(a-rotate < M_PI) overrotation=FALSE;
        looking->x = cos(a-rotate);
        looking->y = sin(a-rotate);
    }
    if(key_pressed[5]) {
        double a = acos((looking->x)/sqrt(looking->x*looking->x+looking->y*looking->y));
        if(overrotation == TRUE) a = 2*M_PI-a;
        if(a+rotate > M_PI) overrotation=TRUE;
        else if(overrotation == TRUE && a+rotate > M_PI*2) overrotation = FALSE;
        if(a+rotate > M_PI*2) {a -= M_PI*2; overrotation=FALSE;}
        looking->x = cos(a+rotate);
        looking->y = sin(a+rotate);
    }
    if(mouse_pressed[2]) {

        Point2D_int mouse = get_mouse_pos(widget);
        double rotate_xy = (((double)mouse.x-(double)WINDOW_WIDTH/2)*(double)WINDOW_WIDTH)/(2500*(double)WINDOW_WIDTH);
        double rotate_z  = -(((double)mouse.y-(double)WINDOW_HEIGHT/2)*(double)WINDOW_HEIGHT)/(2500*(double)WINDOW_HEIGHT);

        double a_xy = acos((looking->x) / sqrt(pow(looking->x, 2) + pow(looking->y, 2)));
        double a_z = acos(looking->z/sqrt(pow(looking->x, 2) + pow(looking->y, 2)));
        if(overrotation == TRUE) a_xy = 2 * M_PI - a_xy;

        if(a_xy + rotate_xy < 0) { overrotation=TRUE; a_xy+= M_PI * 2;}
        else if(a_xy + rotate_xy > 2 * M_PI) { overrotation=FALSE; a_xy-= 2 * M_PI;}
        else if(overrotation == TRUE && a_xy + rotate_xy < M_PI) overrotation = FALSE;
        else if(a_xy + rotate_xy > M_PI && overrotation == FALSE) overrotation=TRUE;

        looking->x = cos(a_xy + rotate_xy);
        looking->y = sin(a_xy + rotate_xy);

        //if(a_z+rotate_z < M_PI/1000) rotate_z = -(a_z-M_PI/1000);
        //else if(a_z+rotate_z > M_PI-M_PI/1000) rotate_z = (M_PI-M_PI/1000)-a_z;

        looking->z = cos(a_z+rotate_z);
        Vector temp_vector = {looking->x,looking->y, looking->z};
        print_vector(temp_vector);
        temp_vector = normalize_vector(temp_vector);
        print_vector(temp_vector);

        looking->x = temp_vector.x;
        looking->y = temp_vector.y;
        looking->z = temp_vector.z;


        int x, y;
        GtkWindow *gtk_window;
        gtk_window = GTK_WINDOW(gtk_widget_get_toplevel(widget));
        gtk_window_get_position(gtk_window, &x, &y);

        GdkDisplay *display = gdk_display_get_default();
        GdkSeat *seat = gdk_display_get_default_seat(display);
        GdkDevice *pointer = gdk_seat_get_pointer(seat);
        GdkScreen *screen;
        screen = gdk_screen_get_default();


        gdk_device_warp(pointer, screen, x+WINDOW_WIDTH/2, y+WINDOW_HEIGHT/2);
        if(mouse_visible == TRUE) {
            GdkCursor *cursor;

            display = gdk_display_get_default();
            cursor = gdk_cursor_new_for_display(display, GDK_BLANK_CURSOR);
            gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
            mouse_visible = FALSE;
        }
    } else if(mouse_visible == FALSE) {
        GdkDisplay *display;
        GdkCursor *cursor;

        display = gdk_display_get_default();
        cursor = gdk_cursor_new_for_display(display, GDK_LEFT_PTR);
        gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
        mouse_visible = TRUE;
    }
}