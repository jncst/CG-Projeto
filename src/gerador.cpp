#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>

using namespace std;

void generateCone(float radius, float height, int slices, int stacks, const std::string& filename)
{
    std::ofstream file("../generatorResults/" + filename);
    if (!file)
    {
        std::cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }

	file << "CONE\n";
	file << "VERT " << slices * stacks + 2 << "\n";		//número de vértices da esfera
	file << "SLICES " << slices << "\n";
	file << "STACKS " << stacks << "\n";

    float angleStep = 2 * M_PI / slices;
    float stackHeight = height / stacks;
	
	// Centro da base
	file << "0 0 0\n";
	
    // Gerar os vértices das stacks (camada por camada)
    for (int i = 0; i < stacks; i++) 
    {
        float r = (radius * (height - (i * stackHeight))) / height;
        float y = i * stackHeight;  // Mantemos Y como altura

        for (int j = 0; j < slices; j++)
        {
            float theta = j * angleStep;

            float x = r * cos(theta);  // Posição X no círculo
            float z = - r * sin(theta);  // Posição Z no círculo
            //file << "x: " << x << " y: " << y << " z: " << z << "\n";
			file << x << " " << y << " " << z << "\n";
        }
    }

	// Centro da Topo
	file << "0 " << height << " 0\n";

    file.close();
}

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

	file << "PLANE\n";
	file << "VERT "<<(divisions + 1) * (divisions + 1) << "\n";		//número de vértices no plano	
	file << "SLICES " << divisions << "\n";

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

void generateBox(float length, int divisions, const std::string& filename)
{
    std::ofstream file("../generatorResults/" + filename);
    if (!file)
    {
        std::cerr << "Erro ao abrir o ficheiro.\n";
        return;
    }

    float subdiv = length / divisions;
    float half = length / 2; // Metade do comprimento para centralizar na origem
    int totalVertices = (divisions + 1) * (divisions + 1) * 6;

	file << "BOX\n";
	file << "VERT " << totalVertices << "\n";		//número de vértices do cubo	
	file << "SLICES " << divisions << "\n";

    int i, j;
    float x, y, z;

    // Geração das 6 faces do cubo
    for (int face = 0; face < 6; ++face)
    {
        for (i = 0; i <= divisions; i++)
        {
            for (j = 0; j <= divisions; j++)
            {
                switch (face)
                {
                case 0: // Plano superior (y = half)
                    x = half - j * subdiv;
                    y = half;
                    z = half - i * subdiv;
                    break;
                case 1: // Plano inferior (y = -half)
                    x = half - j * subdiv;
                    y = -half;
                    z = -half + i * subdiv;
                    break;
                case 2: // Plano frontal (z = half)
                    x = half - j * subdiv;
                    y = -half + i * subdiv;
                    z = half;
                    break;
                case 3: // Plano traseiro (z = -half)
                    x = -half + j * subdiv;
                    y = -half + i * subdiv;
                    z = -half;
                    break;
                case 4: // Plano esquerdo (x = -half)
                    x = -half;
                    y = -half + i * subdiv;
                    z = half - j * subdiv;
                    break;
                case 5: // Plano direito (x = half)
                    x = half;
                    y = -half + i * subdiv;
                    z = -half + j * subdiv;
                    break;
                }
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

	// file << 1 << "\n";
	// file << (slices + 1) * (stacks + 1) << "\n";
	// file << slices << "\n";
	// file << stacks << "\n";

	file << "SPHERE\n";
	file << "VERT " << slices * (stacks - 1) + 2 << "\n";		//número de vértices da esfera
	file << "SLICES " << slices << "\n";
	file << "STACKS " << stacks << "\n";

	int i, j;
	float alpha, beta;
	float x, y, z;

	// Ponto centro base
	file << 0 << " " << -radius << " " << 0 << "\n";

	// Pontos meio

	for (i = 1; i < stacks; i++)
	{
		beta = (M_PI * i / stacks) + (M_PI / 2);

		for (j = 0; j < slices; j++)
		{
			alpha = 2 * M_PI * j / slices;

			x = radius * cos(beta) * sin(alpha);		//conversão para coordenadas polares
			y = -radius * sin(beta);
			z = radius * cos(beta) * cos(alpha);

			file << x << " " << y << " " << z << "\n";
		}
	}

	// Ponto centro topo

	file << 0 << " " << radius << " " << 0 << "\n";

	file.close();
}


int main(int argc, char* argv[])
{
	string line, command, arg1, arg2, arg3, arg4, arg5, arg6;

	

	while (true)
	{
		getline(cin, line);
		stringstream ss(line);
	
		ss >> command >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6;

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
				generateCone(stof(arg2),stof(arg3),stoi(arg4),stoi(arg5),arg6);
				cout << "Cone generated\n";
			}
			else if (arg1 == "sphere")
			{
				generateSphere(stof(arg2), stoi(arg3), stoi(arg4), arg5);
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
		else
		{
			cout << "Invalid command\n";
		}
	}

}