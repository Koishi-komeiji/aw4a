#include <unistd.h>
#include <fcntl.h>	/* for fcntl */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>	/* for mmap */
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "font_ubuntumono_9x18.h"
#include "font_ubuntumono_18x36.h"
#include "icons-48px.h"

#include "imgtst1.h"

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

void write_pixel(char *fbptr, int x, int y, int r, int g, int b, int a){
    int thislocation;
    thislocation = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
    *(fbptr + thislocation) = b;
    *(fbptr + thislocation + 1) = g;
    *(fbptr + thislocation + 2) = r;
    *(fbptr + thislocation + 3) = a;
}

void render_char(char *fbp,int x,int y,char **font,int charname,unsigned short int effects){
    /* Effects:
     * 0 - None
     * 1 - Red
     * 2 - Green
     * 3 - Blue
     */
    unsigned short int which_color = 0;
    unsigned long int fontdata_pos = 0;
    unsigned long int wanted_pos = 0;
    int r,g,b;
    int x_cur;
    int y_cur;

    x_cur = x;
    y_cur = y;

    if ( ( ( x + *font[0] ) > vinfo.xres ) || ( ( y + *font[1] ) > vinfo.yres ) ) {
        printf("Warning: Attempted to access beyond end of framebuffer device. Operation cancelled.\n");
    } else {
        wanted_pos = (charname - 0x20) * *font[0] * *font[1] * 3;
        for ( fontdata_pos = wanted_pos; fontdata_pos < (wanted_pos+(*font[0] * *font[1] * 3)); fontdata_pos++) {

            switch ( which_color ) {
            case 0:
                r = font[2][fontdata_pos];
                which_color++;
                break;
            case 1:
                g = font[2][fontdata_pos];
                which_color++;
                break;
            case 2:
                b = font[2][fontdata_pos];
                which_color = 0;
                if ( r == 0 && g == 0 && b == 0){
                } else {
                    switch ( effects ) {
                    case 0:
                        write_pixel(fbp,x_cur,y_cur,r,g,b,0);
                        break;
                    case 1:
                        write_pixel(fbp,x_cur,y_cur,r,0,0,0);
                        break;
                    case 2:
                        write_pixel(fbp,x_cur,y_cur,0,g,0,0);
                        break;
                    case 3:
                        write_pixel(fbp,x_cur,y_cur,0,0,b,0);
                        break;
                    }
                }
                if ( x_cur - x < (*font[0] - 1) ){
                    x_cur++;
                } else {
                    x_cur = x;
                    y_cur++;
                }

                break;
            }

        }
    }
}

void render_string(char *fbp,int x,int y,char **font,char *string,unsigned short int effects){

    unsigned long int counter = 0;
    unsigned long int offset = 0;

    int x_cur;
    int y_cur;

    x_cur = x;
    y_cur = y;

    for ( counter = 0; counter < strlen(string); counter++){

        render_char(fbp,x_cur,y_cur,font,string[counter],effects);
        x_cur = x_cur + *font[0];
    }

}

void render_image(char *fbp,int x,int y,char **image){

    unsigned short int which_color = 0;
    unsigned long int imgdata_pos = 0;
    int r,g,b;
    int x_cur;
    int y_cur;

    x_cur = x;
    y_cur = y;

    printf("Image: %d x %d\n",atoi(image[0]),atoi(image[1]));
    if ( ( ( x + atoi(image[0]) ) > vinfo.xres ) || ( ( y + atoi(image[1]) ) > vinfo.yres ) ) {
        printf("Warning: Attempted to access beyond end of framebuffer device. Operation cancelled.\n");
    } else {
        for ( imgdata_pos = 0; imgdata_pos < ( atoi(image[0]) * atoi(image[1]) * 3 ); imgdata_pos++) {

            switch ( which_color ) {
            case 0:
                r = image[2][imgdata_pos];
                which_color++;
                break;
            case 1:
                g = image[2][imgdata_pos];
                which_color++;
                break;
            case 2:
                b = image[2][imgdata_pos];
                which_color = 0;

                write_pixel(fbp,x_cur,y_cur,r,g,b,0);


                if ( x_cur - x < ( atoi(image[0]) - 1 ) ){
                    x_cur++;
                } else {
                    x_cur = x;
                    y_cur++;
                }

                break;
            }

        }
    }
}

void render_rectenagle(char *fbp,int x,int y,int w,int h,int s,int r,int g,int b){

    // s: Line size in pixels
    int x_cur;
    int y_cur;

    x_cur = x;
    y_cur = y;

    if ( ( ( x + w ) > vinfo.xres ) || ( ( y + h ) > vinfo.yres ) ) {
        printf("Warning: Attempted to access beyond end of framebuffer device. Operation cancelled.\n");
    } else {

        // QAQ...


    }

}



int main() {
    long int screensize = 0;

    int fbfd;	/* frame buffer file descriptor */
    char* fbp;	/* pointer to framebuffer */
    int location;	/* iterate to location */

    int x, y;	/* x and y location */

    char pxtst[4];
    /* open the file for reading and writing */
    fbfd = open("/dev/fb0",O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        exit(1);
    }
    printf ("The framebuffer device was opened successfully.\n");

    /* get the fixed screen information */
    if (ioctl (fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
        exit(2);
    }

    /* get variable screen information */
    if (ioctl (fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
        exit(3);
    }

    fprintf(stderr,"Framebuffer info:\n");
    fprintf(stderr,"XRes: %d\n",vinfo.xres);
    fprintf(stderr,"YRes: %d\n",vinfo.yres);
    /* figure out the size of the screen in bytes */
    //screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    /* map the device to memory */
    fbp = (char*)mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    if ((int)fbp == -1) {
        printf ("Error: failed to map framebuffer device to memory.\n");
        exit(4);
    }
    printf ("Framebuffer device was mapped to memory successfully.\n");

    // Figure out where in memory to put the pixel
    //    for ( y = 0; y < (768); y++ )
    //        for ( x = 0; x < 1024; x++ ) {
    //            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
    //            if ( vinfo.bits_per_pixel == 32 ) {
    //                HEADER_PIXEL(header_data,pxtst);
    //                *(fbp + location) = pxtst[2]; // Some blue
    //                *(fbp + location + 1) = pxtst[1]; // A little green
    //                *(fbp + location + 2) = pxtst[0]; // A lot of red
    //                *(fbp + location + 3) = 0; // No transparency
    //            } else { //assume 16bpp
    //                int b = 10; int g = (x-100)/6; // A little green
    //                int r = 31-(y-100)/16; // A lot of red
    //                unsigned short int t = r<<11 | g << 5 | b;
    //                *((unsigned short int*)(fbp + location)) = t;
    //            }
    //        }

    render_image(fbp,0,0,img_testqwq_1024x768);
    render_image(fbp,500,800,img_icon_lock_48x48);
    //    render_string(fbp,0,600,font_ubuntumono_9x18,"Naive!!!233333333333!!! La la la Demacia!!! OwO QwQ OAO qwq qaq",0);
    //    render_string(fbp,0,618,font_ubuntumono_9x18,"Naive!!!233333333333!!! La la la Demacia!!! OwO QwQ OAO qwq qaq",1);
    //    render_string(fbp,0,636,font_ubuntumono_9x18,"Naive!!!233333333333!!! La la la Demacia!!! OwO QwQ OAO qwq qaq",2);
    //    render_string(fbp,0,654,font_ubuntumono_9x18,"Naive!!!233333333333!!! La la la Demacia!!! OwO QwQ OAO qwq qaq",3);
    render_string(fbp,0,800,font_ubuntumono_18x36,"Satori loves Koishi~",0);

    //    render_font(fbp,209,200,font_ubuntumono_9x18,'a');
    //    render_font(fbp,218,200,font_ubuntumono_9x18,'i');
    //    render_font(fbp,227,200,font_ubuntumono_9x18,'v');
    //    render_font(fbp,236,200,font_ubuntumono_9x18,'e');
    //    render_font(fbp,245,200,font_ubuntumono_9x18,'!');

    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
