//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef STATICMESH_H
#define STATICMESH_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Mesh.h"
#include "Material.h"

class StaticMesh
{
public:
//private:
	float x, y, z, xscale, yscale, zscale, pitch, yaw, roll;
	std::string name;
	Material* material;
	Mesh* mesh;

public:

	StaticMesh(std::string Name,Mesh* theMesh,Material* theMaterial = NULL,
		float X=0.0f,float Y=0.0f,float Z=0.0f,
		float Xscale=1.0f,float Yscale=1.0f,float Zscale=1.0f,
		float Pitch=0.0f,float Yaw=0.0f,float Roll=0.0f);
	StaticMesh(Mesh* theMesh=NULL,Material* theMaterial = NULL,
		float X=0.0f,float Y=0.0f,float Z=0.0f,
		float Xscale=1.0f,float Yscale=1.0f,float Zscale=1.0f,
		float Pitch=0.0f,float Yaw=0.0f,float Roll=0.0f);
	void draw(bool renderOnlyIfOpaque = false, bool renderOnlyIfTranslucent =  false);
	std::string getName();
	~StaticMesh();
};

#endif
