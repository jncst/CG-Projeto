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

    file << "1\n";  // Identificador do tipo de objeto (por exemplo, 1 para cone)
    file << (slices + 1) * stacks + 1 << "\n";  // Número total de vértices
    file << slices << "\n";
    file << stacks << "\n";

    float angleStep = 2 * M_PI / slices;
    float stackHeight = height / stacks;
    float stackRadiusStep = radius / stacks;

    // Gerar os vértices das stacks (camada por camada)
    for (int i = 0; i <= stacks; i++) 
    {
        float r = radius - (i * stackRadiusStep);  // Raio da camada atual
        float y = i * stackHeight;  // Mantemos Y como altura

        if (y == height)
        {
            file << "0 " << y << " 0\n";  // Vértice no topo do cone
            break;
        }

        // **Gerar os pontos da base em sentido horário (CW)**
        for (int j = slices - 1; j >= 0; j--) 
        {
            float theta = j * angleStep;

            float x = r * cos(theta);  // Posição X no círculo
            float z = r * sin(theta);  // Posição Z no círculo

            file << x << " " << y << " " << z << "\n";
        }

        file << "0 0 0\n";  // Centro da base
    }

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

	// Não sei o que ser este um
	//file << 1 << "\n";
	file << "PLANE\n";
	file << "VERT "<<(divisions + 1) * (divisions + 1) << "\n";		//número de vértices no plano
	// Tmb nao sei o que este 0
	//file << 0 << "\n";		
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
    int totalVertices = (divisions + 1) * (divisions + 1);

	file << 6 << "\n";
    file << totalVertices << "\n";
	file << 0 << "\n";
    file << divisions << "\n";

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
                    z = half - i * subdiv;
                    break;
                case 2: // Plano frontal (z = half)
                    x = half - j * subdiv;
                    y = half - i * subdiv;
                    z = half;
                    break;
                case 3: // Plano traseiro (z = -half)
                    x = half - j * subdiv;
                    y = half - i * subdiv;
                    z = -half;
                    break;
                case 4: // Plano esquerdo (x = -half)
                    x = -half;
                    y = half - i * subdiv;
                    z = half - j * subdiv;
                    break;
                case 5: // Plano direito (x = half)
                    x = half;
                    y = half - i * subdiv;
                    z = half - j * subdiv;
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

	file << 1 << "\n";
	file << (slices + 1) * (stacks + 1) << "\n";
	file << slices << "\n";
	file << stacks << "\n";

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