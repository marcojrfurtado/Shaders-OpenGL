//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "ColouredTextureMaterial.h"

ColouredTextureMaterial::ColouredTextureMaterial(Texture* tex, float red, float green, float blue, float alpha){
	this->texture = tex;
	this->r = red;
	this->g = green;
	this->b = blue;
	this->a = alpha;
}

void ColouredTextureMaterial::bindMaterial() {
	//TEXTURE BINDING
	if(this->texture == NULL){
		//glDisable(GL_TEXTURE_2D);
		glColor4f(this->r,this->g,this->b,this->a);
		//glBindTexture(GL_TEXTURE_2D, 0);//had to remove this command, it resulted in massive performance loss
	}else{
		//glEnable(GL_TEXTURE_2D);
		glColor4f(this->r,this->g,this->b,this->a);
		this->texture->bindTexture();
	}
}

void ColouredTextureMaterial::unbindMaterial(){
	return;
	if(this->texture != NULL){
		this->texture->unbindTexture();
	}
}

unsigned int ColouredTextureMaterial::getID(){
	if(this->texture != NULL){
		return this->texture->getTextureID();
	}
	return 0;
}

bool ColouredTextureMaterial::isAlphaBlended(){
	if(this->a<0.999f || (this->texture != NULL && this->texture->isAlphaBlended())){
		return true;
	}
	return false;
}