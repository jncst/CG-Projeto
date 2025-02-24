#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
#include "parserXML.h"

using namespace std;

void generatePlane(float length, int divisions, const string& filename)
{
	ofstream file("../generatorResults/" + filename);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}

	float subdiv = length / divisions;
	float half = length / 2;		//usado para depois podermos centrar na origem

	file << (divisions + 1) * (divisions + 1) << "\n";		//número de vértices no plano
	file << length << "\n";
	file << divisions << "\n";

	int i, j;
	float x, z;

	for (i = 0; i <= divisions; i++)
	{
		for (j = 0; j <= divisions; j++)
		{
			x = half - j * subdiv;
			z = half - i * subdiv;		//começa do ponto (half,0,half)

			file << x << " 0 " << z << "\n";		//escreve de cada ponto por linha x y z
		}
	}

	file.close();
}

void generateBox(float dimension, int divisions, const string& filename)
{
	ofstream file("../generatorResults/" + filename);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}

	float subdiv = dimension / divisions;
	float half = dimension / 2;		//usado para depois podermos centrar na origem

	file << (divisions + 1) * 3 << "\n";		//n�mero de vértices da box

	int i, j, k;
	float x, y, z;

	for (i = 0; i <= divisions; i++)
	{
		for (j = 0; j <= divisions; j++)
		{
			for (k = 0; k <= divisions; k++)
			{
				x = half - k * subdiv;
				y = half - j * subdiv;
				z = half - i * subdiv;		//começa do ponto (half,half,half)

				file << x << " " << y << " " << z << "\n";
			}
		}
	}

	file.close();
}

void generateSphere(float radius, int slices, int stacks, const string& filename)
{
	ofstream file("../generatorResults/" + filename);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}

	file << (slices + 1) * (stacks + 1) << "\n";

	int i, j;
	float alpha, beta;
	float x, y, z;

	for (i = 0; i <= stacks; i++)
	{
		beta = (M_PI * i / stacks) + (M_PI / 2);

		for (j = 0; j <= slices; j++)
		{
			alpha = 2 * M_PI * j / slices;

			x = radius * cos(beta) * sin(alpha);		//conversão para coordenadas polares
			y = radius * sin(beta);
			z = radius * cos(beta) * cos(alpha);

			file << x << " " << y << " " << z << "\n";
		}
	}

	file.close();
}

int main(int argc, char* argv[])
{
	string line, command, arg1, arg2, arg3, arg4;

	

	while (true)
	{
		getline(cin, line);
		stringstream ss(line);
	
		ss >> command >> arg1 >> arg2 >> arg3 >> arg4;

		if (command == "generator")
		{
			if(arg1 == "plane")
			{
				generatePlane(stof(arg2),stoi(arg3),arg4);
				cout << "Plane generated\n";
			}
			else if (arg1 == "box")
			{
				generateBox(stof(arg2),stoi(arg3),arg4);
				cout << "Box generated\n";
			}
			else if (arg1 == "cone")
			{
				cout << "Cone generated\n";
			}
			else if (arg1 == "sphere")
			{
				//generateSphere(stof(arg2), stoi(arg3), arg4, arg5);
				cout << "Sphere generated\n";
			}
			else
			{
				cout << "Invalid object\n";
			}
			
		}
		else if(command == "quit")
		{
			break;
		}
		else if(command == "test")
		{
		    // Parse the XML file
			parseXML("../test files/test_files_phase_1/test_1_5.xml");

			// Print parsed data
			std::cout << "Window: " << width << "x" << height << std::endl;
			std::cout << "Camera Position: (" << camX << ", " << camY << ", " << camZ << ")\n";
			std::cout << "LookAt: (" << lookAtX << ", " << lookAtY << ", " << lookAtZ << ")\n";
			std::cout << "Up Vector: (" << upX << ", " << upY << ", " << upZ << ")\n";
			std::cout << "Projection: FOV=" << fov << ", Near=" << near << ", Far=" << far << "\n";
		
			std::cout << "Models:" << std::endl;
			for (const std::string& model : models) 
			{
				std::cout << " - " << model << std::endl;
			}
			
		}
		else
		{
			cout << "Invalid command\n";
		}
	}

}