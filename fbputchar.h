#ifndef _FBPUTCHAR_H
#  define _FBPUTCHAR_H

#define FBOPEN_DEV -1          /* Couldn't open the device */
#define FBOPEN_FSCREENINFO -2  /* Couldn't read the fixed info */
#define FBOPEN_VSCREENINFO -3  /* Couldn't read the variable info */
#define FBOPEN_MMAP -4         /* Couldn't mmap the framebuffer memory */
#define FBOPEN_BPP -5          /* Unexpected bits-per-pixel */

extern int fbopen(void);
extern void fbputchar(char, int, int);
extern void fbputs(const char *, int, int);
extern void fbclean(int,int);
extern char findWord(struct KeyState, unsigned char, unsigned char, unsigned char);

struct KeyState {
    unsigned char modifiers;
    unsigned char keycode[2];
    char character;
    // Add other fields if needed
};

struct KeyState *dataset = {
    {0x00, {0x04, 0x00}, 'A'},  // Example entry 1
    /*{0x02, {0x21, 0x22}, 'B'},  // Example entry 2
    {0x03, {0x31, 0x32}, 'C'},*/  // Example entry 3
    // Add more entries as needed
};
#endif
