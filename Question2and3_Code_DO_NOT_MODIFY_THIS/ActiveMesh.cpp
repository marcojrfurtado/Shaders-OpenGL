//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "ActiveMesh.h"

using namespace std;

ActiveMesh::ActiveMesh()
{
	mesh = NULL;
	name = "untitled";
	x= y= z= pitch= yaw= roll= 0.0f;
	xscale= yscale= zscale = 1.0f;
}

ActiveMesh::ActiveMesh(string Name, Mesh* theMesh ,float X,float Y,float Z,float Xscale,float Yscale,float Zscale,float Pitch,float Yaw,float Roll)
{
	name = Name;
	x = X; y = Y; z = Z;
	xscale = Xscale; yscale = Yscale; zscale = Zscale;
	pitch = Pitch; yaw = Yaw; roll = Roll;
	mesh = theMesh;
}
