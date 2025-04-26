#ifndef PARSERXML_H
#define PARSERXML_H

#include <vector>
#include <string>

void parseXML(const char* filename);

extern int width, height;
extern float initialCamX, initialCamY, initialCamZ;
extern float lookAtX, lookAtY, lookAtZ;
extern float upX, upY, upZ;
extern float fov, near, far;

struct Transformation {
    std::string type; // "translate", "rotate", "scale"
    float x, y, z, angle;
};

struct Model {
    std::string file;
};

struct Group {
    std::vector<Transformation> transformations;
    std::vector<Model> models;
    std::vector<Group> subgroups;
};

extern Group grupo;

#endif
