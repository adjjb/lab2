
#ifndef _FBPUTCHAR_H
#  define _FBPUTCHAR_H

#define FBOPEN_DEV -1          /* Couldn't open the device */
#define FBOPEN_FSCREENINFO -2  /* Couldn't read the fixed info */
#define FBOPEN_VSCREENINFO -3  /* Couldn't read the variable info */
#define FBOPEN_MMAP -4         /* Couldn't mmap the framebuffer memory */
#define FBOPEN_BPP -5          /* Unexpected bits-per-pixel */

#include "usb_hid_keys.h"



extern int fbopen(void);
extern void fbputchar(char, int, int);
extern int fbputs(const char *, int, int,int);
extern void fbclean(int,int,int,int);
extern void insertWord(char* , int,  int);



#endif
