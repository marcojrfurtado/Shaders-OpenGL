//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "StaticMesh.h"

StaticMesh::StaticMesh(std::string meshName, Mesh* theMesh,Material* theMaterial,
					   float X,float Y,float Z,float Xscale,float Yscale,float Zscale,float Pitch,float Yaw,float Roll)
{
	this->name = meshName;
	this->x = X; this->y = Y; this->z = Z;
	this->xscale = Xscale; this->yscale = Yscale; this->zscale = Zscale;
	this->pitch = Pitch; this->yaw = Yaw; this->roll = Roll;
	this->mesh = theMesh;
	this->material = theMaterial;
}

StaticMesh::StaticMesh(Mesh* theMesh,Material* theMaterial,
					   float X,float Y,float Z,float Xscale,float Yscale,float Zscale,float Pitch,float Yaw,float Roll)
{
	this->name = "untitled";
	this->x = X; this->y = Y; this->z = Z;
	this->xscale = Xscale; this->yscale = Yscale; this->zscale = Zscale;
	this->pitch = Pitch; this->yaw = Yaw; this->roll = Roll;
	this->mesh = theMesh;
	this->material = theMaterial;
}

void StaticMesh::draw(bool renderOnlyIfOpaque, bool renderOnlyIfTranslucent)
{
	if(this->material != NULL){
		if(this->material->isAlphaBlended() && renderOnlyIfOpaque){
			return;//i.e. if this is an opaque-objects-only pass, and the material is alpha blended, we should exit
		}
		if(!this->material->isAlphaBlended() && renderOnlyIfTranslucent){
			return;
			//i.e. if this is an alpha-blended-objects-only pass, and this material is not alpha blended, it's time to leave
		}
	}

	if(this->mesh == NULL){
		std::cout<<"WARNING! Static mesh \""<<name<<"\" has a NULL mesh!\n";
		return;
	}

	glPushMatrix();
		glTranslatef(this->x,this->y,this->z);
		glRotatef(this->yaw,     0.0f,0.0f,1.0f);
		glRotatef(-(this->pitch),0.0f,1.0f,0.0f);
		glRotatef(this->roll,    1.0f,0.0f,0.0f);
		glScalef(this->xscale, this->yscale, this->zscale);
		if(this->material){
			this->material->bindMaterial();
		}
		
		this->mesh->drawMesh();
		
		if(this->material){
			this->material->unbindMaterial();
		}
	glPopMatrix();
}

std::string StaticMesh::getName(){return name;}

StaticMesh::~StaticMesh()
{
	//delete mesh;
}
