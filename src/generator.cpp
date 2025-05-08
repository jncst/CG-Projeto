#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <vector>
#include <sstream>
#include <array>

using namespace std;


void cleanFile(const string& filename)
{
    // Limpa o ficheiro 
    ofstream file("../generatorResults/" + filename);		//abre o ficheiro para escrita
    if (!file)
    {
        cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }
}

void writeTriangle(const string& filename, float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
    ofstream file("../generatorResults/" + filename, ios::app);		//abre o ficheiro para escrita
    if (!file)
    {
        cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }

    file << ax << " " << ay << " " << az << " " << bx << " " << by << " " << bz << " " << cx << " " << cy << " " << cz << "\n";
}

void writeNormals(const string& filename, float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz)
{
    ofstream file("../generatorResults/" + filename, ios::app);		//abre o ficheiro para escrita
    if (!file)
    {
        cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }

    file << ax << " " << ay << " " << az << " " << bx << " " << by << " " << bz << " " << cx << " " << cy << " " << cz << " normals\n";
}

void normalize(float &x, float &y, float &z)
{
    float length = sqrt(x * x + y * y + z * z);

    if (length == 0.0f)
    {
        // Evita divisão por zero, mas deixa o vetor como está
        return;
    }

    x /= length;
    y /= length;
    z /= length;
}



//============================ BEZIER PATCHES ============================
struct Point
{
    float x, y, z;
};

vector <array<int, 16>> patches;         // cada patch tem 16 índices
vector <Point> controlPoints;

        //==== LEITURA ====

void readPatchFile(const string& filename, vector <array<int, 16>>& patches, vector <Point>& controlPoints)
{
    ifstream file(filename);        // ifstream é para leitura

    if (!file.is_open())
    {
        cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }

    string line;
    int numpatches;
    array <int, 16> patch;
    int numcontrol;

    getline(file, line);
    numpatches = stoi(line);        // número de patches

        // linhas com os índices dos patches
    for (int i = 0; i < numpatches; i++)
    {
        getline(file, line);
        stringstream ss(line);              // para extrair valores facilmente

        for (int j = 0; j < 16; j++)        // 16 índices por patch
        {
            string index;
            getline(ss, index, ',');        // ler até à vírgula
            patch[j] = stoi(index);         // pegar no índice e converter para número
        }

        patches.push_back(patch);           // guarda no vetor criado em cima
    }   

        // número de pontos de controlo
    getline(file, line);
    numcontrol = stoi(line);

        // linhas com os pontos de controlo
    for (int i = 0; i < numcontrol; i++)
    {
        getline(file, line);
        stringstream ss(line);

        Point p;
        char comma;                     // só para consumir as vírgulas

        ss >> p.x >> comma >> p.y >> comma >> p.z;
        controlPoints.push_back(p);     // armazenar o ponto
    }

    file.close();
}

    //===| LEITURA |===


float bernstein(int i, float t)             //Bi(t) = 3Ci * (1-t)^(3-i) * t^i
{
    switch (i)
    {
        case 0: return (1 - t) * (1 - t) * (1 - t);
        case 1: return 3 * t * (1 - t) * (1 - t);
        case 2: return 3 * t * t * (1 - t);
        case 3: return t * t * t;

        default: return 0;
    }
}


Point calculaBezier(float u, float v, Point grelha[4][4])       //u e v em [0,1]
{
    Point p = { 0.0f, 0.0f, 0.0f };
    float bu, bv;

    for (int i = 0; i < 4; i++)
    {
        bu = bernstein(i, u);

        for (int j = 0; j < 4; j++)
        {
            bv = bernstein(j, v);

            p.x += grelha[i][j].x * bu * bv;
            p.y += grelha[i][j].y * bu * bv;
            p.z += grelha[i][j].z * bu * bv;
        }
    }

    return p;
}


void tessellatePatch(Point grelha[4][4], vector<Point>& pontosFinais, int tessellationLevel)       //escreve nos pontos finais a partir da grelha de pontos de controlo
{
    float step = 1.0f / tessellationLevel;          // nível da tesselation (o valor que incrementa u e v)

    for (int i = 0; i < tessellationLevel; ++i)
    {

        float u = i * step;
        float uNext = (i + 1) * step;

        for (int j = 0; j < tessellationLevel; ++j)
        {
            float v = j * step;
            float vNext = (j + 1) * step;

                // Calcular os 4 pontos do quadrado atual
            Point p1 = calculaBezier(u, v, grelha);
            Point p2 = calculaBezier(uNext, v, grelha);
            Point p3 = calculaBezier(u, vNext, grelha);
            Point p4 = calculaBezier(uNext, vNext, grelha);

                // Triângulo 1: p1, p2, p3
            pontosFinais.push_back(p1);
            pontosFinais.push_back(p3);
            pontosFinais.push_back(p2);

                // Triângulo 2: p2, p4, p3
            pontosFinais.push_back(p2);
            pontosFinais.push_back(p3);
            pontosFinais.push_back(p4);
        }
    }
}

void generateBezier(const std::string& filenameIN, int tessellationLevel, const std::string& filenameOUT)     //função principal para os patches bezier ->     | generator patch teapot.patch 10 bezier_10.3d |
{
        // leitura do ficheiro
    readPatchFile(filenameIN, patches, controlPoints);

        // podemos meter o vetor de pontos finais fora que vai armazenar todas as patches aqui
    vector<Point> pontosFinais;

        // para cada patch, armazenamos os pontos de controlo numa grelha 4x4
    for (const auto& patch : patches)
    {
        Point grelha[4][4];

            // preenchimento da grelha
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
                grelha[i][j] = controlPoints[patch[i * 4 + j]];
        }

            // tesselação do patch
        tessellatePatch(grelha, pontosFinais, tessellationLevel);          //tenho os triângulos de um patch
    }

        // escrita dos pontos no ficheiro
    int size = pontosFinais.size();         //tamanho do array para iterar

    for (int i = 0; i < size; i += 3)
    {

        auto& A = pontosFinais[i];
        auto& B = pontosFinais[i + 1];
        auto& C = pontosFinais[i + 2];

        writeTriangle(filenameOUT, A.x, A.y, A.z,
                                B.x, B.y, B.z,
                                C.x, C.y, C.z);
    }
}

//===========================| BEZIER PATCHES |===========================

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

    float n1x = x;
    float n1y = y;
    float n1z = z;

    float n2x = x;
    float n2y = y;
    float n2z = z;

    float n3x = x;
    float n3y = y;
    float n3z = z;


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

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
        

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

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

        float n1x = 0;
        float n1y = -1;
        float n1z = 0;

        float n2x = cos((-M_PI / 2) + anguloVertical) * sin(anguloHorizontal * (i + 1));
        float n2y = sin((-M_PI / 2) + anguloVertical);
        float n2z = cos((-M_PI / 2) + anguloVertical) * cos(anguloHorizontal * (i + 1));

        float n3x = cos((-M_PI / 2) + anguloVertical) * sin(anguloHorizontal * i);
        float n3y = sin((-M_PI / 2) + anguloVertical);
        float n3z = cos((-M_PI / 2) + anguloVertical) * cos(anguloHorizontal * i);

        normalize(n1x, n1y, n1z);
        normalize(n2x, n2y, n2z);
        normalize(n3x, n3y, n3z);

        writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

            float n1x = cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * i);
            float n1y = sin((-M_PI / 2) + anguloVertical * j);
            float n1z = cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * i);

            float n2x = cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            float n2y = sin((-M_PI / 2) + anguloVertical * j);
            float n2z = cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            float n3x = cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            float n3y = sin((-M_PI / 2) + anguloVertical * (j + 1));
            float n3z = cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);

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

            n1x = cos((-M_PI / 2) + anguloVertical * j) * sin(anguloHorizontal * (i + 1));
            n1y = sin((-M_PI / 2) + anguloVertical * j);
            n1z = cos((-M_PI / 2) + anguloVertical * j) * cos(anguloHorizontal * (i + 1));

            n2x = cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * (i + 1));
            n2y = sin((-M_PI / 2) + anguloVertical * (j + 1));
            n2z = cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * (i + 1));

            n3x = cos((-M_PI / 2) + anguloVertical * (j + 1)) * sin(anguloHorizontal * i);
            n3y = sin((-M_PI / 2) + anguloVertical * (j + 1));
            n3z = cos((-M_PI / 2) + anguloVertical * (j + 1)) * cos(anguloHorizontal * i);

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

        float n1x = 0;
        float n1y = 1;
        float n1z = 0;

        float n2x = cos((M_PI / 2) - anguloVertical) * sin(anguloHorizontal * i);
        float n2y = sin((M_PI / 2) - anguloVertical);
        float n2z = cos((M_PI / 2) - anguloVertical) * cos(anguloHorizontal * i);

        float n3x = cos((M_PI / 2) - anguloVertical) * sin(anguloHorizontal * (i + 1));
        float n3y = sin((M_PI / 2) - anguloVertical);
        float n3z = cos((M_PI / 2) - anguloVertical) * cos(anguloHorizontal * (i + 1));

        normalize(n1x, n1y, n1z);
        normalize(n2x, n2y, n2z);
        normalize(n3x, n3y, n3z);

        writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

    float normalY = height / sqrt ( height * height + radius * radius  );
    
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

        float n1x = 0;
        float n1y = -1;
        float n1z = 0;

        float n2x = sin(anguloHorizontal * (i + 1));
        float n2y = 0;
        float n2z = cos(anguloHorizontal * (i + 1));

        float n3x = sin(anguloHorizontal * i);
        float n3y = 0;
        float n3z = cos(anguloHorizontal * i);

        normalize(n1x, n1y, n1z);
        normalize(n2x, n2y, n2z);
        normalize(n3x, n3y, n3z);

        writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

            float n1x = stackRadius * sin(anguloHorizontal * i);
            float n1y = normalY;
            float n1z = stackRadius * cos(anguloHorizontal * i);

            float n2x = stackRadius * sin(anguloHorizontal * (i + 1));
            float n2y = normalY;
            float n2z = stackRadius * cos(anguloHorizontal * (i + 1));

            float n3x = stackAboveRadius * sin(anguloHorizontal * i);
            float n3y = normalY;
            float n3z = stackAboveRadius * cos(anguloHorizontal * i);

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
            

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

            n1x = stackRadius * sin(anguloHorizontal * (i + 1));
            n1y = normalY;
            n1z = stackRadius * cos(anguloHorizontal * (i + 1));

            n2x = stackAboveRadius * sin(anguloHorizontal * (i + 1));
            n2y = normalY;
            n2z = stackAboveRadius * cos(anguloHorizontal * (i + 1));

            n3x = stackAboveRadius * sin(anguloHorizontal * i);
            n3y = normalY;
            n3z = stackAboveRadius * cos(anguloHorizontal * i);

            normalize(n1x, n1y, n1z);
            normalize(n2x, n2y, n2z);
            normalize(n3x, n3y, n3z);

            writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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

        float n1x = 0;
        float n1y = 1;
        float n1z = 0;

        float n2x = stackRadius * sin(anguloHorizontal * i);
        float n2y = height - stackHeight;
        float n2z = stackRadius * cos(anguloHorizontal * i);

        float n3x = stackRadius * sin(anguloHorizontal * (i + 1));
        float n3y = height - stackHeight;
        float n3z = stackRadius * cos(anguloHorizontal * (i + 1));

        normalize(n1x, n1y, n1z);
        normalize(n2x, n2y, n2z);
        normalize(n3x, n3y, n3z);

        writeNormals (filename, n1x, n1y, n1z, n2x, n2y, n2z, n3x, n3y, n3z);
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
    else if (string(argv[1]) == "patch")
    {
        cleanFile(argv [4]);
        generateBezier(argv [2], stoi(argv [3]), argv [4]);
        cout << "Bezier Surface generated\n";
    }
    else
    {
        cout << "Invalid object\n";
    }
}