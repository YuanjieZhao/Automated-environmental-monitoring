#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <ncurses.h>

#define GPIO_11 11 // GPIO connected to #1 ESC (on the right)
#define GPIO_17 17 // GPIO connected to #2 ESC (on the left)

#define TRUE 1
#define FALSE 0

#define STOP 1500 // sending a PWM signal at 1500 us to ESC stops thruster
#define FOWWARD 1550 // clockwise when viewing from the tail
#define REVERSE 1450 // counter-clockwise when viewing from the tail
#define INCREMENT 100

void initialization(void);
void thrusterControl(void);

int main(void)
{
	if (gpioInitialise() < 0)
	{
		printf("pigpio initialisation failed\n");
		return 0;
	}

	initialization();

	thrusterControl();

	/* Terminates the pigpio library, resets the used DMA channels, and release resources */
	gpioTerminate();
	printf("GPIO Terminate\n");

	return 0;
}

/* Set relevent GPIO pins as output, and initialize ESC */
void initialization(void)
{
	printf("Setting GPIO %d and GPIO %d as output...\n", GPIO_11, GPIO_17);
	gpioSetMode(GPIO_11, PI_OUTPUT);
	time_sleep(1); // sleep for one second

	gpioSetMode(GPIO_17, PI_OUTPUT);
	time_sleep(1);

	printf("sending stop signal at STOP us to initialize ESC..\n");
	gpioServo(GPIO_11, STOP); // send "stop" signal to ESC
	time_sleep(2); // delay to allow the ESC to recognize the stopped signal

	gpioServo(GPIO_17, STOP); // send "stop" signal to ESC
	time_sleep(2); // delay to allow the ESC to recognize the stopped signal
}

/* Control thruster via keyboard */
void thrusterControl(void)
{
	int ch;
	int isContinue = TRUE; // flage: if false, quit from keyboard control mode

	/* Basic initialization of curses lib */
	initscr(); // start the curses mode. A new virtual terminal windows is opened.
	cbreak(); // Line buffering disabled. pass on everything
	noecho(); // Don't echo() while we do getch
	keypad(stdscr, TRUE); // enable readings of function keys

	/* Start keyboard control mode */
	// In the curses mode, printw() is equivalent to printf()
	printw("Use arrow keys to control thruster. Press 'q' to quit\n"); 

	while (isContinue)
	{
		switch (ch = getch())
		{
		case KEY_DOWN:
			gpioServo(GPIO_11, FOWWARD);
			gpioServo(GPIO_17, FOWWARD);
			break;

		case KEY_UP:
			gpioServo(GPIO_11, REVERSE);
			gpioServo(GPIO_17, REVERSE);
			break;

		// ROV starts turning left when left arrow key is pressed once
		case KEY_LEFT:
			gpioServo(GPIO_11, REVERSE);
			gpioServo(GPIO_17, FOWWARD);
			break;

		// ROV starts turning right when right arrow key is pressed once
		case KEY_RIGHT:
			gpioServo(GPIO_11, FOWWARD);
			gpioServo(GPIO_17, REVERSE);
			break;

		case 's':
			gpioServo(GPIO_11, STOP);
			gpioServo(GPIO_17, STOP);
			break;

		case 'q':
			isContinue = FALSE;
			break;

		default:
			printw("invalid input \n");
			break;
		}
	}

	endwin(); // close the curses mode
}