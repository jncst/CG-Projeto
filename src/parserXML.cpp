// parserXML.cpp
#include "parserXML.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Definição das variáveis globais
int width, height;
float camX, camY, camZ;
float lookAtX, lookAtY, lookAtZ;
float upX, upY, upZ;
float fov, near, far;
std::vector<std::string> models;

// Função auxiliar para extrair valor de atributo
std::string getAttributeValue(const std::string& line, const std::string& attr) {
    std::string search = attr + "=\"";
    size_t pos = line.find(search);
    if (pos != std::string::npos) {
        pos += search.length();
        size_t endPos = line.find("\"", pos);
        if (endPos != std::string::npos) {
            return line.substr(pos, endPos - pos);
        }
    }
    return "";
}

void parseXML(const char* filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo " << filename << std::endl;
        return;
    }
    
    std::string line;
    models.clear();
    
    while (std::getline(file, line)) {
        // Window
        if (line.find("<window") != std::string::npos) {
            std::string widthStr = getAttributeValue(line, "width");
            std::string heightStr = getAttributeValue(line, "height");
            
            if (!widthStr.empty()) width = std::stoi(widthStr);
            if (!heightStr.empty()) height = std::stoi(heightStr);
        }
        
        // Camera position
        else if (line.find("<position") != std::string::npos) {
            std::string xStr = getAttributeValue(line, "x");
            std::string yStr = getAttributeValue(line, "y");
            std::string zStr = getAttributeValue(line, "z");
            
            if (!xStr.empty()) camX = std::stof(xStr);
            if (!yStr.empty()) camY = std::stof(yStr);
            if (!zStr.empty()) camZ = std::stof(zStr);
        }
        
        // Camera lookAt
        else if (line.find("<lookAt") != std::string::npos) {
            std::string xStr = getAttributeValue(line, "x");
            std::string yStr = getAttributeValue(line, "y");
            std::string zStr = getAttributeValue(line, "z");
            
            if (!xStr.empty()) lookAtX = std::stof(xStr);
            if (!yStr.empty()) lookAtY = std::stof(yStr);
            if (!zStr.empty()) lookAtZ = std::stof(zStr);
        }
        
        // Camera up
        else if (line.find("<up") != std::string::npos) {
            std::string xStr = getAttributeValue(line, "x");
            std::string yStr = getAttributeValue(line, "y");
            std::string zStr = getAttributeValue(line, "z");
            
            if (!xStr.empty()) upX = std::stof(xStr);
            if (!yStr.empty()) upY = std::stof(yStr);
            if (!zStr.empty()) upZ = std::stof(zStr);
        }
        
        // Camera projection
        else if (line.find("<projection") != std::string::npos) {
            std::string fovStr = getAttributeValue(line, "fov");
            std::string nearStr = getAttributeValue(line, "near");
            std::string farStr = getAttributeValue(line, "far");
            
            if (!fovStr.empty()) fov = std::stof(fovStr);
            if (!nearStr.empty()) near = std::stof(nearStr);
            if (!farStr.empty()) far = std::stof(farStr);
        }
        
        // Models
        else if (line.find("<model") != std::string::npos) {
            std::string fileStr = getAttributeValue(line, "file");
            if (!fileStr.empty()) {
                models.push_back(fileStr);
            }
        }
    }
    
    file.close();
}