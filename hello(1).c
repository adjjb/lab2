/*
 * Userspace program that communicates with the vga_ball device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int vga_ball_fd;

/* Read and print the background color */
void print_background_color() {
  vga_ball_arg_t vla;
  
  if (ioctl(vga_ball_fd, VGA_BALL_READ_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_READ_BACKGROUND) failed");
      return;
  }
  printf("%02x %02x %02x %02x %02x\n",
	 vla.background.red, vla.background.green, vla.background.blue, vla.background.centerh, vla.background.centerv);
}

/* Set the background color */
void set_background_color(const vga_ball_color_t *c)
{
  vga_ball_arg_t vla;
  vla.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}

int main()
{
  vga_ball_arg_t vla;
  int i;
  static const char filename[] = "/dev/vga_ball";

  static const vga_ball_color_t colors[] = {
    { 0xff, 0x00, 0x00, 0x00, 0x00 }, /* Red */
    { 0x00, 0xff, 0x00, 0x00, 0x00 }, /* Green */
    { 0x00, 0x00, 0xff, 0x00, 0x00 }, /* Blue */
    { 0xff, 0xff, 0x00, 0x00, 0x00 }, /* Yellow */
    { 0x00, 0xff, 0xff, 0x00, 0x00 }, /* Cyan */
    { 0xff, 0x00, 0xff, 0x00, 0x00 }, /* Magenta */
    { 0x80, 0x80, 0x80, 0x00, 0x00 }, /* Gray */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* Black */
    { 0xff, 0xff, 0xff, 0x00, 0x00 }  /* White */
  };

  static vga_ball_color_t item;

# define COLORS 9

  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }

  printf("initial state: ");
  print_background_color();

  item = colors[1];

    //set_background_color(&colors[3]);
    //print_background_color();
  
  int offset = 50;
  int x = 640;
  int y = offset;
  int dx = 5;
  int dy = 4;

  while(1){
    
    item.centerh = x / 5;
    item.centerv = y / 4;
    set_background_color(&item);
    print_background_color();
    usleep(4000*(480-y)/32);
    x += dx;
    y += dy;
    if(y > 480 - 50) dy = -4;
    else if(y < 50) dy = 4;
    if(x>1280-100) dx = -5;
    else if(x<100) dx = 5;
  }
/*
  for (i = 0 ; i < 24 ; i++) {
    set_background_color(&colors[i % COLORS ]);
    print_background_color();
    usleep(400000);
  }*/
  
  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
