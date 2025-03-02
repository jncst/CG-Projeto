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
#include <math.h>

#include "parserXML.h"

using namespace std;

// Variáveis globais para controlar a rotação da câmara

bool CAMERA_ACTIVE = true; // Mudar para true para conseguir usar rato para mover camera

float angleX = 0.0f, angleY = 0.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

float camera_radius = 6.0f; // Distância da câmara ao centro
float minRadius = 0.1;
float maxRadius = 15;



// TESTING STUFF
int valor = 100; // valor para limitar numero de triangulos em funcoes para ajudar a vizualizar a contruçao do objeto


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

void translateCameraPos()
{
	camera_radius = sqrt(camX * camX + camY * camY + camZ * camZ);
	angleX = atan2(camZ, camX) * 180.0f / M_PI;
	angleY = asin(camY / camera_radius) * 180.0f / M_PI;
}


//* DESENHAR OBJETOS //////////////////////////////////////////////////////////////

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

	// Numero de quadrados que compõem o plano
	// int num_squares = num_slices * num_slices;

	int linhas = 1;

	//* Desenhar o plano

	// Iterar pelos vertices
	for (int i = 0; i < num_vertices; i++)
	{
		// Caso seja o ultimo vertice que queremos desenhar triangulos de uma linha
		if (i != 0 && (i + 1) % (num_slices + 1) == 0)
		{
			// Passa a proxima linha
			linhas ++;

			// Se a proxima linha for maior que o total de linhas para
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

	for (int i = 0; i < num_vertices; i ++)
	{
		getline (file, temp);
		vertices.push_back(getVertice(temp));
	}

	int linhas = 1;
	int face = 1;
	
	//* Desenhar o plano

	// Iterar pelos vertices
	for (int i = 0; i < num_vertices; i++)
	{
		// CENA DE TESTE
		if (i > valor)
		{
			break;
		}

		// Caso seja o ultimo vertice que queremos desenhar triangulos de uma linha
		if (i != 0 && (i + 1) % (num_slices + 1) == 0)
		{
			// Passa à proxima linha
			linhas ++;

			// Se a proxima linha for maior que o numero de linhas total
			if (linhas > num_slices)
			{
				// Passa à proxima face
				face ++;
				i += num_slices + 1;

				// Sai quando chega ao fim da ultima face
				if (face > 6)
				{
					break;
				}

				linhas = 1;
			}

			continue;
		}
		
		desenhaTriangulo(vertices[i], vertices[i + 1], vertices[i + num_slices + 1]);
		desenhaTriangulo(vertices[i + 1], vertices[i + num_slices + 2], vertices[i + num_slices + 1]);
	}
}

void AddSphere (string filepath)
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

	// Numero de Stacks
	getline (file, temp);
	int num_stacks = getNum(temp);

	// Cria lista de vertices
	vector <Vertice> vertices;

	for (int i = 0; i < num_vertices; i ++)
	{
		getline (file, temp);
		vertices.push_back(getVertice(temp));
	}

	// Primeira stack

	for (int i = 1 ; i <= num_slices; i++)
	{
		if (i == num_slices)
		{
			desenhaTriangulo(vertices [0], vertices [i], vertices [1]);
		}
		else
		{
			desenhaTriangulo(vertices [0], vertices [i], vertices [i + 1]);
		}
	}
	// Stacks intermediarias

	for (int i = 1 ; i <= num_vertices - 2 - num_slices ; i++)
	{
		if (i % num_slices == 0)
		{
			desenhaTriangulo(vertices[i], vertices[i + num_slices], vertices [i + 1 - num_slices]);
			desenhaTriangulo(vertices[i + 1], vertices [i + 1 - num_slices], vertices[i + num_slices]);
		}
		else
		{
			desenhaTriangulo(vertices[i], vertices[i + num_slices], vertices [i + 1]);
			desenhaTriangulo(vertices[i + 1], vertices[i + num_slices], vertices [i + num_slices + 1]);
		}
	}

	for (int i = 1 ; i <= num_slices; i++)
	{
		if (i == num_slices)
		{
			desenhaTriangulo(vertices [num_vertices - 1], vertices [num_vertices - 1 - i], vertices [num_vertices - 2]);
		}
		else
		{
			desenhaTriangulo(vertices [num_vertices - 1], vertices [num_vertices - 1 - i], vertices [num_vertices - 1 - i - 1]);
		}
	}
}

void AddCone (string filepath)
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

	// Numero de Stacks
	getline (file, temp);
	int num_stacks = getNum(temp);

	// Cria lista de vertices
	vector <Vertice> vertices;

	for (int i = 0; i < num_vertices; i ++)
	{
		getline (file, temp);
		vertices.push_back(getVertice(temp));
	}

	// Primeira stack

	//for (int i = 1 ; i <= valor ; i++)
	for (int i = 1 ; i <= num_slices; i++)
	{
		if (i == num_slices)
		{
			desenhaTriangulo(vertices [0], vertices [i], vertices [1]);
		}
		else
		{
			desenhaTriangulo(vertices [0], vertices [i], vertices [i + 1]);
		}
	}
	// Stacks intermediarias

	// for (int i = 1 ; i <= valor ; i++)
	for (int i = 1 ; i <= num_vertices - 2 - num_slices ; i++)
	{
		if (i % num_slices == 0)
		{
			desenhaTriangulo(vertices[i], vertices[i + num_slices], vertices [i + 1 - num_slices]);
			desenhaTriangulo(vertices[i + 1], vertices [i + 1 - num_slices], vertices[i + num_slices]);
		}
		else
		{
			desenhaTriangulo(vertices[i], vertices[i + num_slices], vertices [i + 1]);
			desenhaTriangulo(vertices[i + 1], vertices[i + num_slices], vertices [i + num_slices + 1]);
		}
	}

	for (int i = 1 ; i <= num_slices; i++)
	{
		if (i == num_slices)
		{
			desenhaTriangulo(vertices [num_vertices - 1], vertices [num_vertices - 1 - i], vertices [num_vertices - 2]);
		}
		else
		{
			desenhaTriangulo(vertices [num_vertices - 1], vertices [num_vertices - 1 - i], vertices [num_vertices - 1 - i - 1]);
		}
	}
}

//* DESENHAR OBJETOS //////////////////////////////////////////////////////////////



//* CENAS DE INPUT /////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'a') {
        valor++; // Incrementa a variável
    } else if (key == 's') {
		if (valor > 0)
		{
			valor--; // Decrementa a variável
		}
    }
    std::cout << "Valor: " << valor << std::endl;
    glutPostRedisplay(); // Atualiza a janela
}

void mouseMotion(int x, int y) {
    if (mousePressed) {
        angleX += (x - lastMouseX) * 0.5f;
        angleY += (y - lastMouseY) * 0.5f;

		if (angleY > 80.0f) angleY = 80.0f;
        if (angleY < -80.0f) angleY = -80.0f;
        
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

void specialKeys(int key, int x, int y) {
    if (key == GLUT_KEY_UP)
	{   // Simulate scroll up
        camera_radius -= 0.5f;
    } 
	else if (key == GLUT_KEY_DOWN)
	{	// Simulate scroll down
        camera_radius += 0.5f;
    }

    // Keep radius within limits
    if (camera_radius < minRadius) camera_radius = minRadius;
    if (camera_radius > maxRadius) camera_radius = maxRadius;

    std::cout << "Zoom level: " << camera_radius << std::endl;

    glutPostRedisplay(); // Refresh scene
}

//* CENAS DE INPUT /////////////////////////////////////////////////////////////////////


//* FUNCOES DO OPENGL /////////////////////////////////////////////////////////////////////

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
	

	glLoadIdentity();
	
	// Converter ângulos para radianos
	float radX = angleX * M_PI / 180.0f;
	float radY = angleY * M_PI / 180.0f;
	
	// Definir posição da câmara em torno do ponto (0,0,0)
	float camX = camera_radius * cos(radX) * cos(radY);
	float camY = camera_radius * sin(radY);
	float camZ = camera_radius * sin(radX) * cos(radY);
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

	//* Set the camera

    //* Desenha eixos x y z

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

	// End of framesa
	glutSwapBuffers();
}


//* FUNCOES DO OPENGL /////////////////////////////////////////////////////////////////////


//* MAIN ///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    // init GLUT and the windo
        parseXML("../test files/test_files_phase_1/test_1_1.xml");
		cout << "x: " << camX << " y: " << camY << " z: " << camZ << "\n";
		translateCameraPos();
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
        glutInitWindowPosition(100,100);
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
        glFrontFace(GL_CW); 
        glEnable(GL_DEPTH_TEST);
        
        
    // enter GLUT's main cycle
        glutMainLoop();
        
        return 1;
}