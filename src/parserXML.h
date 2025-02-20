#ifndef PARSERXML_H
#define PARSERXML_H

#include <tinyxml2.h>
#include <vector>
#include <string>

void parseXML(const char* filename);

// Variables to store parsed data
extern int width, height;
extern float camX, camY, camZ;
extern float lookAtX, lookAtY, lookAtZ;
extern float upX, upY, upZ;
extern float fov, near, far;
extern std::vector<std::string> models;

#endif // PARSERXML_H
