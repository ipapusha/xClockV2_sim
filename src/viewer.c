#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>

void display(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	// background color
	glClear(GL_COLOR_BUFFER_BIT);

	// draw a square centered at origin
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.5, -0.5);
		glVertex2f( 0.5, -0.5);
		glVertex2f( 0.5,  0.5);
		glVertex2f(-0.5,  0.5);
	glEnd();

	// render now
	glFlush();
}

int main(int argc, char *argv[])
{
	const int win_w = 480, win_h = 480;
	int scr_w, scr_h;
	
	// initialize GLUT and create window
	glutInit(&argc, argv);
	glutCreateWindow("xClockV2_sim");
	
	// center the window
	scr_w = glutGet(GLUT_WINDOW_WIDTH);
	scr_h = glutGet(GLUT_WINDOW_HEIGHT);
	glutInitWindowSize(win_w, win_h);
	glutInitWindowPosition((scr_w - win_w)/2, (scr_h - win_h)/2);

	// display loop
	glutDisplayFunc(display);
	glutMainLoop();
	
	// exit successfully
	return 0;
}
