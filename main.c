#include <libinput.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <bits/types/sig_atomic_t.h>

static int open_restricted(const char *path, int flags, void *user_data) {
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}
static void close_restricted(int fd, void *user_data) {
    close(fd);
}
const static struct libinput_interface interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
};
volatile sig_atomic_t stop = 0;
int main(void) {
    struct libinput *li;
    struct libinput_event *event;
    struct udev *udev = udev_new();
    li = libinput_udev_create_context(&interface, NULL, udev);
    if (!li) {
        return EXIT_FAILURE;
    }
    if(libinput_udev_assign_seat(li, "seat0") != 0) {
        libinput_unref(li);
        return EXIT_FAILURE;
    }
    struct pollfd fds;
    fds.fd = libinput_get_fd(li);
    fds.events = POLLIN;
    fds.revents = 0;
    do {
        if (libinput_dispatch(li) != 0)
            printf("Dispatch failed");
        while ((event = libinput_get_event(li)) != NULL) {
            enum libinput_event_type test = libinput_event_get_type(event);
            switch (test) {
                case LIBINPUT_EVENT_KEYBOARD_KEY:
                    printf("Key Pressed\n");
                    break;
                case LIBINPUT_EVENT_DEVICE_ADDED: {
                    struct libinput_device *device = libinput_event_get_device(event);
                    const char *name = libinput_device_get_sysname(device);
                    printf("%s\n", name);
                    break;
                }
                case LIBINPUT_EVENT_POINTER_MOTION:
                    printf("Pointer Movement triggered\n");
                    break;
                case LIBINPUT_EVENT_POINTER_BUTTON:
                    printf("Pointer Button triggered\n");
                    break;
                default:
                    printf("Event Triggered\n");
            }
            libinput_event_destroy(event);
            libinput_dispatch(li);
        }
    } while (!stop && poll(&fds, 1, -1) > -1);
    libinput_unref(li);
    return 0;
}
