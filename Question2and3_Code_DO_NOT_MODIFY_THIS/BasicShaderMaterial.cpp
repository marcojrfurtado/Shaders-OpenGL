//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "BasicShaderMaterial.h"

BasicShaderMaterial::BasicShaderMaterial(){
	this->bAlphaBlended = false;
}

BasicShaderMaterial::BasicShaderMaterial(const char* vertexShaderFilename,const char* fragmentShaderFilename, 
										 bool alpha_blended){
											 this->init(vertexShaderFilename,fragmentShaderFilename,alpha_blended);
}

BasicShaderMaterial::~BasicShaderMaterial(){
	glDetachShader(this->shaderProgramID, this->fragmentShaderId);
	glDetachShader(this->shaderProgramID, this->vertexShaderId);

	glDeleteShader(this->fragmentShaderId);
	glDeleteShader(this->vertexShaderId);
	glDeleteProgram(this->shaderProgramID);
}

//initalize the shader with actual shader code
void BasicShaderMaterial::init(const char* vertexShaderFilename,const char* fragmentShaderFilename, bool alpha_blended){
	char* vertexShaderText   = this->readInShaderFile(vertexShaderFilename);
	char* fragmentShaderText = this->readInShaderFile(fragmentShaderFilename);
	this->initShadersFromStrings(vertexShaderText,fragmentShaderText,alpha_blended, 
		vertexShaderFilename,fragmentShaderFilename);

	if(vertexShaderText != NULL){
		free(vertexShaderText);
	}
	if(fragmentShaderText != NULL){
		free(fragmentShaderText);
	}

}

//init directly with shader code strings
void BasicShaderMaterial::initShadersFromStrings(const char* vertexShaderText,const char* fragmentShaderText, 
												 bool alpha_blended, const char* vertexShaderFilename,const char* fragmentShaderFilename)
{
	this->bAlphaBlended = alpha_blended;
	this->vertexShaderId = 0;
	this->fragmentShaderId=0;

	if(vertexShaderText == NULL){
		std::cout<<"WARNING! Unable to load Vertex shader! NULL was found instead of shader text!";
		return;
	}
	if(fragmentShaderText == NULL){
		std::cout<<"WARNING! Unable to load Fragment shader! NULL was found instead of shader text!";
		return;
	}

	this->vertexShaderId   = glCreateShader(GL_VERTEX_SHADER);
	this->fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(this->vertexShaderId, 1, &vertexShaderText, 0);
	glShaderSource(this->fragmentShaderId, 1, &fragmentShaderText, 0);

	glCompileShader(this->vertexShaderId);
	this->validateShader(this->vertexShaderId,vertexShaderFilename);
	glCompileShader(this->fragmentShaderId);
	this->validateShader(this->fragmentShaderId,fragmentShaderFilename);

	this->shaderProgramID = glCreateProgram();
	glAttachShader(this->shaderProgramID, this->fragmentShaderId);
	glAttachShader(this->shaderProgramID, this->vertexShaderId);
	glLinkProgram(this->shaderProgramID);
	this->validateProgram(this->shaderProgramID);
}

//Binds the shader for rendering
void BasicShaderMaterial::bindMaterial(){
	glUseProgram(this->shaderProgramID);
}

//Unbinds shader
void BasicShaderMaterial::unbindMaterial(){
	glUseProgram(0);
}

//Get the shader's internal ID number
unsigned int BasicShaderMaterial::getID(){
	return this->shaderProgramID;
}

//You might want to check to see if this material has been flagged as alphablended
bool BasicShaderMaterial::isAlphaBlended(){
	return this->bAlphaBlended;
}

char* BasicShaderMaterial::readInShaderFile(const char *fn) {

	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void BasicShaderMaterial::validateShader(GLuint shader, const char* file) {
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
	if (length > 0) {
		std::cout << "WARNING! Shader " << shader << " (" << (file?file:"") << ") compile error: " 
			<< buffer << "\n";
	}
}

void BasicShaderMaterial::validateProgram(GLuint program) {
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;

	memset(buffer, 0, BUFFER_SIZE);
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	if (length > 0)
		std::cout << "Program " << program << " link error: " << buffer << "\n";

	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
		std::cout << "Error validating shader " << program << "\n";
}