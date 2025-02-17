#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

float z = 2.0f;
float angleX = 0.0f, angleY = 0.0f;
float moveX = 0.0f;
float moveZ = 0.0f;



void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(3.0,2.0,1.0, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);

// put axis drawing in here

	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, 0.0f);
	glVertex3f( 5.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -5.0f, 0.0f);
	glVertex3f(0.0f, 5.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -5.0f);
	glVertex3f(0.0f, 0.0f, 5.0f);

	glEnd();


	glBegin(GL_QUADS);

	glColor3f(1.0f,1.0f, 1.0f);

	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f( 0.333333f, 0.0f, 1.0f);
	glVertex3f(-0.333333f, 0.0f, 1.0f);
	glVertex3f(-1.0f, 0.0f, 1.0f);

	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(1.0f, 0.0f, 0.333333f);
	glVertex3f(0.333333f, 0.0f ,0.333333f);
	glVertex3f(-0.333333f, 0.0f, 0.333333f);
	glVertex3f(-1.0f, 0.0f, 0.333333f);

	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(1.0f, 0.0f, -0.333333f);
	glVertex3f(0.333333f, 0.0f, -0.333333f);
	glVertex3f(-0.333333f, 0.0f, -0.333333f);
	glVertex3f(-1.0f, 0.0f, -0.333333f);

	glEnd();

	glBegin(GL_QUADS);

	glVertex3f(1.0f, 0.0f, -1.0f);
	glVertex3f(0.333333f ,0.0f, -1.0f);
	glVertex3f(-0.333333f ,0.0f, -1.0f);
	glVertex3f(-1.0f, 0.0f ,-1.0f);
	
	
	glEnd();

	// End of frame
	glutSwapBuffers();
}







int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(512,512);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
// put here the registration of the keyboard callbacks



//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}