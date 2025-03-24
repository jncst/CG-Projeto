#ifndef PARSERXML_H
#define PARSERXML_H

#include <vector>
#include <string>

void parseXML(const char* filename);

// Variables to store parsed data
extern int width, height;
extern float initialCamX, initialCamY, initialCamZ;
extern float lookAtX, lookAtY, lookAtZ;
extern float upX, upY, upZ;
extern float fov, near, far;


struct Transformation {
    std::string type; // "translate", "rotate", "scale"
    float x, y, z, angle; // angle for rotation
};

struct Model {
    std::string file;
};

//Estrutura onde serão armazenados os grupo com as transformações e os modelos a desenhar. 
// E basicamente uma arvore onde cada nó tem um grupo de transformações e modelos e subgrupos(que são grupos filhos)
// O resto das structs sao simples acho que nao exigem descrição
struct Group {
    std::vector<Transformation> transformations;
    std::vector<Model> models;
    std::vector<Group> subgroups; 
};

extern Group grupo;



#endif // PARSERXML_H
