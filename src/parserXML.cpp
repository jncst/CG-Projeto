#include "parserXML.h"
#include "tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

int width, height;
float initialCamX, initialCamY, initialCamZ;
float lookAtX, lookAtY, lookAtZ;
float upX = 0, upY = 1, upZ = 0;
float fov = 60, near = 1, far = 1000;
Group grupo;
std::vector<Light> lights;

static float getFloat(XMLElement* e, const char* attr, float def = 0.0f) {
    float val = def;
    e->QueryFloatAttribute(attr, &val);
    return val;
}

static int getInt(XMLElement* e, const char* attr, int def = 0) {
    int val = def;
    e->QueryIntAttribute(attr, &val);
    return val;
}

void parseGroup(XMLElement* xmlGroup, Group& group)
{
    for (XMLElement* elem = xmlGroup->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        std::string tag = elem->Value();

        // Recurse on <transform> wrapper
        if (tag == "transform") {
            parseGroup(elem, group);
            continue;
        }

        if (tag == "translate") {
            Transformation t;
            t.type = "translate";
            t.x = getFloat(elem, "x");
            t.y = getFloat(elem, "y");
            t.z = getFloat(elem, "z");
            t.angle = 0.0f;
            t.time = 0.0f;
            t.align = false;
            t.points.clear();

            // If has animation time, collect time, align and points
            if (elem->Attribute("time")) {
                t.time = getFloat(elem, "time");
                t.type = "translate-time";

                const char* alignAttr = elem->Attribute("align");
                t.align = (alignAttr && std::string(alignAttr) == "true");

                for (XMLElement* pt = elem->FirstChildElement("point"); pt; pt = pt->NextSiblingElement("point")) {
                    Point p;
                    p.x = getFloat(pt, "x");
                    p.y = getFloat(pt, "y");
                    p.z = getFloat(pt, "z");
                    t.points.push_back(p);
                }
            }

            group.transformations.push_back(t);
        }
        else if (tag == "rotate") {
            Transformation t;
            t.type = "rotate";
            t.angle = getFloat(elem, "angle");
            t.x = getFloat(elem, "x");
            t.y = getFloat(elem, "y");
            t.z = getFloat(elem, "z");
            t.time = elem->Attribute("time") ? getFloat(elem, "time") : 0.0f;
            t.align = false;
            t.points.clear();

            if (elem->Attribute("time"))
            {
                t.type = "rotate-time";
            }
            group.transformations.push_back(t);
        }
        else if (tag == "scale")
        {
            Transformation t;
            t.type = "scale";
            t.x = getFloat(elem, "x");
            t.y = getFloat(elem, "y");
            t.z = getFloat(elem, "z");
            t.angle = 0.0f;
            t.time = elem->Attribute("time") ? getFloat(elem, "time") : 0.0f;
            t.align = false;
            t.points.clear();
            group.transformations.push_back(t);
        }
        else if (tag == "models")
        {
            for (XMLElement* modelElem = elem->FirstChildElement("model"); modelElem; modelElem = modelElem->NextSiblingElement("model"))
            {
                Model m;
                if (const char* file = modelElem->Attribute("file"))
                m.file = file;
                
                if (XMLElement* texElem = modelElem->FirstChildElement("texture"))
                {
                    m.texture = texElem->Attribute("file");
                }

                if (XMLElement* colorElem = modelElem->FirstChildElement("color"))  
                {  
                    auto readRGB = [&](const char* tag, float out[4])  
                    {  
                        if (XMLElement* c = colorElem->FirstChildElement(tag))  
                        {  
                            float R = getFloat(c, "R") / 255.0f;  
                            float G = getFloat(c, "G") / 255.0f;  
                            float B = getFloat(c, "B") / 255.0f;  
                            out[0] = R; out[1] = G; out[2] = B; out[3] = 1.0f;  
                        }  
                    };  

                    readRGB("diffuse", m.material.diffuse);  
                    readRGB("ambient", m.material.ambient);  
                    readRGB("specular", m.material.specular);  
                    readRGB("emissive", m.material.emissive);  

                    if (XMLElement* s = colorElem->FirstChildElement("shininess"))  
                    {  
                        m.material.shininess = getFloat(s, "value");  
                    }  
                }  

                group.models.push_back(m);
            }
        }

        else if (tag == "group") {
            Group subgroup;
            parseGroup(elem, subgroup);
            group.subgroups.push_back(subgroup);
        }
    }
}

void parseXML(const char* filename) {
    XMLDocument doc;
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        std::cerr << "Erro a abrir XML: " << filename << std::endl;
        return;
    }

    // Accept <scene> or <world>
    XMLElement* root = doc.FirstChildElement("scene");
    if (!root) root = doc.FirstChildElement("world");
    if (!root) {
        std::cerr << "Elemento <scene> ou <world> não encontrado!" << std::endl;
        return;
    }

    // PARSE INFO DA JANELA

    if (XMLElement* window = root->FirstChildElement("window")) {
        width = getInt(window, "width");
        height = getInt(window, "height");
    }

    // PARSE INFO DA CAMERA

    if (XMLElement* camera = root->FirstChildElement("camera")) {
        if (XMLElement* pos = camera->FirstChildElement("position")) {
            initialCamX = getFloat(pos, "x");
            initialCamY = getFloat(pos, "y");
            initialCamZ = getFloat(pos, "z");
        }
        if (XMLElement* look = camera->FirstChildElement("lookAt")) {
            lookAtX = getFloat(look, "x");
            lookAtY = getFloat(look, "y");
            lookAtZ = getFloat(look, "z");
        }
        if (XMLElement* up = camera->FirstChildElement("up")) {
            upX = getFloat(up, "x", 0);
            upY = getFloat(up, "y", 1);
            upZ = getFloat(up, "z", 0);
        }
        if (XMLElement* proj = camera->FirstChildElement("projection")) {
            fov = getFloat(proj, "fov");
            near = getFloat(proj, "near");
            far = getFloat(proj, "far");
        }
    }

    // PARSE INFO DAS LUZES

    if (XMLElement* lightsElem = root->FirstChildElement("lights"))
    {
        for (XMLElement* lightElem = lightsElem->FirstChildElement("light"); lightElem; lightElem = lightElem->NextSiblingElement("light"))
        {
            Light L;
            L.type = lightElem->Attribute("type");
            
            if (L.type == "point" || L.type == "spot")  
            {  
                L.x = getFloat(lightElem, "posx");  
                L.y = getFloat(lightElem, "posy");  
                L.z = getFloat(lightElem, "posz");  
            }  
            if (L.type == "directional" || L.type == "spot")  
            {  
                L.dirX = getFloat(lightElem, "dirx");  
                L.dirY = getFloat(lightElem, "diry");  
                L.dirZ = getFloat(lightElem, "dirz");  
            }  
            if (L.type == "spot")  
            {  
                L.x = getFloat(lightElem, "posx");  
                L.y = getFloat(lightElem, "posy");  
                L.z = getFloat(lightElem, "posz");  

                L.dirX = getFloat(lightElem, "dirx");  
                L.dirY = getFloat(lightElem, "diry");  
                L.dirZ = getFloat(lightElem, "dirz");  

                L.cutoff = getFloat(lightElem, "cutoff");  
            }  
            lights.push_back(L);  
        }
    }

    for (XMLElement* elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement())
    {
        if (std::string(elem->Value()) == "group")
        {
            Group subGroup;
            parseGroup(elem, subGroup);
            grupo.subgroups.push_back(subGroup);
        }
    }
}