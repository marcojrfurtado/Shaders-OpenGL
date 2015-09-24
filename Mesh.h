//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol
#ifndef MESH_H
#define MESH_H

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class Mesh{

public: 
	std::string name;
	std::string mesh_filename;
	bool bShouldInvertTextureCoordinatesInVDirection;

private: 
	bool bMeshLoadedSuccessfully;

	int totalVertices;
	float* vertexPositionsBuffer;
	float* vertexTexCoordsBuffer;
	float* vertexNormalsBuffer;

public:

	Mesh(std::string mesh_filename);
	void drawMesh();

private:
	bool parseMeshFile(std::string filename);//Parses the relevant sections of the obj file
};

#endif