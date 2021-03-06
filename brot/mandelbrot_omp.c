/* Sequential Mandelbrot program */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define		X_RESN	800	/* x resolution */
#define		Y_RESN	1000	/* y resolution */

typedef struct complextype {
	float real, imag;
} Compl;


 	

/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */
int timeval_subtract (result, x, y)
	struct timeval *result, *x, *y;
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	   tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}


GC mk_black_gc(Display *display, int screen, Window win)
{
	unsigned long valuemask = 0;
	XGCValues values;
	GC gc = XCreateGC(display, win, valuemask, &values);

	XSetBackground(display, gc, WhitePixel(display, screen));
	XSetForeground(display, gc, BlackPixel(display, screen));
	XSetLineAttributes(display, gc, 1, LineSolid,
			CapRound, JoinRound);
	return gc;
}

Window mk_window(Display *display, int screen)
{
	XSizeHints size_hints;
	char *window_name = "Mandelbrot Set";
	int width = X_RESN;
	int height = Y_RESN;

	/* set window position */
	int x = 0;
	int y = 0;

	/* create opaque window */

	int border_width = 4;
	Window win = XCreateSimpleWindow(display,
			RootWindow(display, screen),
				  x, y, width, height, border_width,
				  BlackPixel(display, screen),
				  WhitePixel(display, screen));

	size_hints.flags = USPosition | USSize;
	size_hints.x = x;
	size_hints.y = y;
	size_hints.width = width;
	size_hints.height = height;
	size_hints.min_width = 300;
	size_hints.min_height = 300;

	XSetNormalHints(display, win, &size_hints);
	XStoreName(display, win, window_name);

	return win;
}

void draw_done(Display *display, Window win)
{
	XFlush(display);
#ifndef GET_DATA
	XSelectInput(display, win, ButtonPressMask | KeyPressMask);

	/* perform an events loop unit mouse button or keyis presses */
	{
		int done = 0;
		XEvent an_event;
		while (!done) {
			XNextEvent(display, &an_event);
			switch (an_event.type) {
			case KeyPress:
			case ButtonPress:
				done = 1;
			}
		}
		XCloseDisplay(display);
	}
#else
	XCloseDisplay(display);
#endif
}

int main()
{
	Window win;		/* initialization for a window */
	int screen;		/* which screen */
	int display_width, display_height;
	char *display_name = NULL;
	GC gc;
	Display *display;
	//Pixmap bitmap;
	//XPoint points[800];

	struct timeval tv1, tv2, tvr;
	gettimeofday(&tv1, NULL);

	/* Mandlebrot variables */

	/* connect to Xserver */

	if ((display = XOpenDisplay(display_name)) == NULL) {
		fprintf(stderr, "drawon: cannot connect to X server %s\n",
			XDisplayName(display_name));
		exit(-1);
	}

	/* get screen size */
	screen = DefaultScreen(display);
	display_width = DisplayWidth(display, screen);
	display_height = DisplayHeight(display, screen);

	/* set window size */
	win = mk_window(display, screen);

	/* create graphics context */
	gc = mk_black_gc(display, screen, win);

	/* Funny attribute stuff */
	{
		XSetWindowAttributes attr[1];
		attr[0].backing_store = Always;
		attr[0].backing_planes = 1;
		attr[0].backing_pixel = BlackPixel(display, screen);

		XChangeWindowAttributes(display, win,
					CWBackingStore  |
					CWBackingPlanes |
					CWBackingPixel, attr);

		XMapWindow(display, win);
		XSync(display, 0);
#ifndef GET_DATA
		sleep(1);
#endif
	}

	/* Calculate and draw points */
	int i;
#pragma omp parallel for schedule(runtime)
	for (i = 0; i < X_RESN; i++) {
		int j;
		for (j = 0; j < Y_RESN; j++) {
			Compl z, c;
			z.real = z.imag = 0.0;

			/* scale factors for 800 x 800 window */
			c.real = ((float)j - (Y_RESN/2)) / (Y_RESN/4);
			c.imag = ((float)i - (Y_RESN/2)) / (X_RESN/4);

			int k = 0;
			float lengthsq = 0;
			do {	/* iterate for pixel color */
				float temp =
				    z.real * z.real - z.imag * z.imag + c.real;
				z.imag = 2.0 * z.real * z.imag + c.imag;
				z.real = temp;
				lengthsq = z.real * z.real + z.imag * z.imag;
				k++;

			} while (lengthsq < 4.0 && k < 100);

			if (k == 100) {
#pragma omp critical
				XDrawPoint(display, win, gc, j, i);
			}

		}
	}


	XFlush(display);

	gettimeofday(&tv2, NULL);
	timeval_subtract(&tvr, &tv2, &tv1);

	printf("%ld.%06ld\n", tvr.tv_sec, tvr.tv_usec);

	draw_done(display, win);
	/* Program Finished */

	return 0;
}
