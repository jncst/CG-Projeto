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

struct Point
{
    float x, y, z;
};

struct Transformation
{
    std::string type; // "translate", "rotate", "scale"
    float x, y, z, angle;
    
    float time;
    bool align = false;
    std::vector<Point> points;
};

struct Material  
{  
    float diffuse[4];    // RGBA  
    float ambient[4];  
    float specular[4];  
    float emissive[4];  
    float shininess;  
}; 

struct Light {
    std::string type;
    float x, y, z;
    float dirX, dirY, dirZ;  
    float cutoff; 
};

extern std::vector<Light> lights;

struct Model {
    std::string file;
    std::string texture;
    Material material;
};

struct Group {
    std::vector<Transformation> transformations;
    std::vector<Model> models;
    std::vector<Group> subgroups;
};



extern Group grupo;

#endif
