#include "parserXML.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <memory>

// Definição das variáveis globais
int width, height;
float initialCamX, initialCamY, initialCamZ;
float lookAtX, lookAtY, lookAtZ;
float upX = 0, upY = 1, upZ = 0;
float fov = 60, near = 1, far = 1000;
Group grupo;

// Função auxiliar para extrair valor de atributo
std::string getAttributeValue(const std::string& line, const std::string& attr) 
{
    // Find the attribute in the line
    size_t attrPos = line.find(attr);
    if (attrPos == std::string::npos) return "";

    // Find the equals sign, allowing for spaces
    size_t equalsPos = line.find('=', attrPos);
    if (equalsPos == std::string::npos) return "";

    // Find the first quote after the equals sign
    size_t startQuotePos = line.find_first_of("\"'", equalsPos);
    if (startQuotePos == std::string::npos) return "";

    // Determine quote type
    char quote = line[startQuotePos];

    // Find the closing quote
    size_t endQuotePos = line.find(quote, startQuotePos + 1);
    if (endQuotePos == std::string::npos) return "";

    // Extract the value between quotes
    std::string value = line.substr(startQuotePos + 1, endQuotePos - startQuotePos - 1);

    // Trim whitespace
    size_t first = value.find_first_not_of(" \t");
    size_t last = value.find_last_not_of(" \t");
    
    if (first != std::string::npos && last != std::string::npos) {
        return value.substr(first, last - first + 1);
    }

    return value;
}

// Isto foi para debbugging
float safeStof(const std::string& value, const std::string& attributeName, const std::string& line) 
{
    if (value.empty())
    {
        std::cerr << "Error: Empty value for attribute " << attributeName 
                << ". Line: " << line << std::endl;
        return 0.0f;
    }
    
    try 
    {
        return std::stof(value);
    } catch (const std::invalid_argument& e) 
    {
        std::cerr << "Invalid argument for " << attributeName 
                << ": '" << value << "'. Line: " << line << std::endl;
        return 0.0f;
    } catch (const std::out_of_range& e) 
    {
        std::cerr << "Out of range value for " << attributeName 
                << ": '" << value << "'. Line: " << line << std::endl;
        return 0.0f;
    }
}

// Isto tambem foi para debbugging
int safeStoi(const std::string& value, const std::string& attributeName, const std::string& line) {
    if (value.empty()) {
        std::cerr << "Error: Empty value for attribute " << attributeName 
                  << ". Line: " << line << std::endl;
        return 0;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument for " << attributeName 
                  << ": '" << value << "'. Line: " << line << std::endl;
        return 0;
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range value for " << attributeName 
                  << ": '" << value << "'. Line: " << line << std::endl;
        return 0;
    }
}

void parseXML(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return;
    }
    
    std::string line;
    std::stack<Group*> groupStack;
    grupo = Group();
    groupStack.push(&grupo);
    
    while (std::getline(file, line)) {
        // Debug: print each line for detailed inspection
        // std::cout << "Processing line: [" << line << "]" << std::endl;
        
        // Trim leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line.substr(0, 4) == "<!--") continue;
        
        try {
            // Window
            if (line.find("<window") != std::string::npos) {
                std::string widthStr = getAttributeValue(line, "width");
                std::string heightStr = getAttributeValue(line, "height");
                
                width = safeStoi(widthStr, "width", line);
                height = safeStoi(heightStr, "height", line);
            }
            // Camera position
            else if (line.find("<position") != std::string::npos) {
                initialCamX = safeStof(getAttributeValue(line, "x"), "x", line);
                initialCamY = safeStof(getAttributeValue(line, "y"), "y", line);
                initialCamZ = safeStof(getAttributeValue(line, "z"), "z", line);
            }
            // Camera lookAt
            else if (line.find("<lookAt") != std::string::npos) {
                lookAtX = safeStof(getAttributeValue(line, "x"), "x", line);
                lookAtY = safeStof(getAttributeValue(line, "y"), "y", line);
                lookAtZ = safeStof(getAttributeValue(line, "z"), "z", line);
            }
            // Camera up
            else if (line.find("<up") != std::string::npos) {
                upX = safeStof(getAttributeValue(line, "x"), "x", line);
                upY = safeStof(getAttributeValue(line, "y"), "y", line);
                upZ = safeStof(getAttributeValue(line, "z"), "z", line);
            }
            // Camera projection
            else if (line.find("<projection") != std::string::npos) {
                fov = safeStof(getAttributeValue(line, "fov"), "fov", line);
                near = safeStof(getAttributeValue(line, "near"), "near", line);
                far = safeStof(getAttributeValue(line, "far"), "far", line);
            }
            // Group opening
            else if (line.find("<group") != std::string::npos) {
                Group* currentGroup = groupStack.top();
                
                // Create a new group and add it to the current group's subgroups
                currentGroup->subgroups.push_back(Group());
                groupStack.push(&(currentGroup->subgroups.back()));
            }
            // Group closing
            else if (line.find("</group>") != std::string::npos) {
                if (!groupStack.empty()) {
                    groupStack.pop();
                }
            }
            // Translate transformation
            else if (line.find("<translate") != std::string::npos) {
                Transformation transform;
                transform.type = "translate";
                transform.x = safeStof(getAttributeValue(line, "x"), "x", line);
                transform.y = safeStof(getAttributeValue(line, "y"), "y", line);
                transform.z = safeStof(getAttributeValue(line, "z"), "z", line);
                
                if (!groupStack.empty()) {
                    groupStack.top()->transformations.push_back(transform);
                }
            }
            // Rotate transformation
            else if (line.find("<rotate") != std::string::npos) {
                Transformation transform;
                transform.type = "rotate";
                transform.angle = safeStof(getAttributeValue(line, "angle"), "angle", line);
                transform.x = safeStof(getAttributeValue(line, "x"), "x", line);
                transform.y = safeStof(getAttributeValue(line, "y"), "y", line);
                transform.z = safeStof(getAttributeValue(line, "z"), "z", line);
                
                if (!groupStack.empty()) {
                    groupStack.top()->transformations.push_back(transform);
                }
            }
            // Scale transformation
            else if (line.find("<scale") != std::string::npos) {
                Transformation transform;
                transform.type = "scale";
                transform.x = safeStof(getAttributeValue(line, "x"), "x", line);
                transform.y = safeStof(getAttributeValue(line, "y"), "y", line);
                transform.z = safeStof(getAttributeValue(line, "z"), "z", line);
                
                if (!groupStack.empty()) {
                    groupStack.top()->transformations.push_back(transform);
                }
            }
            // Model
            else if (line.find("<model") != std::string::npos) {
                Model model;
                model.file = getAttributeValue(line, "file");
                
                if (!groupStack.empty()) {
                    groupStack.top()->models.push_back(model);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while processing line: " << line 
                      << "\nError: " << e.what() << std::endl;
        }
    }
    
    file.close();
}