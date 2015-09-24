//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol
#ifndef MATERIAL_H
#define MATERIAL_H

//Common interface for all different possible material types (i.e. textures, shaders, etc)
class Material{
public:
	//You should bind a material before rendering
	virtual void bindMaterial() = 0;
	//You should unbind a material after rendering
	virtual void unbindMaterial() = 0;

	//Get the materials OpengGL id data (i.e. like its texture or shader ID)
	virtual unsigned int getID() = 0;

	//You might want to check to see if this material has been flagged as alphablended
	virtual bool isAlphaBlended() = 0;
};

#endif