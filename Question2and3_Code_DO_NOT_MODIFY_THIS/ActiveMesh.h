//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef ACTIVEMESH_H
#define ACTIVEMESH_H

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

using namespace std;

class ActiveMesh
{
private:
	Mesh* mesh;
	float x, y, z, xscale, yscale, zscale, pitch, yaw, roll;
	float deltax, deltay, deltaxscale, deltayscale, deltazscale, deltapitch, deltayaw, deltaroll;
	float xinit, yinit, zinit, xscaleinit, yscaleinit, zscaleinit, pitchinit, yawinit, rollinit;
	string name;

public:
	ActiveMesh();
	ActiveMesh(string name, Mesh* theMesh, float X=0.0f,float Y=0.0f,float Z=0.0f,float Xscale=1.0f,float Yscale=1.0f,float Zscale=1.0f,float Pitch=0.0f,float Yaw=0.0f,float Roll=0.0f);
	void draw();
	void update();
	void updateMovement();
	string getName();
};

#endif
