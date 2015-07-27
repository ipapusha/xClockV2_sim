#define _BSD_SOURCE
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

//extern uint8_t hour_intensity[12];
//extern uint8_t minute_intensity[12];

void draw_square(GLfloat x, GLfloat y, GLfloat wid, GLfloat alpha) {
	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glVertex2f(x-wid/2.0, y-wid/2.0);
		glVertex2f(x+wid/2.0, y-wid/2.0);
		glVertex2f(x+wid/2.0, y+wid/2.0);
		glVertex2f(x-wid/2.0, y+wid/2.0);
	glEnd();
}

void draw_hours(uint8_t *intensity)
{
	const float clock_hw = 0.7f;
	const float hour_w   = 0.1f; 
	const float sma = clock_hw * tan(30.0*M_PI/180.0);
	const float big = clock_hw;

	// 0/12 o'clock
	draw_square(0.0, big, hour_w, (float)intensity[0]/255.0);

	// 1 o'clock ... 11 o'clock
	draw_square( sma,  big, hour_w, (float)intensity[1]/255.0);
	draw_square( big,  sma, hour_w, (float)intensity[2]/255.0);
	draw_square( big,  0.0, hour_w, (float)intensity[3]/255.0);
	draw_square( big, -sma, hour_w, (float)intensity[4]/255.0);
	draw_square( sma, -big, hour_w, (float)intensity[5]/255.0);
	draw_square( 0.0, -big, hour_w, (float)intensity[6]/255.0);
	draw_square(-sma, -big, hour_w, (float)intensity[7]/255.0);
	draw_square(-big, -sma, hour_w, (float)intensity[8]/255.0);
	draw_square(-big,  0.0, hour_w, (float)intensity[9]/255.0);
	draw_square(-big,  sma, hour_w, (float)intensity[10]/255.0);
	draw_square(-sma,  big, hour_w, (float)intensity[11]/255.0);
}

void display(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// background color
	glClear(GL_COLOR_BUFFER_BIT);

	// commence drawing
	//uint8_t intensity[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
	uint8_t intensity[12];
	for (int i = 0; i < 12; ++i) {
		intensity[i] = (uint8_t)(((i+1)/12.0) * 255.0);
	}
	draw_hours(intensity);

	// render now
	glFlush();
}

void keypressed(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
			exit(0);
			break;
		default:
			break;	
	}
}

int main(int argc, char *argv[]) {
	const int win_w = 480, win_h = 480;
	
	// initialize GLUT and create centered window
	glutInit(&argc, argv);
	glutCreateWindow("xClockV2-sim");
	glutInitWindowSize(win_w, win_h);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - win_w)/2,
	                       (glutGet(GLUT_SCREEN_HEIGHT) - win_h)/2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// register callbacks
	glutKeyboardFunc(keypressed);
	
	// display loop
	glutDisplayFunc(display);
	glutMainLoop();

	// exit successfully
	return 0;
}
