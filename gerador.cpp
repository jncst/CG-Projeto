#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>

using namespace std;

void generatePlane(float length, int divisions, const string& filename)
{
	ofstream file(filename);		//abre o ficheiro para escrita
	if (!file)
	{
		cerr << "Erro ao abrir o ficheiro.\n";
		return;
	}

	float subdiv = length / divisions;
	float half = length / 2;		//usado para depois podermos centrar na origem

	file << (divisions + 1) * (divisions + 1) << "\n";		//número de vértices no plano

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
	ofstream file(filename);		//abre o ficheiro para escrita
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

				file << x << y << z << "\n";
			}
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
		}
		else
		{
			cout << "Invalid command\n";
		}
	}

}