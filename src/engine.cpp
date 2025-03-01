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
#include <vector>

#include "parserXML.h"

using namespace std;

class Vertice
{
	public:
		// Member variables to store x, y, z coordinates
		float x, y, z;
	
		// Constructor to initialize the coordinates
		Vertice(float xCoord, float yCoord, float zCoord)
		{
			x = xCoord;
			y = yCoord;
			z = zCoord;
		}
		
		// Method to display the coordinates
		void display()
		{
			cout << "Vertice coordinates: (" << x << ", " << y << ", " << z << ")" << endl;
		}
};

void desenhaTriangulo (Vertice a, Vertice b, Vertice c)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

		glVertex3d(a.x, a.y, a.z);
		glVertex3d(b.x, b.y, b.z);
		glVertex3d(c.x, c.y, c.z);

	glEnd();
}

Vertice getVertice (const string& line)
{
	stringstream ss(line);
    float x, y, z;
    ss >> x >> y >> z;

    return Vertice(x, y, z);
}

int getNum(const string& line)
{
	stringstream ss(line);
	string keyword;
    int number;
    ss >> keyword >> number;
    return number;
}

void AddPlane (string filepath)
{
	string temp = "";
	ifstream file(filepath);

	// Remove tipo de objeto
	getline(file, temp);

	// Numero de Vertices
	getline(file, temp);
	int num_vertices = getNum(temp);

	// Numero de Slices
	getline (file, temp);
	int num_slices = getNum(temp);

	// Cria lista de vertices
	vector <Vertice> vertices;

	//* Lê vertices do ficheiro
	for (int i = 0; i < num_vertices; i ++)
	{
		getline (file, temp);
		vertices.push_back(getVertice(temp));
	}

	for (int i = 0; i < num_vertices; i++)
	{
		vertices[i].display();
	}

	// Numero de quadrados que compõem o plano
	// int num_squares = num_slices * num_slices;

	int linhas = 1;

	//* Desenhar o plano
	for (int i = 0; i < num_vertices; i++)
	{
		if (i != 0 && (i + 1) % (num_slices + 1) == 0)
		{
			linhas ++;
			if (linhas > num_slices)
			{
				break;
			}
			continue;
		}
		
		desenhaTriangulo(vertices[i], vertices[i + 1], vertices[i + num_slices + 1]);
		desenhaTriangulo(vertices[i + 1], vertices[i + num_slices + 2], vertices[i + num_slices + 1]);
	}
}

void AddBox (string filepath)
{

}

void AddSphere (string filepath)
{

}

void AddCone (string filepath)
{

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

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

    //* put axis drawing in here

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

	glEnd();
	
	for(string model : models)
	{
		ifstream file("../generatorResults/" + model);
		string tipo;
		getline(file, tipo);

		if (tipo == "PLANE")
		{
			AddPlane("../generatorResults/" + model);
		}
		else if (tipo == "BOX")
		{
			AddBox("../generatorResults/" + model);
		}
		else if (tipo == "CONE")
		{
			AddCone("../generatorResults/" + model);
		}
		else if (tipo == "SPHERE")
		{
			AddSphere("../generatorResults/" + model);
		}
		else
		{
			cout << "Ficheiro.3d não contem um objeto válido\n";
		}
	}

	// End of frame
	glutSwapBuffers();
}

int main(int argc, char **argv) {

    // init GLUT and the windo
        parseXML("../test files/test_files_phase_1/test_1_1.xml");
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
        glutInitWindowPosition(100,100);
        glutInitWindowSize(width,height);
        glutCreateWindow("Projeto-CG@DI-UM");
        
            
    // Required callback registry 
        glutDisplayFunc(renderScene);
        glutReshapeFunc(changeSize);
    
    // put here the registration of the keyboard callbacks
    
    //  OpenGL settings
        glEnable(GL_CULL_FACE);         // Habilita o culling
        glCullFace(GL_BACK);            
        glFrontFace(GL_CW); 
        glEnable(GL_DEPTH_TEST);
        
        
    // enter GLUT's main cycle
        glutMainLoop();
        
        return 1;
    }