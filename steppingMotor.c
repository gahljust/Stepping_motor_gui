#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int fd; // File descriptor for the port

int open_port(void)
{
    fd = open("/dev/tty.usbserial-A601VOHX", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror("open_port: Unable to open /dev/ttyUSB0 - ");
    }
    else
    {
        fcntl(fd, F_SETFL, 0);
    }
    return (fd);
}

void send_command(int fd, char *command)
{
    write(fd, command, strlen(command));
    sleep(1); // Wait for command to execute
}

void read_response(int fd)
{
    char buffer[32];
    int n = read(fd, buffer, sizeof(buffer));
    if (n < 0)
    {
        perror("Read failed");
        return;
    }
    buffer[n] = '\0';
    printf("Received: %s\n", buffer);
}

static void online_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "F");
}

GtkWidget *distance_entry;
GtkWidget *position_label;
GtkWidget *status_label;

// Function to handle the "Move" button click
static void move_button_clicked_cb(GtkWidget *button, gpointer data)
{
    const char *distance_str = gtk_entry_get_text(GTK_ENTRY(distance_entry));
    double distance = strtod(distance_str, NULL); // Convert the string to a double
    // Convert the distance to steps by dividing by 0.0025 and rounding to the nearest integer
    int steps = (int)((distance / 0.0025 + 0.5));                
    char command[32];
    sprintf(command, "C I1M-%d, R", steps); 
    // print the steps to the terminal
    printf("Steps: %d\n", steps);
    printf("Command: %s\n", command);
    send_command(fd, command);
    sleep(1);
   
}

// Function to handle the "Move" button click
static void move_closer_button_clicked_cb(GtkWidget *button, gpointer data)
{
    const char *distance_str = gtk_entry_get_text(GTK_ENTRY(distance_entry));
    double distance = strtod(distance_str, NULL); // Convert the string to a double
    // Convert the distance to steps by dividing by 0.0025 and rounding to the nearest integer
    int steps = (int)((distance / 0.0025));
    char command[32];
    sprintf(command, "C I1M%d, R", steps);
    // print the steps to the terminal
    printf("Steps: %d\n", steps);
    printf("Command: %s\n", command);
    send_command(fd, command);
    sleep(1);
}

static void home_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "I1M0, R");
}

static void set_zero_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "N");
}

static void status_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "V");
    static char buffer[32];                       // Make buffer static so it remains valid after function returns
    int n = read(fd, buffer, sizeof(buffer) - 1); // Leave space for null terminator
    if (n < 0)
    {
        perror("Read failed");
    }
    else
    {
        buffer[n] = '\0';
        // if the buffer is R, the motor is running if the buffer is B the motor is busy else its in jog mode
        if (strcmp(buffer, "R") == 0)
        {
            strcpy(buffer, "Running");
        }
        else if (strcmp(buffer, "B") == 0)
        {
            strcpy(buffer, "Busy");
        }
        else
        {
            strcpy(buffer, "Jog");
        }


        char label_text[64];                                       // Enough space for prefix and position value
        sprintf(label_text, "Motor Status: %s", buffer);   // Create the new label text with two decimal places
        gtk_label_set_text(GTK_LABEL(status_label), label_text); // Set the new label text
    }

}

static void get_position_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "X");
    static char buffer[32];                       // Make buffer static so it remains valid after function returns
    int n = read(fd, buffer, sizeof(buffer) - 1); // Leave space for null terminator
    if (n < 0)
    {
        perror("Read failed");
    }
    else
    {
        buffer[n] = '\0';
        int position = atoi(buffer);                               // Convert the string to an integer
        double distance_mm = position * 0.0025;                    // Convert the position to distance in mm
        char label_text[64];                                       // Enough space for prefix and position value
        sprintf(label_text, "Position (mm): %.2f", distance_mm);   // Create the new label text with two decimal places
        gtk_label_set_text(GTK_LABEL(position_label), label_text); // Set the new label text
    }
}

static void clear_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "C");
}

static void kill_button_clicked_cb(GtkWidget *button, gpointer data)
{
    send_command(fd, "K");
}

static void close_port_clicked(void)
{
    send_command(fd, "Q");
    close(fd);
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    open_port();

    // configure the port
    struct termios tty;
    memset(&tty, 0, sizeof tty);

    if (tcgetattr(fd, &tty) != 0)
    {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, B9600); // set baud rate
    cfsetispeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls, enable reading
    tty.c_cflag &= ~CSIZE;           // Mask the character size bits
    tty.c_cflag |= CS8;              // 8 data bit
    tty.c_cflag &= ~PARENB;          // no parity bit
    tty.c_cflag &= ~CSTOPB;          // only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS;         // no hardware flowcontrol

    // apply the settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Motor Controller");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 200);

    GtkWidget *online_button = gtk_button_new_with_label("Online");
    g_signal_connect(online_button, "clicked", G_CALLBACK(online_button_clicked_cb), NULL);

    GtkWidget *home_button = gtk_button_new_with_label("Home");
    g_signal_connect(home_button, "clicked", G_CALLBACK(home_button_clicked_cb), NULL);

    GtkWidget *set_zero_button = gtk_button_new_with_label("Set Position to Zero");
    g_signal_connect(set_zero_button, "clicked", G_CALLBACK(set_zero_button_clicked_cb), NULL);

    GtkWidget *move_button = gtk_button_new_with_label("Move Away");
    g_signal_connect(move_button, "clicked", G_CALLBACK(move_button_clicked_cb), NULL);

    GtkWidget *move_closer_button = gtk_button_new_with_label("Move Closer");
    g_signal_connect(move_closer_button, "clicked", G_CALLBACK(move_closer_button_clicked_cb), NULL);

    GtkWidget *get_position_button = gtk_button_new_with_label("Get Position");
    g_signal_connect(get_position_button, "clicked", G_CALLBACK(get_position_button_clicked_cb), NULL);

    GtkWidget *clear_button = gtk_button_new_with_label("Clear");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_button_clicked_cb), NULL);

    GtkWidget *kill_button = gtk_button_new_with_label("Kill Op.");
    g_signal_connect(kill_button, "clicked", G_CALLBACK(kill_button_clicked_cb), NULL);

    GtkWidget *status_button = gtk_button_new_with_label("Status");
    g_signal_connect(status_button, "clicked", G_CALLBACK(status_button_clicked_cb), NULL);

    GtkWidget *close_port_button = gtk_button_new_with_label("Offline");
    g_signal_connect(close_port_button, "clicked", G_CALLBACK(close_port_clicked), NULL);

    distance_entry = gtk_entry_new();

    GtkWidget *operation_commands_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(operation_commands_box), online_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(operation_commands_box), clear_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(operation_commands_box), kill_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(operation_commands_box), status_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(operation_commands_box), close_port_button, FALSE, FALSE, 0);

    GtkWidget *operation_commands_frame = gtk_frame_new("Operation Commands");
    gtk_container_add(GTK_CONTAINER(operation_commands_frame), operation_commands_box);

    /* Create the second group of buttons */
    GtkWidget *movement_commands_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(movement_commands_box), home_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(movement_commands_box), set_zero_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(movement_commands_box), move_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(movement_commands_box), move_closer_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(movement_commands_box), get_position_button, FALSE, FALSE, 0);

    GtkWidget *movement_commands_frame = gtk_frame_new("Movement Commands");
    gtk_container_add(GTK_CONTAINER(movement_commands_frame), movement_commands_box);

    GtkWidget *display_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *distance_label = gtk_label_new("Enter the Distance (mm): ");
    gtk_box_pack_start(GTK_BOX(display_box), distance_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(display_box), distance_entry, FALSE, FALSE, 0);

    position_label = gtk_label_new("Position (mm): ");
    gtk_box_pack_start(GTK_BOX(display_box), position_label, FALSE, FALSE, 0);
   

    status_label = gtk_label_new("Motor Status: ");
    gtk_box_pack_start(GTK_BOX(display_box), status_label, FALSE, FALSE, 0);
   

    GtkWidget *display_frame = gtk_frame_new("Display");
    gtk_container_add(GTK_CONTAINER(display_frame), display_box);

    /* Put everything together */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), operation_commands_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), movement_commands_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), display_frame, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), hbox);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
