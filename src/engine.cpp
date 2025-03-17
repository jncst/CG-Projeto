#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <cstdlib>
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

#include "parserXML.h"

using namespace std;

//* VARIÁVEIS /////////////////////////////////////////////////////////////////////

// Camera
float angleX = 0.0f;
float angleY = 0.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

float camera_radius = 3.0; // Distância da câmara ao centro
float minRadius = 1.0;
float maxRadius = 300.0;
float maxAngleY = 60.0;
float minAngleY = -60.0;

float camX;
float camY;
float camZ;

// TESTING STUFF
int valor = 100; // valor para limitar numero de triangulos em funcoes para ajudar a vizualizar a contruçao do objeto

//* VARIÁVEIS /////////////////////////////////////////////////////////////////////

//* AUX FUNCTIONS /////////////////////////////////////////////////////////////////

void translateInitialCameraPos()
{
	camera_radius = sqrt(initialCamX * initialCamX + initialCamY * initialCamY + initialCamZ * initialCamZ);
	angleX = atan2(initialCamZ, initialCamX) * 180.0f / M_PI;
	angleY = asin(initialCamY / camera_radius) * 180.0f / M_PI;
}

void translateCameraPos()
{
	float radX = angleX * M_PI / 180.0f;
	float radY = angleY * M_PI / 180.0f;
	
	// Definir posição da câmara em torno do ponto (0,0,0)
	camX = camera_radius * cos(radX) * cos(radY);
	camY = camera_radius * sin(radY);
	camZ = camera_radius * sin(radX) * cos(radY);
}

//* AUX FUNCTIONS /////////////////////////////////////////////////////////////////

//* DESENHAR OBJETOS //////////////////////////////////////////////////////////////

void drawTriangle (string line)
{
	float ax, ay, az, bx, by, bz, cx, cy, cz;
	
	stringstream ss(line);
    ss >> ax >> ay >> az >> bx >> by >> bz >> cx >> cy >> cz;

	glBegin(GL_TRIANGLES);

		glVertex3d(ax, ay, az);
		glVertex3d(bx, by, bz);
		glVertex3d(cx, cy, cz);

	glEnd();
}

void drawObject (string model)
{
	ifstream file("../generatorResults/" + model);

	string line = "";
	while (getline(file, line))
	{
		drawTriangle (line);
	}
}

//* DESENHAR OBJETOS //////////////////////////////////////////////////////////////

//* CENAS DE INPUT /////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a')
	{
        valor++;
    }
	else if (key == 's')
	{
		if (valor > 0)
		{
			valor--;
		}
    }
    // cout << "Valor: " << valor << "\n";
    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    if (mousePressed)
	{
        angleX += (x - lastMouseX) * 0.2f;
        angleY += (y - lastMouseY) * 0.2f;

		if (angleY > maxAngleY) angleY = maxAngleY;
        if (angleY < minAngleY) angleY = minAngleY;
        
        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void mouseClick(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
	{
        if (state == GLUT_DOWN)
		{
            mousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        }
		else
		{
            mousePressed = false;
        }
    }
}

void specialKeys(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP: camera_radius -= 0.5f;
			if (camera_radius < minRadius)
				camera_radius = minRadius;
			break;
	
		case GLUT_KEY_DOWN: camera_radius += 0.5f;
			if (camera_radius > maxRadius)
				camera_radius = maxRadius;
			break;
	}

    glutPostRedisplay();
}

//* CENAS DE INPUT /////////////////////////////////////////////////////////////////////

//* FUNCOES DO OPENGL /////////////////////////////////////////////////////////////////////

void drawAxis ()
{
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f( 100.0f, 0.0f, 0.0f);

	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);

	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void changeSize(int w, int h)
{
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
	gluPerspective(fov ,ratio, near , far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void) 
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	translateCameraPos();
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

	//* Set the camera

    //* Desenha eixos x y z

	drawAxis();
	
	for(string model : models)
	{
		drawObject (model);
	}

	// End of framesa
	glutSwapBuffers();
}

//* FUNCOES DO OPENGL /////////////////////////////////////////////////////////////////////


//* MAIN ///////////////////////////////////////////////////////////////////////////////

void renderMain(string test_number)
{
	    // init GLUT and the windo
		parseXML(("../test files/test_files_phase_1/test_1_" + test_number + ".xml").c_str());

		translateInitialCameraPos();
        glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);

        glutInitWindowPosition(300,300);
        glutInitWindowSize(width,height);
        glutCreateWindow("Projeto-CG@DI-UM");
        
            
    // Required callback registry 
        glutDisplayFunc(renderScene);
        glutReshapeFunc(changeSize);
    
    // put here the registration of the keyboard callbacks
		glutKeyboardFunc(keyboard);

		glutMouseFunc(mouseClick);
    	glutMotionFunc(mouseMotion);

		glutSpecialFunc(specialKeys);

    //  OpenGL settings
        glEnable(GL_CULL_FACE);         // Habilita o culling
        glCullFace(GL_BACK);            
        // glEnable(GL_DEPTH_TEST);
        
    // enter GLUT's main cycle
        glutMainLoop();  
}

int main(int argc, char **argv)
{
	string line, test_number;

	glutInit(&argc, argv);
    while (true)
	{
		getline(cin, line);
		stringstream ss(line);
	
		ss >> test_number;

		renderMain(test_number);
	}  
	return 0;
}