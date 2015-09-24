//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include "Mesh.h"

Mesh::Mesh(std::string mesh_filename){
	this->name = mesh_filename;
	this->mesh_filename = mesh_filename;
	this->bShouldInvertTextureCoordinatesInVDirection = false;

	this->bMeshLoadedSuccessfully = this->parseMeshFile(mesh_filename);
}

void Mesh::drawMesh(){
	if(!this->bMeshLoadedSuccessfully)
		return;
	/*
	glBegin(GL_TRIANGLES);
	for(int i = 0; i<this->totalVertices; i++){
		glVertex3f(
			this->vertexPositionsBuffer[i*3+0],
			this->vertexPositionsBuffer[i*3+1],
			this->vertexPositionsBuffer[i*3+2]);
	}
	glEnd();
	*/
	
	//Enable vertex buffer rendering
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//Load all the buffers into memory
	glVertexPointer(3, GL_FLOAT, 0, this->vertexPositionsBuffer);
	glTexCoordPointer(2, GL_FLOAT, 0, this->vertexTexCoordsBuffer);
	glNormalPointer(GL_FLOAT, 0,this->vertexNormalsBuffer);

	//Actually draw all the stuff
	glDrawArrays(GL_TRIANGLES, 0, this->totalVertices);

	//Disable vertex buffer rendering for now

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
}

void replaceSlahesWithSpaces(std::string& str){
	for(unsigned int i = 0; i<str.size(); i++){
		if(str[i] == '/'){
			str[i] = ' ';
		}
	}
}

//Parses the relevant sections of the obj file
bool Mesh::parseMeshFile(std::string filename){

	std::ifstream fin;
	fin.open(mesh_filename.c_str(),std::ios::in);

	if(!fin){
		std::cout<<"WARNING! Mesh file \""<<filename<<"\" could not be found!\n";
		fin.close();
		return false;
	}

	std::vector<float*> positionList;
	std::vector<float*> texCoordList;
	std::vector<float*> normalsList;
	std::vector<int*>   faceList;

	//boolean bPositionVertsPresent = false;
	//boolean bTextureCoordsPresent = false;
	//boolean bNormalsPresent = false;

	std::string line;

	while(!fin.eof()){

		getline(fin, line);

		if(line == "" || line == "\n"){//i.e. this is a blank line
			//std::cout<<"Blank Line\n";
			continue;
		}

		//std::cout<<"Line: \""+line+"\"  <-";
		std::istringstream sin(line);
		std::string firstToken;
		sin>>firstToken;

		if(firstToken[0] == '#'){//i.e. check for a "comment" line
			//std::cout<<"Comment Line\n";
			continue;
		}else if(firstToken == "v"){
			//std::cout<<"Vertex Position Data\n";
			float* pos = new float[3];
			sin>>pos[0];
			sin>>pos[1];
			sin>>pos[2];
			positionList.push_back(pos);
		}else if(firstToken == "vt"){
			//std::cout<<"Vertex Texture Coord Data\n";
			float* txr = new float[2];
			sin>>txr[0];
			sin>>txr[1];
			texCoordList.push_back(txr);
		}else if(firstToken == "vn"){
			//std::cout<<"Vertex Normal Data\n";
			float* nrm = new float[3];
			sin>>nrm[0];
			sin>>nrm[1];
			sin>>nrm[2];
			normalsList.push_back(nrm);
		}else if(firstToken == "f"){
			//std::cout<<"Face Data\n";

			std::string vertex1String;
			std::string vertex2String;
			std::string vertex3String;
			sin>>vertex1String;
			sin>>vertex2String;
			sin>>vertex3String;
			replaceSlahesWithSpaces(vertex1String);
			replaceSlahesWithSpaces(vertex2String);
			replaceSlahesWithSpaces(vertex3String);
			std::istringstream vertex1in(vertex1String);
			std::istringstream vertex2in(vertex2String);
			std::istringstream vertex3in(vertex3String);

			int* indices = new int[9];

			vertex1in>>indices[0];
			vertex1in>>indices[1];
			vertex1in>>indices[2];

			vertex2in>>indices[3];
			vertex2in>>indices[4];
			vertex2in>>indices[5];

			vertex3in>>indices[6];
			vertex3in>>indices[7];
			vertex3in>>indices[8];

			faceList.push_back(indices);

		}else{
			//std::cout<<"WARNING! Unknown line!\n";
		}
	}


	fin.close();
	//Finally, we can use the parsed data to build the final vertex arrays

	//i.e. 3 verts per face, 3 floats per vert
	this->totalVertices = faceList.size()*3;

	float* vertexPositions = new float[faceList.size()*3*3];
	float* vertexTexCoords = new float[faceList.size()*3*2];
	float* vertexNormals   = new float[faceList.size()*3*3];

	for(unsigned int i = 0;i<faceList.size();i++ ){//i = face number
		int* tempFace = faceList[i];
		for(int j = 0; j<3;j++){//j = vertex number

			//need to subtract 1 from all the index values because
			//obj files start their indices at 1
			float* positionXYZ = positionList[(tempFace[j*3+0]-1)];
			float* texCoordUV  = texCoordList[(tempFace[j*3+1]-1)];
			float* normalXYZ   =  normalsList[(tempFace[j*3+2]-1)];

			vertexPositions[i*9+j*3+0] = positionXYZ[0];
			vertexPositions[i*9+j*3+1] = positionXYZ[1];
			vertexPositions[i*9+j*3+2] = positionXYZ[2];

			vertexTexCoords[i*6+j*2+0] = texCoordUV[0];
			if(this->bShouldInvertTextureCoordinatesInVDirection)
				vertexTexCoords[i*6+j*2+1] = 1.0f-texCoordUV[1];
			else
				vertexTexCoords[i*6+j*2+1] = texCoordUV[1];

			vertexNormals[i*9+j*3+0] = normalXYZ[0];
			vertexNormals[i*9+j*3+1] = normalXYZ[1];
			vertexNormals[i*9+j*3+2] = normalXYZ[2];
		}
	}

	this->vertexPositionsBuffer = vertexPositions;
	this->vertexTexCoordsBuffer = vertexTexCoords;
	this->vertexNormalsBuffer   = vertexNormals;

	/*
	for(int i = 0; i<this->totalVertices; i++){
		std::cout<<"Vertex "<<i<<":"
			<<this->vertexPositionsBuffer[i*3+0]
			<<" "<<this->vertexPositionsBuffer[i*3+1]
			<<" "<<this->vertexPositionsBuffer[i*3+2]<<"\n";

	}
	std::cout<<"Total vertices: "<<this->totalVertices<<"\n";
	*/
	return true;
}


