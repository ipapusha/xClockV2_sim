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

void draw_clock(uint8_t *hour_intensity, uint8_t *min_intensity) {
	const float clock_hw = 0.7f;
	const float hour_w   = 0.15f; 
	const float min_w    = 0.12f; 
	const float min_h    = 0.55f;
	const float sma = clock_hw * tan(30.0*M_PI/180.0);
	const float big = clock_hw;

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
	draw_tri(min_w, min_h,   90, (float)hour_intensity[0]/255.0);
	draw_tri(min_w, min_h,   60, (float)hour_intensity[1]/255.0);
	draw_tri(min_w, min_h,   30, (float)hour_intensity[2]/255.0);
	draw_tri(min_w, min_h,    0, (float)hour_intensity[3]/255.0);
	draw_tri(min_w, min_h,  -30, (float)hour_intensity[4]/255.0);
	draw_tri(min_w, min_h,  -60, (float)hour_intensity[5]/255.0);
	draw_tri(min_w, min_h,  -90, (float)hour_intensity[6]/255.0);
	draw_tri(min_w, min_h, -120, (float)hour_intensity[7]/255.0);
	draw_tri(min_w, min_h, -150, (float)hour_intensity[8]/255.0);
	draw_tri(min_w, min_h,  180, (float)hour_intensity[9]/255.0);
	draw_tri(min_w, min_h,  150, (float)hour_intensity[10]/255.0);
	draw_tri(min_w, min_h,  120, (float)hour_intensity[11]/255.0);
}

void display(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// background color
	glClear(GL_COLOR_BUFFER_BIT);

	// commence drawing
	uint8_t intensity[12];
	for (int i = 0; i < 12; ++i) {
		intensity[i] = (uint8_t)(((i+1)/12.0) * 255.0);
	}
	draw_clock(intensity, intensity);

	// render now
	//glFlush();
	glutSwapBuffers();
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
	
	// display loop
	glutMainLoop();

	// exit successfully
	return 0;
}
