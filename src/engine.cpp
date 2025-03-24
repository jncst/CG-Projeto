
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


std::map<std::string, GLuint> loadedModels;


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



//Chandre todas as alterações ao teu codigo fora o parser, foram so o acréscimo desta função e a mudança da load objects para aqui, para além disso tive de por um triangles.clear() algures porque senao eram desenhadas figuras a mais. Ja nao me lembro exatamente o que causava isso mas sempre que um subgrupo novo era desenhado todas as figuras dos grupos anteriores também eram redesenhadas.Acho que havia um problema qualquer em que a loadObjects era chamada sem nada para desenhar e desenhava o grupo anterior.Entao eu fino como sou em vez de tentar corrigir esse problema simplesmente dei clear ao vetor triangulos depois de desenhar cada grupo, assim a loadobjects continua a ser chamda em branco mas desenha em branco tambem haha. Nao sei se esse problema entretanto foi resolvido porque isso tinha haver com o parser e com a maneira como eu armazenava as cenas e acedia a elas. Mas como ja alterei muito toda essa estrutura desde ai se calhar tambem resolveu isso.




//A renderGroup basicamente pega na estrutura grupo e desenha todos os modelos com as transformações adequadas. Pelo que eu entendi do que e suposto fazer, que deve ser o certo visto que o nosso program devolve os resultados esperados, é que as transformações de um grupo devem ser aplicadas ao grupo e ao seus descendentes, ou seja, elas vao stackando. Se um grupo tem um translate de 1 para a direita e o seu filho tambem tem um translate de 1 para a direita o filho deve ser desenhado 2 para a direita. Toda essa logica de aplicar as translações so ao proprio grupo e aos descendentes e deita atraves do pushMatix e popMatrix. Man ja cansei de explicar

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

void renderMain(string test_number)
{
	    // init GLUT and the windo
		parseXML(("../test files/test_files_phase_2/test_2_" + test_number + ".xml").c_str());

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
        // glEnable(GL_DEPTH_TEST);
        
    // enter GLUT's main cycle
        glutMainLoop();  
}



int main(int argc, char **argv)
{
	string line, test_number;

	glutInit(&argc, argv);
	
	getline(cin, line);
	stringstream ss(line);

	ss >> test_number;

	renderMain(test_number);

	return 0;
}