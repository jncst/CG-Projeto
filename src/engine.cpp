#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include "parserXML.h"
#include <string>

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ostream>

float z = 2.0f;
float angleX = 0.0f, angleY = 0.0f;
float moveX = 0.0f;
float moveZ = 0.0f;

std::vector<std::vector<std::vector<float> > > points;


std::vector<std::vector<std::vector<float> > > parsePointsFromFile(const std::string& filename, float& length, int& divisions) {
    std::ifstream file(filename);
    std::vector<std::vector<std::vector<float> > > points;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return points;
    }

    int numPointsPerFace;
    file >> numPointsPerFace; // Lê o número de pontos por face
    file >> length;    // Lê o valor de length
    file >> divisions; // Lê o valor de divisions

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int face = 0; face < 6; ++face) { // Um cubo tem 6 faces
        std::vector<std::vector<float> > facePoints;
        for (int i = 0; i < numPointsPerFace; ++i) {
            std::string line;
            std::getline(file, line);
            std::stringstream ss(line);

            float x, y, z;
            ss >> x >> y >> z;

            std::vector<float> point;
            point.push_back(x);
            point.push_back(y);
            point.push_back(z);
            facePoints.push_back(point);
        }
        points.push_back(facePoints);
    }

    file.close();
    return points;
}


void renderBoxPlane(std::string filePath,int which)
{
	float length = 0;
	int divisions = 0;

	points = parsePointsFromFile(filePath, length, divisions);

	printPoints(points);

	for(int n = 0; n < which; n++)
	{
		int i = 0;

		std::cout << "Faces:" << which << std::endl;
		for(int j = 0; j < (divisions * divisions); j++)
		{
			//std::cout << " " << divisions*divisions << std::endl;
		
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_TRIANGLES);
	
			int plus = divisions + 1;
	
			glVertex3d(points[n][i][0], points[n][i][1], points[n][i][2]);
			std::cout << points[n][i][0] << " " << points[n][i][1] << " " << points[n][i][2] << std::endl;
			glVertex3d(points[n][i+1][0], points[n][i+1][1], points[n][i+1][2]);
			std::cout << points[n][i+1][0] << " " << points[n][i+1][1] << " " << points[n][i+1][2] << std::endl;
			glVertex3d(points[n][i+plus][0], points[n][i+plus][1], points[n][i+plus][2]);
			std::cout << points[n][i+plus][0] << " " << points[n][i+plus][1] << " " << points[n][i+plus][2] << std::endl;
			
	
	
			glEnd();
	
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_TRIANGLES);
	
			glVertex3d(points[n][i+1][0], points[n][i+1][1], points[n][i+1][2]);
			std::cout << points[n][i+1][0] << " " << points[n][i+1][1] << " " << points[n][i+1][2] << std::endl;
			glVertex3d(points[n][i+plus][0], points[n][i+plus][1], points[n][i+plus][2]);
			std::cout << points[n][i+plus][0] << " " << points[n][i+plus][1] << " " << points[n][i+plus][2] << std::endl;
			glVertex3d(points[n][i+plus+1][0], points[n][i+plus+1][1], points[n][i+plus+1][2]);
			std::cout << points[n][i+plus+1][0] << " " << points[n][i+plus+1][1] << " " << points[n][i+plus+1][2] << std::endl;
			
			//std::cout << " " << i << std::endl;
	
			i++;
			if ((j+1) % divisions == 0)
				i = (i+1);
	
			
			glEnd();
		}
		std::cout << "Face" << n << std::endl;
	}

}



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
	gluLookAt(camX, camY, camZ,		
				lookAtX, lookAtY, lookAtZ,		
				upX, upY, upZ);

// put axis drawing in here

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
	
	for(std::string model : models)
	{
		if(model.find("plane") != std::string::npos)
		{
			renderBoxPlane("../generatorResults/" + model,1);	
		}
		if(model.find("box") != std::string::npos)
		{
			std::cout << "GERANDO BOX..." << std::endl;
			renderBoxPlane("../generatorResults/" + model,6);
			std::cout << "BOX GERADO" << std::endl;
		}
		if(model.find("cone") != std::string::npos)
		{
			std::cout << "Not implemented yet" << std::endl;
		}
		if(model.find("sphere") != std::string::npos)
		{
			std::cout << "Not implemented yet" << std::endl;
		}
	}

	// End of frame
	glutSwapBuffers();
}


int main(int argc, char **argv) {

	
// init GLUT and the windo
	parseXML("../test files/test_files_phase_1/test_1_4.xml");
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
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}