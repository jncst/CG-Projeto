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
//float angleX = 0.0f, angleY = 0.0f;
float moveX = 0.0f;
float moveZ = 0.0f;

std::vector<std::vector<std::vector<float> > > points;

// TESTE STUFF/////////////////////////////////////////////

# include <math.h>
// Variáveis globais para controlar a rotação da câmara
float angleX = 0.0f, angleY = 0.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

int valor = 1;

///////////////////////////////////////////////////////////


std::vector<std::vector<std::vector<float> > > parsePointsFromFile(const std::string& filename, int& arg1, int& arg2) {
    std::ifstream file(filename);
    std::vector<std::vector<std::vector<float> > > points;

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return points;
    }

    int numPointsPerFace;
	int faces;

	file >> faces;
    file >> numPointsPerFace; // Lê o número de pontos por face
	file >> arg1;
    file >> arg2; // Lê o valor de divisions

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int face = 0; face < faces; ++face) 
	{ // Um cubo tem 6 faces
        std::vector<std::vector<float> > facePoints;
        for (int i = 0; i < numPointsPerFace; ++i) 
		{
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


void renderSphere(const std::string& filename) 
{
	int slices = 0;
	int stacks = 0;
    points = parsePointsFromFile(filename,slices,stacks);
	std::cout << "Slices: " << slices << " Stacks: " << stacks << std::endl;

	int i = 0;
	
	glEnable(GL_DEPTH_TEST);
    
	glColor3f(1.0f, 1.0f, 1.0f);

    for (int j = 0; j < slices*stacks; j++) 
	{
		glBegin(GL_TRIANGLES);
        glVertex3fv(points[0][i].data());
        glVertex3fv(points[0][i + 1].data());
        glVertex3fv(points[0][i + (slices+2)].data());
		glEnd();

		glBegin(GL_TRIANGLES);
        glVertex3fv(points[0][i].data());
        glVertex3fv(points[0][i + (slices+2)].data());
        glVertex3fv(points[0][i + (slices+1)].data());
		glEnd();

		i++;
		if((j+1) % stacks == 0)
			i += 1;
    }
}

void renderCone(const std::string& filename) 
{
	int slices = 0;
	int stacks = 0;
	points = parsePointsFromFile(filename,slices,stacks);
	std::cout << "Slices: " << slices << " Stacks: " << stacks << std::endl;

	for (int j = 0; j < slices ; j++) 
	{
		glColor3f(1.0f, 1.0f, 1.0f);

		if(j + 1 == slices)
		{
			glBegin(GL_TRIANGLES);
			
			glVertex3fv(points[0][j].data());
			glVertex3fv(points[0][0].data());      // BASE
			glVertex3fv(points[0][slices].data());
			
			glEnd();
			break;
		}
		
		glBegin(GL_TRIANGLES);
		
		glVertex3fv(points[0][j].data());
		glVertex3fv(points[0][j + 1].data());		
		glVertex3fv(points[0][slices].data());
		
		glEnd();

	}

	int i = 0;

	for(int st = 0; st < stacks - 1; st++)
	{
		for(int j = 0; j < slices ; j++)
		{
			glColor3f(1.0f, 1.0f, 1.0f);

			glBegin(GL_TRIANGLES);
			
			glVertex3fv(points[0][i].data());
			glVertex3fv(points[0][i+slices+1].data());      
			glVertex3fv(points[0][i+1].data());
			
			glEnd();									//MEIO

			glBegin(GL_TRIANGLES);
			
			glVertex3fv(points[0][i+slices+1].data());
			glVertex3fv(points[0][i+slices+2].data());      
			glVertex3fv(points[0][i+1].data());
			
			glEnd();

			i++;
		}
		i++;
	}
	std :: cout << "Stacks: " << i << std::endl;

	for(int f = 0; f < slices - 1; f++)
	{

		if(f + 1 == slices)
		{
			glBegin(GL_TRIANGLES);
			
			glVertex3fv(points[0][i].data());
			glVertex3fv(points[0][(slices+1)*stacks].data());      // BASE
			glVertex3fv(points[0][i-(slices-1)].data());
			
			glEnd();
			break;
		}

		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_TRIANGLES);
		
		glVertex3fv(points[0][i].data());
		glVertex3fv(points[0][(slices+1)*stacks].data());
		glVertex3fv(points[0][i+1].data());

		glEnd();
		i++;
	}

	

}



void renderBoxPlane(std::string filePath, int which)
{
    int any = 0;
    int divisions = 0;
    points = parsePointsFromFile(filePath, any, divisions);


    for (int n = 0; n < which; n++)
    {
        int i = 0;
        std::cout << "Faces: " << which << std::endl;

        for (int j = 0; j < (divisions * divisions); j++)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_TRIANGLES);
            
            int plus = divisions + 1;
            
            // Primeiro triângulo (sentido horário - CW)
            glVertex3d(points[n][i][0], points[n][i][1], points[n][i][2]);
            glVertex3d(points[n][i + 1][0], points[n][i + 1][1], points[n][i + 1][2]);
            glVertex3d(points[n][i + plus + 1][0], points[n][i + plus + 1][1], points[n][i + plus + 1][2]);
            
            glEnd();
            glBegin(GL_TRIANGLES);
            
            // Segundo triângulo (sentido horário - CW)
            glVertex3d(points[n][i + plus + 1][0], points[n][i + plus + 1][1], points[n][i + plus + 1][2]);
            glVertex3d(points[n][i + plus][0], points[n][i + plus][1], points[n][i + plus][2]);
            glVertex3d(points[n][i][0], points[n][i][1], points[n][i][2]);
            
            glEnd();
            
            i++;
            if ((j + 1) % divisions == 0)
                i = (i + 1);
        }
        std::cout << "Face " << n << std::endl;
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

	// CENAS TESTE ////////////////////
	
	// Converter ângulos para radianos
    float radX = angleX * M_PI / 180.0f;
    float radY = angleY * M_PI / 180.0f;

    // Definir posição da câmara em torno do ponto (0,0,0)
    float radius = 4.0f; // Distância da câmara ao centro
    float camX = radius * cos(radX) * cos(radY);
    float camY = radius * sin(radY);
    float camZ = radius * sin(radX) * cos(radY);

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
			renderCone("../generatorResults/" + model);
		}
		if(model.find("sphere") != std::string::npos)
		{
			renderSphere("../generatorResults/" + model);
			//renderPoints();
		}
	}

	// End of frame
	glutSwapBuffers();
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        angleX += (x - lastMouseX) * 0.5f;
        angleY += (y - lastMouseY) * 0.5f;
        
        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay(); // Redesenhar a cena com nova rotação
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            mousePressed = false;
        }
    }
}


int main(int argc, char **argv) {

	
// init GLUT and the windo
	parseXML("../test files/test_files_phase_1/teste.xml");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
	glutCreateWindow("Projeto-CG@DI-UM");
	
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
// put here the registration of the keyboard callbacks

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);


//  OpenGL settings
	glEnable(GL_CULL_FACE);         // Habilita o culling
    glCullFace(GL_BACK);            
    glFrontFace(GL_CW); 
	glEnable(GL_DEPTH_TEST);
	
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}