/* Sequential Mandelbrot program */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#define		X_RESN	800	/* x resolution */
#define		Y_RESN	800	/* y resolution */

typedef struct complextype {
	float real, imag;
} Compl;

typedef struct drawing_board {
	Window win;
	Display *display;
	int screen;
	GC gc;

	Pixmap main_map;
	int m_sz_x;
	int m_sz_y;
} db_t;

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
}

void master(int nprocs)
{
	Window win;		/* initialization for a window */
	GC gc;
	Display *display;

	double t1 = MPI_Wtime();

	char *display_name = NULL;
	int screen;		/* which screen */
	int display_width, display_height;

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
	}

	int ct;
	int *col = malloc((Y_RESN + 1) * sizeof(*col));
	for (ct = X_RESN; ct > 0; ct--) {
		MPI_Status status;
		MPI_Recv(col, Y_RESN + 1, MPI_INT, MPI_ANY_SOURCE, 0,
				MPI_COMM_WORLD, &status);

		int j;
		int i = col[Y_RESN];
		for( j = 0; j < Y_RESN; j++) {
			if (col[j])
				XDrawPoint(display, win, gc, j, i);
		}

	}

	XFlush(display);
	free(col);

	double t2 = MPI_Wtime();
	printf("%f\n", t2-t1);

	draw_done(display,win);
}

void slave(int rank, int nprocs)
{
	int *col = malloc((Y_RESN + 1) * sizeof(*col));

	int i_rem = X_RESN % nprocs;
	int i_ct = X_RESN / nprocs;

	int i_start = i_ct * rank;
	if (rank < i_rem) {
		i_start += rank;
		i_ct += 1;
	} else {
		i_start += i_rem;
	}

	/* Calculate and draw points */
	int i;
	for (i = i_start; i < i_ct; i++) {
		int j;
		for (j = 0; j < Y_RESN; j++) {
			Compl z, c;
			z.real = z.imag = 0.0;

			/* scale factors for 800 x 800 window */
			c.real = ((float)j - (Y_RESN/2)) / (Y_RESN/4);
			c.imag = ((float)i - (X_RESN/2)) / (X_RESN/4);

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
				col[j] = 1;
			} else {
				col[j] = 0;
			}
		}

		MPI_Send(col, Y_RESN + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
}

int main(int argc, char **argv)
{

	MPI_Init(&argc, &argv);

	int rank;
	int nprocs;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (rank == 0) {
		master(nprocs);
	} else {
		slave(rank - 1, nprocs - 1);
	}

	/* Program Finished */
	MPI_Finalize();
	return 0;
}
