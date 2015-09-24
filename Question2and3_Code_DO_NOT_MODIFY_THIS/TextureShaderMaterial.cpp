//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "TextureShaderMaterial.h"

TextureShaderMaterial::TextureShaderMaterial(){
	this->bAlphaBlended = false;
}

TextureShaderMaterial::TextureShaderMaterial(const char* vertexShaderFilename,const char* fragmentShaderFilename, 
							bool alpha_blended, bool use_time,
							unsigned int texture0,unsigned int texture1,unsigned int texture2,unsigned int texture3,
							unsigned int texture4,unsigned int texture5,unsigned int texture6,unsigned int texture7)
{
	 this->init(vertexShaderFilename,fragmentShaderFilename,alpha_blended,use_time,
		 texture0,texture1,texture2,texture3,texture4,texture5,texture6,texture7);
}

TextureShaderMaterial::~TextureShaderMaterial(){
	glDetachShader(this->shaderProgramID, this->fragmentShaderId);
	glDetachShader(this->shaderProgramID, this->vertexShaderId);

	glDeleteShader(this->fragmentShaderId);
	glDeleteShader(this->vertexShaderId);
	glDeleteProgram(this->shaderProgramID);
}

//initalize the shader with actual shader code
void TextureShaderMaterial::init(const char* vertexShaderFilename,const char* fragmentShaderFilename, 
								 bool alpha_blended, bool use_time,
								unsigned int texture0,unsigned int texture1,unsigned int texture2,unsigned int texture3,
								unsigned int texture4,unsigned int texture5,unsigned int texture6,unsigned int texture7)
{
	char* vertexShaderText   = this->readInShaderFile(vertexShaderFilename);
	char* fragmentShaderText = this->readInShaderFile(fragmentShaderFilename);
	this->initShadersFromStrings(vertexShaderText,fragmentShaderText,alpha_blended,use_time,
		vertexShaderFilename,fragmentShaderFilename,
		texture0,texture1,texture2,texture3,texture4,texture5,texture6,texture7);

	if(vertexShaderText != NULL){
		free(vertexShaderText);
	}
	if(fragmentShaderText != NULL){
		free(fragmentShaderText);
	}

}

//init directly with shader code strings
void TextureShaderMaterial::initShadersFromStrings(const char* vertexShaderText,const char* fragmentShaderText, 
			bool alpha_blended, bool use_time, const char* vertexShaderFilename,const char* fragmentShaderFilename,
			unsigned int texture0,unsigned int texture1,unsigned int texture2,unsigned int texture3,
			unsigned int texture4,unsigned int texture5,unsigned int texture6,unsigned int texture7)
{
	this->bAlphaBlended = alpha_blended;
	this->vertexShaderId = 0;
	this->fragmentShaderId=0;

	this->currentTime = 0;
	this->bUseTimeVariable = use_time;
	
	this->texture_0 = texture0;
	this->texture_1 = texture1;
	this->texture_2 = texture2;
	this->texture_3 = texture3;
	this->texture_4 = texture4;
	this->texture_5 = texture5;
	this->texture_6 = texture6;
	this->texture_7 = texture7;
	
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

	this->bUniformVariablesCacheNeedsBuilding = true;
}

//Binds the shader for rendering
void TextureShaderMaterial::bindMaterial(){
	glUseProgram(this->shaderProgramID);

	if(this->bUniformVariablesCacheNeedsBuilding){
		this->buildUniformVariablesCache();
	}

	if(this->bUseTimeVariable && this->cachedTimeVariableLocation != -1){
		glUniform1f(this->cachedTimeVariableLocation, this->currentTime);
	}

	if(this->texture_0 != 0){
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(this->cachedUniformLocationTexture0, 0);
		glBindTexture(GL_TEXTURE_2D, this->texture_0);
	}
	
	if(this->texture_1 != 0){
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(this->cachedUniformLocationTexture1, 1);
		glBindTexture(GL_TEXTURE_2D, this->texture_1);
	}
	
	if(this->texture_2 != 0){
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(this->cachedUniformLocationTexture2, 2);
		glBindTexture(GL_TEXTURE_2D, this->texture_2);
	}
	
	if(this->texture_3 != 0){
		glActiveTexture(GL_TEXTURE3);
		glUniform1i(this->cachedUniformLocationTexture3, 3);
		glBindTexture(GL_TEXTURE_2D, this->texture_3);
	}
	
	if(this->texture_4 != 0){
		glActiveTexture(GL_TEXTURE4);
		glUniform1i(this->cachedUniformLocationTexture4, 4);
		glBindTexture(GL_TEXTURE_2D, this->texture_4);
	}
	
	if(this->texture_5 != 0){
		glActiveTexture(GL_TEXTURE5);
		glUniform1i(this->cachedUniformLocationTexture5, 5);
		glBindTexture(GL_TEXTURE_2D, this->texture_5);
	}
	
	if(this->texture_6 != 0){
		glActiveTexture(GL_TEXTURE6);
		glUniform1i(this->cachedUniformLocationTexture6, 6);
		glBindTexture(GL_TEXTURE_2D, this->texture_6);
	}
	
	if(this->texture_7 != 0){
		glActiveTexture(GL_TEXTURE7);
		glUniform1i(this->cachedUniformLocationTexture7, 7);
		glBindTexture(GL_TEXTURE_2D, this->texture_7);
	}
	
	glActiveTexture(GL_TEXTURE0);
	
	
}

void TextureShaderMaterial::buildUniformVariablesCache(){
	
	this->cachedTimeVariableLocation = -1;
	this->cachedUniformLocationTexture0 =-1;
	this->cachedUniformLocationTexture1 =-1;
	this->cachedUniformLocationTexture2 =-1;
	this->cachedUniformLocationTexture3 =-1;
	this->cachedUniformLocationTexture4 =-1;
	this->cachedUniformLocationTexture5 =-1;
	this->cachedUniformLocationTexture6 =-1;
	this->cachedUniformLocationTexture7 =-1;
	
	if(this->bUseTimeVariable){
		this->cachedTimeVariableLocation   =  glGetUniformLocation(this->shaderProgramID, "currentTime");
		if(this->cachedTimeVariableLocation == -1){
			std::cout<<"WARNING! \"currentTime\" not found in shader, but this material is set to use it!\n";
		}
	}

	if(this->texture_0 != 0){
		this->cachedUniformLocationTexture0 = glGetUniformLocation(this->shaderProgramID, "textureSample_0");
	}
	if(this->texture_1 != 0){
		this->cachedUniformLocationTexture1 = glGetUniformLocation(this->shaderProgramID, "textureSample_1");
	}
	if(this->texture_2 != 0){
		this->cachedUniformLocationTexture2 = glGetUniformLocation(this->shaderProgramID, "textureSample_2");
	}
	if(this->texture_3 != 0){
		this->cachedUniformLocationTexture3 = glGetUniformLocation(this->shaderProgramID, "textureSample_3");
	}
	if(this->texture_4 != 0){
		this->cachedUniformLocationTexture4 = glGetUniformLocation(this->shaderProgramID, "textureSample_4");
	}
	if(this->texture_5 != 0){
		this->cachedUniformLocationTexture5 = glGetUniformLocation(this->shaderProgramID, "textureSample_5");
	}
	if(this->texture_6 != 0){
		this->cachedUniformLocationTexture6 = glGetUniformLocation(this->shaderProgramID, "textureSample_6");
	}
	if(this->texture_7 != 0){
		this->cachedUniformLocationTexture7 = glGetUniformLocation(this->shaderProgramID, "textureSample_7");
	}
	this->bUniformVariablesCacheNeedsBuilding = false;
}

//Unbinds shader
void TextureShaderMaterial::unbindMaterial(){
	glUseProgram(0);
}

//Get the shader's internal ID number
unsigned int TextureShaderMaterial::getID(){
	return this->shaderProgramID;
}

//You might want to check to see if this material has been flagged as alphablended
bool TextureShaderMaterial::isAlphaBlended(){
	return this->bAlphaBlended;
}

	//Checks to see if this material is set to use the time variable
bool TextureShaderMaterial::usesTimeVariable(){
	return this->bUseTimeVariable;
}

	//Updates the time of the shader
void TextureShaderMaterial::updateTime(float current_time){
	this->currentTime = current_time;
}

char* TextureShaderMaterial::readInShaderFile(const char *fn) {

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

void TextureShaderMaterial::validateShader(GLuint shader, const char* file) {
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

void TextureShaderMaterial::validateProgram(GLuint program) {
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
