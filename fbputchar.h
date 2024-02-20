#ifndef _FBPUTCHAR_H
#  define _FBPUTCHAR_H

#define FBOPEN_DEV -1          /* Couldn't open the device */
#define FBOPEN_FSCREENINFO -2  /* Couldn't read the fixed info */
#define FBOPEN_VSCREENINFO -3  /* Couldn't read the variable info */
#define FBOPEN_MMAP -4         /* Couldn't mmap the framebuffer memory */
#define FBOPEN_BPP -5          /* Unexpected bits-per-pixel */

struct KeyState {
    unsigned int modifiers;
    unsigned int keycode[2];
    unsigned int character;
    // Add other fields if needed
};


extern int fbopen(void);
extern void fbputchar(char, int, int);
extern void fbputs(const char * , int, int);
extern void fbclean(int,int);
extern char findWord(const struct KeyState dataset[], unsigned int, unsigned int, unsigned int);
#endif
