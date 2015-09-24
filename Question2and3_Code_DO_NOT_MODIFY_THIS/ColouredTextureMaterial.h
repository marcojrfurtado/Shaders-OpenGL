//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol
#ifndef COLOURED_TEXTURE_MATERIAL_H
#define COLOURED_TEXTURE_MATERIAL_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Material.h"
#include "Texture.h"

class ColouredTextureMaterial : public Material{

public:
	float r;
	float g;
	float b;
	float a;
	Texture* texture;

	ColouredTextureMaterial(Texture* tex = NULL, float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f);
	virtual void bindMaterial();
	virtual void unbindMaterial();
	virtual unsigned int getID();
	virtual bool isAlphaBlended();
	
};

#endif