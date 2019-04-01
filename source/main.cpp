#include "../include/FEAModeller.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>

int main()
{
	auto start = std::chrono::high_resolution_clock::now();

	FEAModeller app;
	app.CreateHexas();
	app.ProcessMaterials();
	app.ExportINP();
	//app.ExportTXT();
	//app.Render(0, 1);

	std::string fileName = "log_" + app.GetFileName() + ".txt";
	std::ofstream file;
	file.open(fileName.c_str(), std::ios_base::app);

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << "Complete Elapsed Time: " << elapsedTime - app.GetFileReadingElapsedTime() << std::endl;
	file << std::setw(30) << app.GetProblemSize() << ", " << std::setw(30) << elapsedTime - app.GetFileReadingElapsedTime() << std::endl;

}