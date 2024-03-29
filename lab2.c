/*
 *
 * CSEE 4840 Lab 2 for 2019
 *
 * Chuyi Jiang / cj2792
 * Xinzi Yu / xy2590
 */
#include "fbputchar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "usbkeyboard.h"
#include <pthread.h>

/* Update SERVER_HOST to be the IP address of
 * the chat server you are connecting to
 */
/* arthur.cs.columbia.edu */
#define SERVER_HOST "128.59.19.114"
#define SERVER_PORT 42000

#define BUFFER_SIZE 128

/*
extern int fbopen(void);
 * References:
 *
 * http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 * http://www.thegeekstuff.com/2011/12/c-socket-programming/
 * 
 */

int sockfd; /* Socket file descriptor */

struct libusb_device_handle *keyboard;
uint8_t endpoint_address;
pthread_t network_thread;
void *network_thread_f(void *);

int rowDisplay = 0 ;

/*A dictionary map for keyboard code*/extern void insertWord(char* , int,  int);
const char ascii_to_hid_key_map[95][3]= {
    {0, KEY_SPACE, ' '}, {KEY_MOD_LSHIFT, KEY_1, '!'}, {KEY_MOD_LSHIFT, KEY_APOSTROPHE,'\"'},
    {KEY_MOD_LSHIFT, KEY_3, '#'}, {KEY_MOD_LSHIFT, KEY_4,'$'}, {KEY_MOD_LSHIFT, KEY_5,'%'},
    {KEY_MOD_LSHIFT, KEY_7, '&'}, {0, KEY_APOSTROPHE,'\''}, {KEY_MOD_LSHIFT, KEY_9,'('},
    {KEY_MOD_LSHIFT, KEY_0,')'}, {KEY_MOD_LSHIFT, KEY_8, '*'}, {KEY_MOD_LSHIFT, KEY_EQUAL, '+'},
    {0, KEY_COMMA, ','}, {0, KEY_DOT, '.' }, {0, KEY_SLASH,'/'}, {0, KEY_0, '0'},
    {0, KEY_1, '1'}, {0, KEY_2, '2'}, {0, KEY_3, '3'}, {0, KEY_4, '4'}, {0, KEY_5, '5'}, {0, KEY_6, '6'},
    {0, KEY_7, '7'}, {0, KEY_8, '8'}, {0, KEY_9, '9'}, {KEY_MOD_LSHIFT, KEY_SEMICOLON,':'},
    {0, KEY_SEMICOLON,';'}, {KEY_MOD_LSHIFT, KEY_COMMA, '<'}, {0, KEY_EQUAL,'='},
    {KEY_MOD_LSHIFT, KEY_DOT, '>'}, {KEY_MOD_LSHIFT, KEY_SLASH,'?'}, {KEY_MOD_LSHIFT, KEY_2, '@'},
    {KEY_MOD_LSHIFT, KEY_A, 'A'}, {KEY_MOD_LSHIFT, KEY_B, 'B'}, {KEY_MOD_LSHIFT, KEY_C, 'C'},
    {KEY_MOD_LSHIFT, KEY_D, 'D'}, {KEY_MOD_LSHIFT, KEY_E, 'E'}, {KEY_MOD_LSHIFT, KEY_F, 'F'},
    {KEY_MOD_LSHIFT, KEY_G, 'G'}, {KEY_MOD_LSHIFT, KEY_H, 'H'}, {KEY_MOD_LSHIFT, KEY_I, 'I'},
    {KEY_MOD_LSHIFT, KEY_J, 'J'}, {KEY_MOD_LSHIFT, KEY_K, 'K'}, {KEY_MOD_LSHIFT, KEY_L, 'L'},
    {KEY_MOD_LSHIFT, KEY_M, 'M'}, {KEY_MOD_LSHIFT, KEY_N, 'N'}, {KEY_MOD_LSHIFT, KEY_O, 'Q'},
    {KEY_MOD_LSHIFT, KEY_P, 'P'}, {KEY_MOD_LSHIFT, KEY_Q, 'Q'}, {KEY_MOD_LSHIFT, KEY_R, 'R'},
    {KEY_MOD_LSHIFT, KEY_S, 'S'}, {KEY_MOD_LSHIFT, KEY_T, 'T'}, {KEY_MOD_LSHIFT, KEY_U, 'U'},
    {KEY_MOD_LSHIFT, KEY_V, 'V'}, {KEY_MOD_LSHIFT, KEY_W, 'W'}, {KEY_MOD_LSHIFT, KEY_X, 'X'},
    {KEY_MOD_LSHIFT, KEY_Y, 'Y'}, {KEY_MOD_LSHIFT, KEY_Z, 'Z'}, {0, KEY_LEFTBRACE,'['},
    {0, KEY_BACKSLASH,'\\'}, {0, KEY_RIGHTBRACE,']'}, {KEY_MOD_LSHIFT, KEY_6, '^'},
    {KEY_MOD_LSHIFT, KEY_MINUS,'_'},{0, KEY_MINUS,'-'}, {0, KEY_GRAVE,'`'}, {0, KEY_A,'a'}, {0, KEY_B,'b'},
    {0, KEY_C, 'c'}, {0, KEY_D, 'd'}, {0, KEY_E, 'e'}, {0, KEY_F, 'f'}, {0, KEY_G, 'g'}, {0, KEY_H, 'h'},
    {0, KEY_I, 'i'}, {0, KEY_J, 'j'}, {0, KEY_K, 'k'}, {0, KEY_L, 'l'}, {0, KEY_M, 'm'}, {0, KEY_N, 'n'},
    {0, KEY_O, 'o'}, {0, KEY_P, 'p'}, {0, KEY_Q, 'q'}, {0, KEY_R, 'r'}, {0, KEY_S, 's'}, {0, KEY_T, 't'},
    {0, KEY_U, 'u'}, {0, KEY_V, 'v'}, {0, KEY_W, 'w'}, {0, KEY_X, 'x'}, {0, KEY_Y, 'y'}, {0, KEY_Z, 'z'},
    {KEY_MOD_LSHIFT, KEY_LEFTBRACE,'{'}, {KEY_MOD_LSHIFT, KEY_BACKSLASH,'|'},
    {KEY_MOD_LSHIFT, KEY_RIGHTBRACE,'}'}, {KEY_MOD_LSHIFT, KEY_GRAVE,'~'},
};



int main()
{
  int err, col;
  struct sockaddr_in serv_addr;

  struct usb_keyboard_packet packet;
  int transferred;
  char keystate[12];
  char word[512]; /*Record the word and characters*/
  unsigned int a,b,c; 
  unsigned order = 0;/*The position of the cursor*/
	
  if ((err = fbopen()) != 0) {
    fprintf(stderr, "Error: Could not open framebuffer: %d\n", err);
    exit(1);      
  }

  /* Draw space to whole screen*/
  fbclean(24,64,0,0);
	
  /* Draw rows of asterisks across the top and bottom of the screen */
  for (col = 0 ; col < 64 ; col++) {
    fbputchar('_', 20, col);
  }

  /*Initalization */
  for (int k = 0; k < 512; k ++) {
	word[k] = '\0';
  }
  word[0] = '|';
  fbputs(word,21,0,0);

  /* Open the keyboard */
  if ( (keyboard = openkeyboard(&endpoint_address)) == NULL ) {
    fprintf(stderr, "Did not find a keyboard\n");
    exit(1);
  }
    
  /* Create a TCP communications socket */
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "Error: Could not create socket\n");
    exit(1);
  }

  /* Get the server address */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  if ( inet_pton(AF_INET, SERVER_HOST, &serv_addr.sin_addr) <= 0) {
    fprintf(stderr, "Error: Could not convert host IP \"%s\"\n", SERVER_HOST);
    exit(1);
  }

  /* Connect the socket to the server */
  if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error: connect() failed.  Is the server running?\n");
    exit(1);
  }

  /* Start the network thread */
  pthread_create(&network_thread, NULL, network_thread_f, NULL);

  /* Look for and handle keypresses */
  for (;;) {
    libusb_interrupt_transfer(keyboard, endpoint_address,
			      (unsigned char *) &packet, sizeof(packet),
			      &transferred, 0);
    if (transferred == sizeof(packet)) {
      sprintf(keystate, "%02x %02x %02x", packet.modifiers, packet.keycode[0],
	      packet.keycode[1]);
      sscanf(keystate, "%02x %02x %02x", &a, &b, &c);
      if (b != 0 && c == 0){
	      for (int i = 0; i < 95; ++i) {
		if (ascii_to_hid_key_map[i][0] == a && ascii_to_hid_key_map[i][1] == b ){
		
			for (int i = strlen(word); i > order ;i --){
					word[i] = word[i-1];
			}
			word[order] = ascii_to_hid_key_map[i][2];
			order ++;
	
		}
	      }
      } 
	    
      if (b!= 0 && c ==0){
		fbputs(word,21,0,0);
      }
      if (packet.keycode[0] == 0x29) { /* ESC prerowssed? */
	break;
      }

      /* Press the arrow, only move the cursor through exchange 
      the neighbour position */
      else if (packet.keycode[0] == 0x50){ /*Left arrow*/
	word[order] = word[order -1];
	word[order -1] = '|';
	order --;
        fbputs(word,21,0,0);
      }   
      else if (packet.keycode[0] == 0x4f){ /*right arrow*/
	word[order] = word[order + 1];
	word[order+1] = '|';
	order ++;
	fbputs(word,21,0,0);
      }
	      
      else if (packet.keycode[0] == 0x2a){  /*backspace*/
	/*When the consur is at the mid of the sentences
 	  The total length of the word should be reduce by one*/
	for (int i = order; i < strlen(word);i ++){
		word[i-1] = word[i];
	}
	word[strlen(word) - 1] = '\0';
	order --;
	fbclean(23,64,21,0);
	fbputs(word, 21, 0,0);
      }
      
      else if (packet.keycode[0] == 0x28){ /*enter*/
	fbclean(24,64,21,0);
	if (b!= 0 ){
		/*When the consur is at the end of the sentences and the mid of the sentences*/
		if (order+1 == strlen(word)){
			word[order] = '\0';
		}
		else{
			for (int i = order+1; i < strlen(word);i ++){
				word[i-1] = word[i];
				order ++;
			}
		}

		/*Send the message and clean the word variable*/
		write(sockfd, word, strlen(word));
		for (int k = 0; k < 512; k ++) {
			word[k] = '\0';
		}
		order = 0;
		word[0] = '|';
		fbputs(word, 21, 0,0);
	}
      }
    }
  }

  /* Terminate the network thread */
  pthread_cancel(network_thread);

  /* Wait for the network thread to finish */
  pthread_join(network_thread, NULL);

  return 0;
}

void *network_thread_f(void *ignored)
{
  char recvBuf[BUFFER_SIZE];
  int n;
  /* Receive data */
  while ( (n = read(sockfd, &recvBuf, BUFFER_SIZE - 1)) > 0 ) {
    recvBuf[n] = '\0';
    printf("%s", recvBuf);
    int changeLine = fbputs(recvBuf, rowDisplay, 0,1);
    rowDisplay = rowDisplay + 1 + changeLine;

    /*If the displayed message reach the splited line, clean the screen*/
    if (rowDisplay >= 20){
	fbclean(rowDisplay,64,0,0);
	rowDisplay = 0;
    }
  }

  return NULL;
}
