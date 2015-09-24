//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef TEXTURE_SHADER_MATERIAL_H
#define TEXTURE_SHADER_MATERIAL_H

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

class TextureShaderMaterial : public Material{

public:
	bool bAlphaBlended;

	TextureShaderMaterial();
	TextureShaderMaterial(const char* vertexShaderFilename,const char* fragmentShaderFilename,
		bool alpha_blended = false, bool use_time = false,
		unsigned int texture0=0,unsigned int texture1=0,unsigned int texture2=0,unsigned int texture3=0,
		unsigned int texture4=0,unsigned int texture5=0,unsigned int texture6=0,unsigned int texture7=0);
	~TextureShaderMaterial();

	//initalize the shader with actual shader code
	void init(const char* vertexShaderFilename,const char* fragmentShaderFilename,
		bool alpha_blended = false, bool use_time = false,
		unsigned int texture0=0,unsigned int texture1=0,unsigned int texture2=0,unsigned int texture3=0,
		unsigned int texture4=0,unsigned int texture5=0,unsigned int texture6=0,unsigned int texture7=0);

	//init directly with shader code strings
	void initShadersFromStrings(const char* vertexShaderText,const char* fragmentShaderText,
		bool alpha_blended = false, bool use_time = false,
		const char* vertexShaderFilename = 0,const char* fragmentShaderFilename = 0,
		unsigned int texture0=0,unsigned int texture1=0,unsigned int texture2=0,unsigned int texture3=0,
		unsigned int texture4=0,unsigned int texture5=0,unsigned int texture6=0,unsigned int texture7=0);
	
	//Binds the shader for rendering
	virtual void bindMaterial();
	
	//Unbinds shader
	virtual void unbindMaterial();

	//Get the shader's internal ID number
	virtual unsigned int getID();

	//You might want to check to see if this material has been flagged as alphablended
	virtual bool isAlphaBlended();

	//Checks to see if this material is set to use the time variable
	bool usesTimeVariable();

	//Updates the time of the shader
	void updateTime(float current_time);

	//Check for shader compile errors
	static void validateShader(GLuint shader, const char* file = 0);
	static void validateProgram(GLuint program);

	static char* readInShaderFile(const char* filename);

private:

	bool bUseTimeVariable;
	float currentTime;
	unsigned int texture_0;
	unsigned int texture_1;
	unsigned int texture_2;
	unsigned int texture_3;
	unsigned int texture_4;
	unsigned int texture_5;
	unsigned int texture_6;
	unsigned int texture_7;

	unsigned int shaderProgramID;
	unsigned int vertexShaderId;
	unsigned int fragmentShaderId;

	bool bUniformVariablesCacheNeedsBuilding;

	int cachedTimeVariableLocation;
	int cachedUniformLocationTexture0;
	int cachedUniformLocationTexture1;
	int cachedUniformLocationTexture2;
	int cachedUniformLocationTexture3;
	int cachedUniformLocationTexture4;
	int cachedUniformLocationTexture5;
	int cachedUniformLocationTexture6;
	int cachedUniformLocationTexture7;

	void buildUniformVariablesCache();
};

#endif
