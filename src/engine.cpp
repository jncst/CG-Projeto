
#ifdef __APPLE__
#include <GL/glew.h>
#include <GLUT/glut.h>
#else
#include <cstdlib>
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <IL/il.h>

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

float zoom = 0.5f;

// Animation stuff
float TESSELATION = 0.01;


float camX;
float camY;
float camZ;

// TESTING STUFF
int valor = 100; // valor para limitar numero de triangulos em funcoes para ajudar a vizualizar a contruçao do objeto


// VBO STUFF
vector <float> triangles;
vector <float> normals;
vector <float> texturePoints;
GLuint vbo, vbo_normals, vbo_uv;

static std::map<std::string, GLuint> textureMap;

bool showLightSource = false;
bool showNormals = false;

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

GLuint loadTexture(std::string s)
{
    unsigned int t,tw,th;
    unsigned char *texData;
    unsigned int texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1,&texID);
    
    glBindTexture(GL_TEXTURE_2D,texID);

    glTexParameteri(GL_TEXTURE_2D,    GL_TEXTURE_WRAP_S,      GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,    GL_TEXTURE_WRAP_T,      GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,    GL_TEXTURE_MAG_FILTER,      GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,    GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

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

void loadNormals (string line)
{
	float ax, ay, az, bx, by, bz, cx, cy, cz;
	
	stringstream ss(line);
    ss >> ax >> ay >> az >> bx >> by >> bz >> cx >> cy >> cz;

	normals.push_back (ax);
	normals.push_back (ay);
	normals.push_back (az);

	normals.push_back (bx);
	normals.push_back (by);
	normals.push_back (bz);

	normals.push_back (cx);
	normals.push_back (cy);
	normals.push_back (cz);
}

void loadTexturePoints (string line)
{
	float ax, ay, bx, by, cx, cy; 
	
	stringstream ss(line);
    ss >> ax >> ay >> bx >> by >> cx >> cy;

	texturePoints.push_back (ax);
	texturePoints.push_back (ay);

	texturePoints.push_back (bx);
	texturePoints.push_back (by);
	texturePoints.push_back (cx);
	texturePoints.push_back (cy);
}



void loadObject (string model)
{
	ifstream file("../generatorResults/" + model);

	string line = "";
	while (getline(file, line))
	{
		loadTriangle (line);
		getline(file, line);
		loadNormals (line);
		getline(file, line);
		loadTexturePoints (line);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texturePoints.size(), texturePoints.data(), GL_STATIC_DRAW);


	//triangles.clear();
}

void drawTriangles ()
{
	int numVertices = triangles.size() / 3;


	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	
	// GLint bufferSize;
	// glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
	

	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (showNormals)
	{
		// Desenha as normais para debug
		glDisable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 0.0f);

		glBegin(GL_LINES);
		int numVerts = normals.size() / 3;   // cada normal tem 3 componentes
		for (int i = 0; i < numVerts; ++i)
		{
			// Posição do vértice
			float vx = triangles[3 * i + 0];
			float vy = triangles[3 * i + 1];
			float vz = triangles[3 * i + 2];

			// Normal correspondente
			float nx = normals[3 * i + 0];
			float ny = normals[3 * i + 1];
			float nz = normals[3 * i + 2];

			// Linha da normal: de v a v + n*scale
			glVertex3f(vx, vy, vz);
			glVertex3f(vx + nx * 0.2f,
						vy + ny * 0.2f,
						vz + nz * 0.2f);
		}
		glEnd();

		glEnable(GL_LIGHTING);
	}

	triangles.clear();
	normals.clear();
	texturePoints.clear();
}

void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}

void normalize(float *a) {

	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}


float length(float *v) {

	float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	return res;

}

void multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}

void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, float *pos, float *deriv) {

    // catmull-rom matrix
    float m[4][4] = {    {-0.5f,  1.5f, -1.5f,  0.5f},
                        { 1.0f, -2.5f,  2.0f, -0.5f},
                        {-0.5f,  0.0f,  0.5f,  0.0f},
                        { 0.0f,  1.0f,  0.0f,  0.0f}};
            
    // Compute A = M * P
    // 
        // Matriz de pontos de controlo
    float Px[4] = { p0.x, p1.x, p2.x, p3.x };    //for each component i: // x, y, z. In component i, P is the vector (p0[i], p1[i], p2[i],p3[i])
    float Py[4] = { p0.y, p1.y, p2.y, p3.y };
    float Pz[4] = { p0.z, p1.z, p2.z, p3.z };

    float Ax[4], Ay[4], Az[4];
    multMatrixVector((float *) m, Px, Ax);            // Resultado armazenado em A
    multMatrixVector((float *) m, Py, Ay);
    multMatrixVector((float *) m, Pz, Az);

    // Compute pos[i] = T * A

        // O vetor T = [t^3 t^2 t 1]    -> Vamos usar o valor t passado na função
    float T[4] = { powf(t,3), powf(t,2), t, 1 };

    pos[0] = T[0] * Ax[0] + T[1] * Ax[1] + T[2] * Ax[2] + T[3] * Ax[3];        //a multmatrixvector não deixa chamar os indices de pos por isso vai ter que ser assim
    pos[1] = T[0] * Ay[0] + T[1] * Ay[1] + T[2] * Ay[2] + T[3] * Ay[3];
    pos[2] = T[0] * Az[0] + T[1] * Az[1] + T[2] * Az[2] + T[3] * Az[3];
    
    // compute deriv[i] = T' * A

        // O vetor T' = [3t^2 2t 1 0]    -> Vamos usar o valor t passado na função
    float Tderiv[4] = { 3 * powf(t,2), 2* t, 1, 0 };

    deriv[0] = Tderiv[0] * Ax[0] + Tderiv[1] * Ax[1] + Tderiv[2] * Ax[2] + Tderiv[3] * Ax[3];
    deriv[1] = Tderiv[0] * Ay[0] + Tderiv[1] * Ay[1] + Tderiv[2] * Ay[2] + Tderiv[3] * Ay[3];
    deriv[2] = Tderiv[0] * Az[0] + Tderiv[1] * Az[1] + Tderiv[2] * Az[2] + Tderiv[3] * Az[3];
    // ...
}

void getCatmullGlobalRomPoint(float gt, float *pos, float *deriv, vector<Point> points) {

	int num_points = points.size();
	float t = gt * num_points; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4]; 
	indices[0] = (index + num_points-1)%num_points;	
	indices[1] = (indices[0]+1)%num_points;
	indices[2] = (indices[1]+1)%num_points; 
	indices[3] = (indices[2]+1)%num_points;

	getCatmullRomPoint(t, points[indices[0]], points[indices[1]], points[indices[2]], points[indices[3]], pos, deriv);
}

void renderCatmullRomCurve(vector <Point> points)
{
	// draw curve using line segments with GL_LINE_LOOP
	float pos [3];
	float deriv [3];
	float gt;

	glBegin(GL_LINE_LOOP);
	glColor3f (0, 0, 0);
	for (gt = 0; gt < 1; gt += TESSELATION)
	{
		getCatmullGlobalRomPoint(gt, pos, deriv, points);
		glVertex3f(pos[0], pos[1], pos[2]);
	}
	glColor3f (1, 1, 1);
	glEnd();

	glBegin(GL_LINES);
	glColor3f (0.7, 0.7, 0.7);
	for (gt = 0; gt < 1; gt += TESSELATION)
	{
		getCatmullGlobalRomPoint(gt, pos, deriv, points);
		glVertex3f(pos[0], pos[1], pos[2]);
		normalize(deriv);
		glVertex3f(pos[0] + deriv [0], pos[1] + deriv [1], pos[2] + deriv [2]);
	}
	glColor3f (1, 1, 1);
	glEnd();
}

//* DESENHAR OBJETOS //////////////////////////////////////////////////////////////

//* CENAS DE INPUT /////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
    if (key == 'l' || key == 'L')
	{
        showLightSource = !showLightSource;
    }

	if (key == 'n' || key == 'N')
	{
        showNormals = !showNormals;
    }
	
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
		case GLUT_KEY_UP: camera_radius -= zoom;
			if (camera_radius < minRadius)
				camera_radius = minRadius;
			break;
	
		case GLUT_KEY_DOWN: camera_radius += zoom;
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

void renderGroup(const Group& group, float elapsed_time) 
{
    glPushMatrix();


    for (const auto& transform : group.transformations) 
	{
        if (transform.type == "translate") 
		{
            glTranslatef(transform.x, transform.y, transform.z);
        } 
		if (transform.type == "translate-time")
		{
			float pos [3];
			float deriv [3];
			vector<Point> points = transform.points;

			renderCatmullRomCurve (points);
			
			float t = fmod(elapsed_time / transform.time, 1.0f);

			getCatmullGlobalRomPoint(t, pos, deriv, points);

			glTranslatef(pos[0], pos[1], pos[2]);

			if (transform.align)
			{
				float m [16];
				float Y [3] = {0 , 1 , 0};
			
				float x [3] = {deriv[0], deriv[1] , deriv [2]};
				normalize(x);
			
				float z [3];
				cross(x, Y, z);
				normalize(z);
			
				cross(z, x, Y);
				normalize(Y);
			
				buildRotMatrix(x, Y, z, m);
			
				glMultMatrixf(m);
			}
		}

		// SE FOR TRANSLATE TIME METEMOS AQUI NO IF E ELE VAI DAR TRANSLATE CONSOANTE O TEMPO
        if (transform.type == "rotate") 
		{
            glRotatef(transform.angle, transform.x, transform.y, transform.z);
        }
		if (transform.type == "rotate-time")
		{
			float angle = fmod((elapsed_time / transform.time) * 360.0f, 360.0f);
			glRotatef(angle, transform.x, transform.y, transform.z); 
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

		glMaterialfv(GL_FRONT, GL_DIFFUSE,   model.material.diffuse);  
		glMaterialfv(GL_FRONT, GL_AMBIENT,   model.material.ambient);  
		glMaterialfv(GL_FRONT, GL_SPECULAR,  model.material.specular);  
		glMaterialfv(GL_FRONT, GL_EMISSION,  model.material.emissive);  

		glMaterialf (GL_FRONT, GL_SHININESS, model.material.shininess);  

		if (!model.texture.empty())
		{
			glEnable(GL_TEXTURE_2D);
			auto it = textureMap.find(model.texture);
			if (it == textureMap.end())
			{
				// carrega apenas uma vez
				string textureFileName = "../textures/" + model.texture;

				GLuint id = loadTexture(textureFileName);
				textureMap[model.texture] = id;
				it = textureMap.find(model.texture);
			}
			// agora sim já existe
			glBindTexture(GL_TEXTURE_2D, it->second);
		}	
        loadObject(model.file);
        drawTriangles();
		
		// glDisable(GL_TEXTURE_2D);
		
        glPopMatrix();
    }

    for (const auto& subgroup : group.subgroups) 
	{
        renderGroup(subgroup, elapsed_time);
    }

    glPopMatrix();
}

void renderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	translateCameraPos();
	gluLookAt(camX, camY, camZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);

	for (size_t i = 0; i < lights.size() && i < 8; ++i)
	{
		auto &L = lights[i];
		GLenum LE = GL_LIGHT0 + i;

		// w = 0 para directional, w = 1 para point/spot
		if (L.type == "point")
		{
			GLfloat pos[4] = {
				L.x,
				L.y,
				L.z,
				1.0f
			};
			glLightfv(LE, GL_POSITION, pos);
		}

		else if (L.type == "directional")
		{
			GLfloat pos[4] = {
				L.dirY,
				L.dirX,
				L.dirZ,
				0.0f
			};
			glLightfv(LE, GL_POSITION, pos);
		}

		else if (L.type == "spot")
		{
			GLfloat pos[4] = {
				L.x,
				L.y,
				L.z,
				1.0f
			};
			glLightfv(LE, GL_POSITION, pos);

			GLfloat dir[3] = {
				L.dirX,
				L.dirY,
				L.dirZ
			};
			glLightfv(LE, GL_SPOT_DIRECTION, dir);
			glLightf(LE, GL_SPOT_CUTOFF, L.cutoff);
		}

		if (showLightSource)
		{
			glDisable(GL_LIGHTING);

			glPushMatrix();
			glTranslatef(L.x, L.y, L.z);
			
			glColor3f(1.0f, 1.0f, 1.0f);
			glutSolidSphere(0.1f, 20, 20);


			if (L.type == "directional")
			{
				glBegin(GL_LINES);
				glColor3f(1.0f, 1.0f, 0.0f);
				glVertex3f(L.x, L.y, L.z);
				glVertex3f(L.x - L.dirX, L.y - L.dirY, L.z - L.dirZ);
				glEnd();
			}

			if (L.type == "spot")
			{
				glBegin(GL_LINES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				
				glVertex3f(L.dirX, L.dirY, L.dirZ);
				glEnd();
			}

			glPopMatrix();
			glEnable(GL_LIGHTING);
		}
	}

	if (lights.size() > 0)
	{
		glDisable(GL_LIGHTING);
	}
	drawAxis();
	if (lights.size() > 0)
	{
		glEnable(GL_LIGHTING);
	}


	float elapsedMs = glutGet(GLUT_ELAPSED_TIME);
    float elapsed  = elapsedMs / 1000.0f;
	
	//Desenha o grupo, estrutura de dados que contem as transformacoes e os modelos
	renderGroup(grupo, elapsed);

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

	// glShadeModel(GL_SMOOTH);
	
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_normals);
	glGenBuffers(1, &vbo_uv);

	// cenas de luzes ig

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (lights.size() > 0)
	{
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
		float dark[4] = {0.2, 0.2, 0.2, 1.0};
		float white[4] = {1.0, 1.0, 1.0, 1.0};
		float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

		for (size_t i = 0; i < lights.size() && i < 8; ++i)
		{
			GLenum LE = GL_LIGHT0 + i;
	
			glEnable(LE);
	
			// light colors
			GLfloat diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat amb [4] = { 0.2f, 0.2f, 0.2f, 1.0f };
			GLfloat spec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
			glLightfv(LE, GL_AMBIENT, dark);
			glLightfv(LE, GL_DIFFUSE, white);
			glLightfv(LE, GL_SPECULAR, white);
		}
	
		// controls global ambient light
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
	}

    // Required callback registry 
        glutDisplayFunc(renderScene);
		glutIdleFunc(renderScene);
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
		glEnable(GL_RESCALE_NORMAL);
        
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
        if (entry.is_directory() 
            || (entry.is_regular_file() && entry.path().extension() == ".xml"))
        {
            items.push_back(entry);
            cout << index++ << ". " << entry.path().filename().string() << "\n";
        }
    }
}

string readFile(const fs::path& filePath)
{
    ifstream file(filePath);
    if (!file)
    {
        cerr << "Erro ao abrir o ficheiro: " << filePath << "\n";
        return "";
    }
    return filePath.string();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    fs::path currentPath = "../test files";
    vector<fs::directory_entry> items;
    int choice;
    string fileContent = "";

    while (true)
    {
        cout << "\nConteúdo de: " << currentPath.string() << "\n";
        listDirectory(currentPath, items);

        cout << "\nEscolhe um número (0 para sair): ";
        cin >> choice;

        if (choice == 0)
        {
            return 0;
        }
        if (choice < 1 || choice > (int)items.size())
        {
            cout << "Escolha inválida!\n";
            continue;
        }

        fs::directory_entry selected = items[choice - 1];

        if (fs::is_directory(selected))
        {
            currentPath = selected.path(); // entra no diretório
        }
        else // só pode ser ficheiro .xml aqui
        {
            fileContent = readFile(selected.path());
            break;
        }
    }

    cout << "Ficheiro XML escolhido: " << fileContent << "\n";
    renderMain(fileContent);

    return 0;
}
