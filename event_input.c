#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

void handler (int sig)
{
    printf ("nexiting...(%d)n", sig);
    exit (0);
}

void perror_exit (char *error)
{
    perror (error);
    handler (9);
}

int main (int argc, char *argv[])
{
    char mybuf[64];
    int fd, rd, size = sizeof (struct input_event);
    char name[256] = "Unknown";
    char *device = NULL;

    //Setup check
    if (argv[1] == NULL){
        printf("Please specify (on the command line) the path to the dev event interface device\n");
        exit (0);
    }

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");

    if (argc > 1)
        device = argv[1];

    //Open Device
    if ((fd = open (device, O_RDONLY)) == -1)
        printf ("%s is not a vaild device.\n", device);

    //Print Device Name
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : %s (%s)\n", device, name);

    if ( (strcmp(argv[2],"1")) == 0 ) {
        printf("keypad mode.\n");
        while (1){
            if ((rd = read (fd, mybuf, size * 64)) < size)
                perror_exit ("read()");

            printf("Key event detected.\n");
            printf ("buf[10] = %d, buf[12] = %d\n", mybuf[10],mybuf[12]);
            if( (int)mybuf[10] == 115 ) {
                printf("Volume Up Key ");
            } else if ( (int)mybuf[10] == 114 ) {
                printf("Volume Down Key ");
            }

            if( (int)mybuf[12] == 1 ) {
                printf("Pressed.\n");
            } else if( (int)mybuf[12] == 0 ) {
                printf("Released.\n");
            }

        }
    } else if ( (strcmp(argv[2],"2")) == 0 ) {
        printf("ts mode.\n");
        //int multiplier = 0;
        int final_value = 0;
        while (1){
            if ((rd = read (fd, mybuf, sizeof(char) * 16)) < size)
                perror_exit ("read()");

            //printf ("buf[10] = %d, buf[12] * buf[13] = %d\n", mybuf[10],(mybuf[12]*mybuf[13]));
            if( (int)mybuf[10] == 53 ) {
                final_value = ( mybuf[13] * 256 + mybuf[12] ) / 2.08;
                printf("\nXVal: %d %d %d\n",mybuf[12],mybuf[13],final_value);
            } else if ( (int)mybuf[10] == 54 ) {
                final_value = ( mybuf[13] * 256 + mybuf[12] ) / 2.08;
                printf("\nYVal: %d %d %d\n",mybuf[12],mybuf[13],final_value);
            }

            //            if( (int)mybuf[12] == 1 ) {
            //                printf("Pressed.\n");
            //            } else if( (int)mybuf[12] == 0 ) {
            //                printf("Released.\n");
            //            }

        }
    }

    return 0;
}
