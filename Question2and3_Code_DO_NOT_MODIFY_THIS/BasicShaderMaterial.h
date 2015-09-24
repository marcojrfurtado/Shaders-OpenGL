//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef BASIC_SHADER_MATERIAL_H
#define BASIC_SHADER_MATERIAL_H

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Material.h"

class BasicShaderMaterial : public Material{

public:
	bool bAlphaBlended;

	BasicShaderMaterial();
	BasicShaderMaterial(const char* vertexShaderFilename,const char* fragmentShaderFilename,bool alpha_blended = false);
	~BasicShaderMaterial();

	//initalize the shader with actual shader code
	void init(const char* vertexShaderFilename,const char* fragmentShaderFilename,bool alpha_blended = false);

	//init directly with shader code strings
	void initShadersFromStrings(const char* vertexShaderText,const char* fragmentShaderText,
		bool alpha_blended = false,
		const char* vertexShaderFilename = 0,const char* fragmentShaderFilename = 0);

	//Binds the shader for rendering
	virtual void bindMaterial();
	
	//Unbinds shader
	virtual void unbindMaterial();

	//Get the shader's internal ID number
	virtual unsigned int getID();

	//You might want to check to see if this material has been flagged as alphablended
	virtual bool isAlphaBlended();

	//Check for shader compile errors
	static void validateShader(GLuint shader, const char* file = 0);
	static void validateProgram(GLuint program);

	static char* readInShaderFile(const char* filename);

private:

	unsigned int shaderProgramID;
	unsigned int vertexShaderId;
	unsigned int fragmentShaderId;
};

#endif
