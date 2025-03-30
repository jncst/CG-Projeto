#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <sstream>

using namespace std;

void cleanFile (const string& filename)
{
    // Limpa o ficheiro 
    ofstream file ("../generatorResults/" + filename);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}
}

void writeTriangle (const string& filename, float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
    ofstream file ("../generatorResults/" + filename, ios::app);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}

    file << ax << " " << ay << " " << az << " " << bx << " " << by << " " << bz << " " << cx << " " << cy << " " << cz << "\n";
}

// É TUDO COUNTER CLOCK WISE

void generatePlane(int x, int y, int z, int centered, float length, int divisions, const string& filename)
{
    centered = 1 - centered;

    float subDivision = length / divisions;
    float half = length / 2;

    float startX = half * ((x * centered) + y - z);
    float startY = half * (-abs(x) + (y * centered) - abs(z));
    float startZ = half * (x + abs(y) + (z * centered));

    float addHorizontalX = subDivision * (z);
    float addHorizontalY = subDivision * 0;
    float addHorizontalZ = subDivision * (-x - abs(y));

    float addVerticalX = subDivision * (-y);
    float addVerticalY = subDivision * (abs(x) + abs(z));
    float addVerticalZ = subDivision * 0;

    // cout << "x start: " << startX << "\n";
    // cout << "y start: " << startY << "\n";
    // cout << "z start: " << startZ << "\n";

    // cout << "addh: (" << addHorizontalX << ", " << addHorizontalY << ", " << addHorizontalZ << ")" << "\n";
    // cout << "addv: (" << addVerticalX << ", " << addVerticalY << ", " << addVerticalZ << ")" << "\n";

    // Loop que varia o x
    for (int i = 0; i < divisions; i ++)
    {
        // Loop que varia o z
        for (int j = 0; j < divisions; j ++)
        {
            // C      C         B
            // +       +-------+
            // |\       \      |
            // | \       \     |
            // |  \       \    |
            // |   \       \   |
            // |    \        \ |
            // |_____\         |
            // A      B         A

            // Triangulo 1
            float ax = startX + (i * addHorizontalX) + (j * addVerticalX);
            float ay = startY + (i * addHorizontalY) + (j * addVerticalY);
            float az = startZ + (i * addHorizontalZ) + (j * addVerticalZ);

            float bx = startX + ((i + 1) * addHorizontalX) + (j * addVerticalX);
            float by = startY + ((i + 1) * addHorizontalY) + (j * addVerticalY);
            float bz = startZ + ((i + 1) * addHorizontalZ) + (j * addVerticalZ);

            float cx = startX + (i * addHorizontalX) + ((j + 1) * addVerticalX);
            float cy = startY + (i * addHorizontalY) + ((j + 1) * addVerticalY);
            float cz = startZ + (i * addHorizontalZ) + ((j + 1) * addVerticalZ);
           
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
        

            // Triangulo 2
            ax = startX + ((i + 1) * addHorizontalX) + (j * addVerticalX);
            ay = startY + ((i + 1) * addHorizontalY) + (j * addVerticalY);
            az = startZ + ((i + 1) * addHorizontalZ) + (j * addVerticalZ);

            bx = startX + ((i + 1) * addHorizontalX) + ((j + 1) * addVerticalX);
            by = startY + ((i + 1) * addHorizontalY) + ((j + 1) * addVerticalY);
            bz = startZ + ((i + 1) * addHorizontalZ) + ((j + 1) * addVerticalZ);

            cx = startX + (i * addHorizontalX) + ((j + 1) * addVerticalX);
            cy = startY + (i * addHorizontalY) + ((j + 1) * addVerticalY);
            cz = startZ + (i * addHorizontalZ) + ((j + 1) * addVerticalZ);
            
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
        }
    }
}

void generateBox(float length, int divisions, const std::string& filename)
{
    generatePlane(1, 0, 0, 0, length,divisions,filename);
    generatePlane(-1, 0, 0, 0, length,divisions,filename);
    generatePlane(0, 1, 0, 0, length,divisions,filename);
    generatePlane(0, -1, 0, 0, length,divisions,filename);
    generatePlane(0, 0, 1, 0, length,divisions,filename);
    generatePlane(0, 0, 0-1, 0, length,divisions,filename);
}

void generateSphere(float radius, int slices, int stacks, const string& filename)
{
    float anguloHorizontal = (2 * M_PI) / slices;
    float anguloVertical = M_PI / stacks;

    // Triangulos Base
    // ...C     BC    B...
    //    \____|____/
    //     \   |   /
    //      \  |  /
    //       \ | /
    //        \|/
    //         A
    
    for (int i = 0; i < slices; i ++)
    {
        float ax = 0;
        float ay = -radius ;
        float az = 0;

        float bx = radius * cos((-M_PI / 2) + anguloVertical) * sin(anguloHorizontal * (i + 1));
        float by = radius * sin((-M_PI / 2) + anguloVertical);
        float bz = radius * cos((-M_PI / 2) + anguloVertical) * cos(anguloHorizontal * (i + 1));

        float cx = radius * cos((-M_PI / 2) + anguloVertical) * sin(anguloHorizontal * i);
        float cy = radius * sin((-M_PI / 2) + anguloVertical);
        float cz = radius * cos((-M_PI / 2) + anguloVertical) * cos(anguloHorizontal * i);

        writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
    }


    // Triangulos Centrais
    // C      C         B
    // +       +-------+
    // |\       \      |
    // | \       \     |
    // |  \       \    |
    // |   \       \   |
    // |    \        \ |
    // |_____\         |
    // A      B         A

    for (int j = 1; j < stacks - 1; j ++)
    {
        for (int i = 0; i < slices; i ++)
        {
            // Triangulo 1
            float ax = radius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * i);
            float ay = radius * sin((-M_PI / 2) + anguloVertical * j);
            float az = radius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * i);

            float bx = radius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            float by = radius * sin((-M_PI / 2) + anguloVertical * j);
            float bz = radius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            float cx = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            float cy = radius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            float cz = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);
           
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);

            // Triangulo 2
            ax = radius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            ay = radius * sin((-M_PI / 2) + anguloVertical * j);
            az = radius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            bx = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * (i + 1));
            by = radius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            bz = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * (i + 1));

            cx = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            cy = radius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            cz = radius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);
            
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
        }
    }
    
    // Triangulos Topo
    //          C
    //         /|\
    //        / | \
    //       /  |  \
    //      /   |   \
    //     /____|____\
    // ...A     BA    B...

    for (int i = 0; i < slices; i ++)
    {
        float ax = 0;
        float ay = radius ;
        float az = 0;

        float bx = radius * cos((M_PI / 2) - anguloVertical) * sin(anguloHorizontal * i);
        float by = radius * sin((M_PI / 2) - anguloVertical);
        float bz = radius * cos((M_PI / 2) - anguloVertical) * cos(anguloHorizontal * i);

        float cx = radius * cos((M_PI / 2) - anguloVertical) * sin(anguloHorizontal * (i + 1));
        float cy = radius * sin((M_PI / 2) - anguloVertical);
        float cz = radius * cos((M_PI / 2) - anguloVertical) * cos(anguloHorizontal * (i + 1));

        writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
    }
}



void generateCone(float radius, float height, int slices, int stacks, const std::string& filename)
{
    float anguloHorizontal = (2 * M_PI) / slices;
    float stackHeight = height / stacks;

    // Triangulos Base
    // ...C     BC    B...
    //    \____|____/
    //     \   |   /
    //      \  |  /
    //       \ | /
    //        \|/
    //         A
    
    for (int i = 0; i < slices; i ++)
    {
        float ax = 0;
        float ay = 0 ;
        float az = 0;

        float bx = radius * sin(anguloHorizontal * (i + 1));
        float by = 0;
        float bz = radius * cos(anguloHorizontal * (i + 1));

        float cx = radius * sin(anguloHorizontal * i);
        float cy = 0;
        float cz = radius * cos(anguloHorizontal * i);

        writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
    }


    // Triangulos Centrais
    // C      C         B
    // +       +-------+
    // |\       \      |
    // | \       \     |
    // |  \       \    |
    // |   \       \   |
    // |    \        \ |
    // |_____\         |
    // A      B         A

    for (int j = 0; j < stacks - 1; j ++)
    {
        float thisStackHeight = j * stackHeight;
        float stackRadius = (radius * (height - thisStackHeight)) / height;
        float stackAboveRadius = (radius * (height - thisStackHeight - stackHeight)) / height;
    
        for (int i = 0; i < slices; i ++)
        {
            // Triangulo 1
            float ax = stackRadius * sin(anguloHorizontal * i);
            float ay = thisStackHeight;
            float az = stackRadius * cos(anguloHorizontal * i);

            float bx = stackRadius * sin(anguloHorizontal * (i + 1));
            float by = thisStackHeight;
            float bz = stackRadius * cos(anguloHorizontal * (i + 1));

            float cx = stackAboveRadius * sin(anguloHorizontal * i);
            float cy = thisStackHeight + stackHeight;
            float cz = stackAboveRadius * cos(anguloHorizontal * i);
           
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);

            // Triangulo 2
            ax = stackRadius * sin(anguloHorizontal * (i + 1));
            ay = thisStackHeight;
            az = stackRadius * cos(anguloHorizontal * (i + 1));

            bx = stackAboveRadius * sin(anguloHorizontal * (i + 1));
            by = thisStackHeight + stackHeight;
            bz = stackAboveRadius * cos(anguloHorizontal * (i + 1));

            cx = stackAboveRadius * sin(anguloHorizontal * i);
            cy = thisStackHeight + stackHeight;
            cz = stackAboveRadius * cos(anguloHorizontal * i);

            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
        }
    }
    
    // Triangulos Topo
    //          C
    //         /|\
    //        / | \
    //       /  |  \
    //      /   |   \
    //     /____|____\
    // ...A     BA    B...

    for (int i = 0; i < slices; i ++)
    {
        float stackRadius = (radius * stackHeight) / height;
        float thisStackHeight =  height - stackHeight;

        float ax = 0;
        float ay = height;
        float az = 0;

        float bx = stackRadius * sin(anguloHorizontal * i);
        float by = height - stackHeight;
        float bz = stackRadius * cos(anguloHorizontal * i);

        float cx = stackRadius * sin(anguloHorizontal * (i + 1));
        float cy = height - stackHeight;
        float cz = stackRadius * cos(anguloHorizontal * (i + 1));

        writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
    }
}

void generateTorus(float insideRadius, float outsideRadius, int slices, int stacks, const std::string& filename)
{
    float anguloVertical = M_PI / stacks;
    float anguloHorizontal = (2 * M_PI) / slices;

    // Triangulos Centrais
    // C      C         B
    // +       +-------+
    // |\       \      |
    // | \       \     |
    // |  \       \    |
    // |   \       \   |
    // |    \        \ |
    // |_____\         |
    // A      B         A

    for (int i = 0; i < slices; i ++)
    {
        float startx = outsideRadius * sin(anguloHorizontal * i);
        float startz = outsideRadius * cos(anguloHorizontal * i);

        for (int j = 0; j < stacks * 2; j ++)
        {
            // Triangulo 1
            float ax = outsideRadius * sin(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * i);
            float ay = insideRadius * sin((-M_PI / 2) + anguloVertical * j);
            float az = outsideRadius * cos(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * i);

            float bx = outsideRadius * sin(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            float by = insideRadius * sin((-M_PI / 2) + anguloVertical * j);
            float bz = outsideRadius * cos(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            float cx = outsideRadius * sin(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            float cy = insideRadius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            float cz = outsideRadius * cos(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);
           
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);

            // Triangulo 2
            ax = outsideRadius * sin(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            ay = insideRadius * sin((-M_PI / 2) + anguloVertical * j);
            az = outsideRadius * cos(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            bx = outsideRadius * sin(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * (i + 1));
            by = insideRadius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            bz = outsideRadius * cos(anguloHorizontal * (i + 1)) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * (i + 1));

            cx = outsideRadius * sin(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            cy = insideRadius * sin((-M_PI / 2) + anguloVertical * (j + 1));
            cz = outsideRadius * cos(anguloHorizontal * i) + insideRadius * cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);
            
            writeTriangle (filename, ax, ay, az, bx, by, bz, cx, cy, cz);
        }
    }
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < 7; i ++)
    {
        if (argv [i] != NULL)
        {
            cout << "argument" << i << ": " << argv [i] << "\n";
        }
    }

    if(string(argv[1]) == "plane")
    {
        cleanFile (argv [4]);
        generatePlane(0, 1, 0, 1, stof(argv [2]),stoi(argv [3]),argv [4]);
        generatePlane(0, -1, 0, 1, stof(argv [2]),stoi(argv [3]),argv [4]);
        cout << "Plane generated\n";
    }
    else if (string(argv[1]) == "box")
    {
        cleanFile (argv [4]);
        generateBox(stof(argv [2]),stoi(argv [3]),argv [4]);
        cout << "Box generated\n";
    }
    else if (string(argv[1]) == "sphere")
    {
        cleanFile (argv [5]);
        generateSphere(stof(argv [2]), stoi(argv [3]), stoi(argv [4]), argv [5]);
        cout << "Sphere generated\n";
    }
    else if (string(argv[1]) == "cone")
    {
        cleanFile (argv [6]);
        generateCone(stof(argv [2]),stof(argv [3]),stoi(argv [4]),stoi(argv [5]), argv [6]);
        cout << "Cone generated\n";
    }
    else if (string(argv[1]) == "torus")
    {
        cleanFile (argv [6]);
        generateTorus(stof(argv [2]), stof(argv [3]), stof(argv [4]), stoi(argv [5]), argv [6]);
        cout << "Torus generated\n";
    }
    else
    {
        cout << "Invalid object\n";
    }
}