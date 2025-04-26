
#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <cstdlib>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <map>
#include <filesystem>

#include "parserXML.h"

using namespace std;
namespace fs = std::filesystem;

//* VARIÁVEIS /////////////////////////////////////////////////////////////////////

// Camera
float angleX = 0.0f;
float angleY = 0.0f;
int lastMouseX, lastMouseY;
bool mousePressed = false;

float camera_radius = 3.0; // Distância da câmara ao centro
float minRadius = 1.0;
float maxRadius = 10000;
float maxAngleY = 60.0;
float minAngleY = -60.0;


map<string, GLuint> loadedModels;


float camX;
float camY;
float camZ;

// TESTING STUFF
int valor = 100; // valor para limitar numero de triangulos em funcoes para ajudar a vizualizar a contruçao do objeto


// VBO STUFF
vector <float> triangles;
GLuint vbo;

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

void loadTriangle (string line)
{
	float ax, ay, az, bx, by, bz, cx, cy, cz;
	
	stringstream ss(line);
    ss >> ax >> ay >> az >> bx >> by >> bz >> cx >> cy >> cz;

	triangles.push_back (ax);
	triangles.push_back (ay);
	triangles.push_back (az);

	triangles.push_back (bx);
	triangles.push_back (by);
	triangles.push_back (bz);

	triangles.push_back (cx);
	triangles.push_back (cy);
	triangles.push_back (cz);
}

void loadObject (string model)
{
	ifstream file("../generatorResults/" + model);

	string line = "";
	while (getline(file, line))
	{
		loadTriangle (line);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangles.size(), triangles.data(), GL_STATIC_DRAW);
	//triangles.clear();
}

void drawTriangles ()
{
	int numVertices = triangles.size() / 3;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLint bufferSize;
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glColor3f(1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	triangles.clear();
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
		case GLUT_KEY_UP: camera_radius -= 100.0f;
			if (camera_radius < minRadius)
				camera_radius = minRadius;
			break;
	
		case GLUT_KEY_DOWN: camera_radius += 100.0f;
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

void renderGroup(const Group& group) 
{
    
    glPushMatrix();


    for (const auto& transform : group.transformations) 
	{
        if (transform.type == "translate") 
		{
            glTranslatef(transform.x, transform.y, transform.z);
        }
        if (transform.type == "rotate") 
		{
            glRotatef(transform.angle, transform.x, transform.y, transform.z);
        }
        if (transform.type == "scale") 
		{
            glScalef(transform.x, transform.y, transform.z);
        }
    }

    // Draw models in this group
    for (const auto& model : group.models)
	{
        glPushMatrix();

        loadObject(model.file);
        drawTriangles();

        glPopMatrix();
    }

    for (const auto& subgroup : group.subgroups) 
	{
        renderGroup(subgroup);
    }

    glPopMatrix();
}



void renderScene() 
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();  // Reset transformations
	translateCameraPos();
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

	//* Draw world axes
	drawAxis();
	
	//Desenha o grupo, estrutura de dados que contem as transformacoes e os modelos
	renderGroup(grupo);

	// End of frame
	glutSwapBuffers();
}

//* FUNCOES DO OPENGL /////////////////////////////////////////////////////////////////////


//* MAIN ///////////////////////////////////////////////////////////////////////////////

void renderMain(string file)
{
	    // init GLUT and the windo
		parseXML((file).c_str());

		std::cout << "Numero de transformações: " << grupo.transformations.size() << std::endl;
		std::cout << "Numero de modelos: " << grupo.models.size() << std::endl;								//estas 3 linhas para debug
		std::cout << "Numero de subgrupos: " << grupo.subgroups.size() << std::endl;

		translateInitialCameraPos();
        glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);

        glutInitWindowPosition(300,300);
        glutInitWindowSize(width,height);
        glutCreateWindow("Projeto-CG@DI-UM");
		
		glewInit();
        
		glGenBuffers(1, &vbo);


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
        glEnable(GL_DEPTH_TEST);		// potencialmente comentar esta linha
        
    // enter GLUT's main cycle
        glutMainLoop();  
}


// Cenas de ler os ficheiros de teste
void listDirectory(const fs::path& currentPath, vector<fs::directory_entry>& items)
{
    items.clear();
    int index = 1;

    for (const auto& entry : fs::directory_iterator(currentPath))
	{
        items.push_back(entry);
        cout << index++ << ". " << entry.path().filename().string() << "\n";
    }
}

string readFile(const fs::path& filePath)
{
   ifstream file(filePath);

   if (!file)
	{
       cerr << "Erro ao abrir o ficheiro!\n";
       return "";
   }

   stringstream buffer;
   buffer << file.rdbuf(); // lê todo o conteúdo para o buffer
   return buffer.str();    // devolve como string
}


// int main(int argc, char **argv)
// {
// 	string line, test_number;

// 	glutInit(&argc, argv);
	
// 	fs::path currentPath = "../test files";
//     vector<fs::directory_entry> items;
//     int choice;
// 	string fileContent = "../test files/test_files_phase_2/test_2_5";

// 	//while (true)
// 	//{
// 	//	listDirectory(currentPath, items);
//  //       
// 	//	cout << "\nEscolhe um número (0 para sair): ";
// 	//	cin >> choice;

//  //       if (choice == 0)
// 	//	{
// 	//		break;
// 	//	}
//  //       if (choice < 1 || choice > items.size())
// 	//	{
//  //           cout << "Escolha inválida!\n";
//  //           continue;
//  //       }
//  //       
//  //       fs::directory_entry selected = items[choice - 1];

//  //       if (fs::is_directory(selected))
// 	//	{
//  //           currentPath = selected.path(); // Entra no diretório
//  //       } 
// 	//	else if (fs::is_regular_file(selected))
// 	//	{
//  //           fileContent = readFile(selected.path());
// 	//		break;
//  //       }
// 	//	else
// 	//	{
//  //           cout << "Item inválido!\n";
//  //       }
//  //   }

// 	//cout << fileContent;

// 	renderMain(fileContent);

// 	return 0;
// }

int main(int argc, char **argv)
{
	string line, test_number;

	glutInit(&argc, argv);
	
	fs::path currentPath = "../test files";
    vector<fs::directory_entry> items;
    int choice;
	string fileContent = "../test files/test_files_phase_2/test_2_5";

	while (true)
	{
		listDirectory(currentPath, items);
       
		cout << "\nEscolhe um número (0 para sair): ";
		cin >> choice;

       if (choice == 0)
		{
			break;
		}
       if (choice < 1 || choice > items.size())
		{
           cout << "Escolha inválida!\n";
           continue;
       }
       
       fs::directory_entry selected = items[choice - 1];

       if (fs::is_directory(selected))
		{
           currentPath = selected.path(); // Entra no diretório
       } 
		else if (fs::is_regular_file(selected))
		{
           fileContent = readFile(selected.path());
			break;
       }
		else
		{
           cout << "Item inválido!\n";
       }
   }

	cout << fileContent;

	renderMain(fileContent);

	return 0;
}