#define _BSD_SOURCE

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h> /* exit */
#include <math.h>
#include <stdint.h>

#include "simulator.h"

void draw_square(GLfloat x, GLfloat y, GLfloat wid, GLfloat alpha) {
	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glVertex2f(x-wid/2.0, y-wid/2.0);
		glVertex2f(x+wid/2.0, y-wid/2.0);
		glVertex2f(x+wid/2.0, y+wid/2.0);
		glVertex2f(x-wid/2.0, y+wid/2.0);
	glEnd();
}

void draw_tri(GLfloat w, GLfloat h, GLfloat rot_deg, GLfloat alpha) {
	glPushMatrix();
	glRotatef(rot_deg, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(h, -w/2.0);
		glVertex2f(h, w/2.0);
	glEnd();
	glPopMatrix();
}

void draw_string(GLfloat x, GLfloat y, char *str) {
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
	glRasterPos2f(x, y);
	while(str && *str) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *str);
		str++;
	}
}

void draw_clock(uint8_t *hour_intensity, uint8_t *min_intensity, uint16_t time_secs) {
	const float clock_hw = 0.7f;
	const float hour_w   = 0.15f; 
	const float min_w    = 0.12f; 
	const float min_h    = 0.55f;
	const float sma = clock_hw * tan(30.0*M_PI/180.0);
	const float big = clock_hw;
	const float eps = 0.05f;
	char strbuf[256];

	// hours
	draw_square( 0.0,  big, hour_w, (float)hour_intensity[0]/255.0);
	draw_square( sma,  big, hour_w, (float)hour_intensity[1]/255.0);
	draw_square( big,  sma, hour_w, (float)hour_intensity[2]/255.0);
	draw_square( big,  0.0, hour_w, (float)hour_intensity[3]/255.0);
	draw_square( big, -sma, hour_w, (float)hour_intensity[4]/255.0);
	draw_square( sma, -big, hour_w, (float)hour_intensity[5]/255.0);
	draw_square( 0.0, -big, hour_w, (float)hour_intensity[6]/255.0);
	draw_square(-sma, -big, hour_w, (float)hour_intensity[7]/255.0);
	draw_square(-big, -sma, hour_w, (float)hour_intensity[8]/255.0);
	draw_square(-big,  0.0, hour_w, (float)hour_intensity[9]/255.0);
	draw_square(-big,  sma, hour_w, (float)hour_intensity[10]/255.0);
	draw_square(-sma,  big, hour_w, (float)hour_intensity[11]/255.0);

	// minutes
	draw_tri(min_w, min_h,   90, (float)min_intensity[0]/255.0);
	draw_tri(min_w, min_h,   60, (float)min_intensity[1]/255.0);
	draw_tri(min_w, min_h,   30, (float)min_intensity[2]/255.0);
	draw_tri(min_w, min_h,    0, (float)min_intensity[3]/255.0);
	draw_tri(min_w, min_h,  -30, (float)min_intensity[4]/255.0);
	draw_tri(min_w, min_h,  -60, (float)min_intensity[5]/255.0);
	draw_tri(min_w, min_h,  -90, (float)min_intensity[6]/255.0);
	draw_tri(min_w, min_h, -120, (float)min_intensity[7]/255.0);
	draw_tri(min_w, min_h, -150, (float)min_intensity[8]/255.0);
	draw_tri(min_w, min_h,  180, (float)min_intensity[9]/255.0);
	draw_tri(min_w, min_h,  150, (float)min_intensity[10]/255.0);
	draw_tri(min_w, min_h,  120, (float)min_intensity[11]/255.0);

	// draw current time in seconds
	snprintf(strbuf, sizeof(strbuf), "time: %d", time_secs);
	draw_string(-1.0f+eps, -1.0f+eps, strbuf);
}

void display(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// background color
	glClear(GL_COLOR_BUFFER_BIT);

	// commence drawing
	pthread_mutex_lock(&global_intensity_mutex);
	pthread_mutex_lock(&global_time_secs_mutex);
	draw_clock(global_hour_intensity, global_minute_intensity, global_time_secs);
	pthread_mutex_unlock(&global_intensity_mutex);
	pthread_mutex_unlock(&global_time_secs_mutex);

	// render now
	glutSwapBuffers();

	// schedule to redisplay
	glutPostRedisplay(); // not the best way to animate
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(win_w, win_h);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - win_w)/2,
	                       (glutGet(GLUT_SCREEN_HEIGHT) - win_h)/2);
	glutCreateWindow(argv[0]);
	
	// enables alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// register callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(keypressed);

	// simulation
	start_sim();
	
	// display loop
	glutMainLoop();
	wait_sim();

	// exit successfully
	return 0;
}
