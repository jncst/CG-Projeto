#include "parserXML.h"
#include <iostream>

using namespace tinyxml2;

// Define the global variables
int width, height;
float camX, camY, camZ;
float lookAtX, lookAtY, lookAtZ;
float upX, upY, upZ;
float fov, near, far;
std::vector<std::string> models;

void parseXML(const char* filename) 
{
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(filename) != XML_SUCCESS) 
    {
        std::cerr << "Error loading file " << filename << std::endl;
        return;
    }

    // Window
    XMLElement* window = doc.FirstChildElement("world")->FirstChildElement("window");
    if (window) {
        width = window->IntAttribute("width");
        height = window->IntAttribute("height");
    }

    // Camera
    XMLElement* camera = doc.FirstChildElement("world")->FirstChildElement("camera");
    if (camera) {
        XMLElement* position = camera->FirstChildElement("position");
        if (position) {
            camX = position->FloatAttribute("x");
            camY = position->FloatAttribute("y");
            camZ = position->FloatAttribute("z");
        }

        XMLElement* lookAt = camera->FirstChildElement("lookAt");
        if (lookAt) {
            lookAtX = lookAt->FloatAttribute("x");
            lookAtY = lookAt->FloatAttribute("y");
            lookAtZ = lookAt->FloatAttribute("z");
        }

        XMLElement* up = camera->FirstChildElement("up");
        if (up) {
            upX = up->FloatAttribute("x");
            upY = up->FloatAttribute("y");
            upZ = up->FloatAttribute("z");
        }

        XMLElement* projection = camera->FirstChildElement("projection");
        if (projection) {
            fov = projection->FloatAttribute("fov");
            near = projection->FloatAttribute("near");
            far = projection->FloatAttribute("far");
        }
    }

    // Models
    models.clear();
    XMLElement* modelsNode = doc.FirstChildElement("world")
                                 ->FirstChildElement("group")
                                 ->FirstChildElement("models");

    if (modelsNode) {
        for (XMLElement* model = modelsNode->FirstChildElement("model"); model; model = model->NextSiblingElement("model")) 
        {
            const char* file = model->Attribute("file");
            if (file) {
                models.push_back(file);
            }
        }
    }
}



