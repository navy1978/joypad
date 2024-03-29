/**
 *
 * Description:
 * Reads joystick/gamepad events and displays them.
 *
 * Compile:
 * gcc joystick.c -o joystick
 *
 * Run:
 * ./joystick [/dev/input/jsX]
 *
 * See also:
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>

int lastPadPressed;
/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/**
 * Current state of an axis.
 */
struct axis_state
{
    short x, y;
};

/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }
    else
    {
        if (event->number == 6)
        {
            if (event->value == -32767)
            {
                printf("DPAD LEFT pressed\n");
                lastPadPressed = -1;
            }
            else if (event->value == 32767)
            {
                printf("DPAD RIGHT pressed\n");
                lastPadPressed = 1;
            }
            else
            {
                printf("DPAD %s released\n", lastPadPressed == 1 ? "RIGHT" : "LEFT");
            }
        }
        else if (event->number == 7)
        {
            if (event->value == -32767)
            {
                printf("DPAD UP pressed\n");
                lastPadPressed = -1;
            }
            else if (event->value == 32767)
            {
                printf("DPAD DOWN pressed\n");
                lastPadPressed = 1;
            }
            else
            {
                printf("DPAD %s released\n", lastPadPressed == 1 ? "DOWN" : "UP");
            }
        }
    }

    return axis;
}

void *listenJoypad(void *vargp)
{
    const char *device ;
    int js;
    struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;

    device ="/dev/input/js0"; // (char *)vargp;

    js = open(device, O_RDONLY);

    if (js == -1)
        perror("Could not open joystick");

    /* This loop will exit if the controller is unplugged. */
    while (read_event(js, &event) == 0)
    {
        switch (event.type)
        {
        case JS_EVENT_BUTTON:
            printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
            break;
        case JS_EVENT_AXIS:
            axis = get_axis_state(&event, axes);
            if (axis < 3)
                printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
            break;
        default:
            /* Ignore init events. */
            break;
        }

        fflush(stdout);
    }

    close(js);
    
}

int main(int argc, char *argv[])
{
    const char *device;

 if (argc > 1)
        device = argv[1];
    else
        device = "/dev/input/js0";


     pthread_t thread_id;
    printf("Before Thread\n");
    pthread_create(&thread_id, NULL, listenJoypad, &device);
    // pthread_join(thread_id, NULL);
    printf("After Thread\n");
    while (1){
        sleep(3);
        printf("doing something\n");
    }
    return 0;
}
