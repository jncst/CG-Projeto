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

std::vector<std::vector<float> > points;


std::vector<std::vector<float> > parsePointsFromFile(const std::string& filename) 
{
    std::ifstream file(filename);
	
	std::vector<std::vector<float> > points;  

    if (!file.is_open()) 
	{  // Verifica se o arquivo foi aberto com sucesso
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return points;
    }

    int numPoints;
    file >> numPoints;  // Lê o número de pontos

    // Para limpar o caractere de nova linha (\n) que ficou após a leitura do número de pontos
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Lê os pontos e os armazena na lista de listas
    for (int i = 0; i < numPoints; ++i) 
	{
        std::string line;
        std::getline(file, line);  
        std::stringstream ss(line);  

        float x, y, z;
        ss >> x >> y >> z;  // Lê os valores de x, y, z

		std::vector<float> point;  // Cria o vetor de ponto
		point.push_back(x);
		point.push_back(y);
		point.push_back(z);

        points.push_back(point);
		
    }

    file.close();  

	return points;

}

std::pair<float, int> extractLengthAndDivisions(const std::string& filename) {
    std::string prefix = "plane_";
    std::string suffix = ".3d";

    // Verifica se o filename começa com "plane_" e termina com ".3d"
    if (filename.find(prefix) != 0 || filename.rfind(suffix) != filename.length() - suffix.length()) {
        std::cerr << "Formato inválido: " << filename << std::endl;
        return std::make_pair(0.0f, 0); // Retorna valores padrão em caso de erro
    }

    // Remove "plane_" do início e ".3d" do final
    std::string core = filename.substr(prefix.length(), filename.length() - prefix.length() - suffix.length());

    // Encontra o underscore que separa length e divisions
    size_t underscorePos = core.find('_');
    if (underscorePos == std::string::npos) {
        std::cerr << "Formato inválido: " << filename << std::endl;
        return std::make_pair(0.0f, 0);
    }

    // Extrai as substrings de length e divisions
    std::string lengthStr = core.substr(0, underscorePos);
    std::string divisionsStr = core.substr(underscorePos + 1);

    // Converte para float e int
    float length;
    int divisions;
    std::stringstream ssLength(lengthStr), ssDivisions(divisionsStr);

    if (!(ssLength >> length) || !(ssDivisions >> divisions)) {
        std::cerr << "Erro ao converter valores: " << filename << std::endl;
        return std::make_pair(0.0f, 0);
    }

    return std::make_pair(length, divisions);
}

void renderPlane(float length, int divisions, std::string filePath)
{
	points = parsePointsFromFile(filePath);
	int i = 0;

	for(int j = 0; j < divisions * divisions; j++)
	{
	
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		

		glVertex3d(points[i][0], points[i][1], points[i][2]);
		glVertex3d(points[i+1][0], points[i+1][1], points[i+1][2]);
		glVertex3d(points[i+4][0], points[i+4][1], points[i+4][2]);
		


		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		glVertex3d(points[i+1][0], points[i+1][1], points[i+1][2]);
		glVertex3d(points[i+4][0], points[i+4][1], points[i+4][2]);
		glVertex3d(points[i+5][0], points[i+5][1], points[i+5][2]);
		
		i++;
		if ((j+1) % divisions == 0)
			i = (i+1);

		
		glEnd();
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
			auto [length, division] = extractLengthAndDivisions(model);

			renderPlane(length, division, "../generatorResults/" + model);	
		}
		if(model.find("box") != std::string::npos)
		{
			std::cout << "Not implemented yet" << std::endl;
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
	parseXML("../test files/test_files_phase_1/test_1_5.xml");
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