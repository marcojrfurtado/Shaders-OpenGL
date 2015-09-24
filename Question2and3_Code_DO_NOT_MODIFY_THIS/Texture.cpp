//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol
//Note: much of this code was based off NeHe's Lesson 33

#include "Texture.h"

Texture::Texture(const char * filename, bool texture_is_alpha_blended, bool clamp_u, bool clamp_v){
		this->textureFilename = (char *)filename;
		this->bTextureLoaded = false;
		this->bTextureNeedsToBeStreamedIn = true;
		this->bTextureIsAlphaBlended = texture_is_alpha_blended;

		this->bClampedU = clamp_u;
		this->bClapmedV = clamp_v;
}

bool Texture::loadTexture(){
	this->bTextureNeedsToBeStreamedIn = false;
	
	this->bTextureLoaded= this->loadTGATextureFile(this->textureFilename);
	
	return this->bTextureLoaded;
}

void Texture::bindTexture(){
	if(this->bTextureNeedsToBeStreamedIn){
		this->loadTexture();
	}
	if(this->bTextureLoaded){
		//glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, this->texture);
	}
}

void Texture::unbindTexture(){
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getTextureID(){
	if(this->bTextureNeedsToBeStreamedIn){
		this->loadTexture();
	}
	if(this->bTextureLoaded){
		return this->texture;
	}
	return 0;
}

bool Texture::isAlphaBlended(){
	return this->bTextureIsAlphaBlended;
}


bool Texture::loadTGATextureFile(char * filename) // Loads a TGA file
{
	FILE * fTGA;// File pointer to texture file
	//errno_t error_code = fopen_s(&fTGA,filename, "rb");
	//Old "unsafe" code:  
	fTGA = fopen(filename, "rb");

	//if(error_code != 0 || fTGA == NULL){
	if(fTGA == NULL){
		cerr<<"Could not open texture file!\n";
		return false;
	}

	TGAImageData *pBitMap = loadTGAImageData(fTGA);

	if(pBitMap == NULL){
		std::cout<<"WARNING: Loading TGA file failed! The Bitmap was NULL! Memory may have been leaked!\n";
		return false;
	}

	glGenTextures(1, &(this->texture));
	glBindTexture(GL_TEXTURE_2D, this->texture);
	int textureType = GL_RGB;
	if(pBitMap->channels == 4){	
		textureType = GL_RGBA;
	}
	gluBuild2DMipmaps(GL_TEXTURE_2D, pBitMap->channels, pBitMap->size_x, pBitMap->size_y, textureType, GL_UNSIGNED_BYTE, pBitMap->data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	if(this->bClampedU){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	}
	if(this->bClapmedV){
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	}

	if (pBitMap){
		if (pBitMap->data){
			free(pBitMap->data);
		}
		free(pBitMap);				
	}
	return true;
}

Texture::TGAImageData* Texture::loadTGAImageData(FILE* tgaFile)
{	
	FILE *pFile			= tgaFile;	
	TGAImageData *pImgData	= NULL;	
	WORD width			= 0;
	WORD height			= 0;			
	byte length			= 0;					
	byte imgType		= 0;					
	byte bits			= 0;						
	int channels		= 0;					
	int stride			= 0;						
	int i				= 0;							

	pImgData = (TGAImageData*)malloc(sizeof(TGAImageData));
	
	fread(&length, sizeof(byte), 1, pFile);
	
	fseek(pFile,1,SEEK_CUR); 
	
	fread(&imgType, sizeof(byte), 1, pFile);
	
	fseek(pFile, 9, SEEK_CUR); 
	
	fread(&width,  sizeof(WORD), 1, pFile);
	fread(&height, sizeof(WORD), 1, pFile);
	fread(&bits,   sizeof(byte), 1, pFile);
	
	fseek(pFile, length + 1, SEEK_CUR); 
	
	if(imgType != TGA_RLE)
	{
		// Check for 24 or 32 Bit
		if(bits == 24 || bits == 32)
		{
			
			channels = bits / 8;
			stride = channels * width;
			pImgData->data = new unsigned char[stride * height];
			
			for(int y = 0; y < height; y++)
			{
				unsigned char *pLine = &(pImgData->data[stride * y]);
				
				fread(pLine, stride, 1, pFile);
				
				for(i = 0; i < stride; i += channels)
				{
					int temp     = pLine[i];
					pLine[i]     = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
				
		// Check for 16 Bit
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;
			
			channels = 3;
			stride = channels * width;
			pImgData->data = new unsigned char[stride * height];
			
			for(int i = 0; i < width*height; i++)
			{
				fread(&pixels, sizeof(unsigned short), 1, pFile);
				
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;
				
				pImgData->data[i * 3 + 0] = r;
				pImgData->data[i * 3 + 1] = g;
				pImgData->data[i * 3 + 2] = b;
			}
		}	
		
		else
			return NULL;
	}
	
	else
	{
		
		byte rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * width;
		
		pImgData->data = new unsigned char[stride * height];
		byte *pColors = new byte [channels];
		
		while(i < width*height)
		{
			
			fread(&rleID, sizeof(byte), 1, pFile);
			
			
			if(rleID < 128)
			{
				rleID++;
				
				while(rleID)
				{
					fread(pColors, sizeof(byte) * channels, 1, pFile);
					
					pImgData->data[colorsRead + 0] = pColors[2];
					pImgData->data[colorsRead + 1] = pColors[1];
					pImgData->data[colorsRead + 2] = pColors[0];
					
					if(bits == 32)	pImgData->data[colorsRead + 3] = pColors[3];
					
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			
			else
			{
				rleID -= 127;
				
				fread(pColors, sizeof(byte) * channels, 1, pFile);
				
				while(rleID)
				{
					pImgData->data[colorsRead + 0] = pColors[2];
					pImgData->data[colorsRead + 1] = pColors[1];
					pImgData->data[colorsRead + 2] = pColors[0];
					
					if(bits == 32)	pImgData->data[colorsRead + 3] = pColors[3];
					
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
		}
		delete[] pColors;
	}
	
	fclose(pFile);
	
	
	pImgData->channels  = channels;
	pImgData->size_x    = width;
	pImgData->size_y    = height;
	
	return pImgData;
}



/*

bool Texture::loadUncompressedTGA(char * filename, FILE * fTGA)
{
	int size = sizeof(tga.header);
	int garbage = fread(tga.header, sizeof(tga.header), 1, fTGA);
	if(garbage == 0){// Attempt to read header
	//if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0){
		cerr<<"Could not read info header!\n";
		if(fTGA != NULL){
			fclose(fTGA);
		}
		return false;
	}

	width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	bpp	= tga.header[4];										// Determine the bits per pixel
	tga.Width		= width;										// Copy width into local structure						
	tga.Height		= height;										// Copy height into local structure
	tga.Bpp			= bpp;											// Copy BPP into local structure

	if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp !=32))){	// Make sure all information is valid
		cerr<<"Invalid texture information. You fail it!\n";	// Display Error
		if(fTGA != NULL){
			fclose(fTGA);
		}
		return false;
	}

	if(bpp == 24)// If the BPP of the image is 24...
		type	= GL_RGB;// Set Image type to GL_RGB
	else{						// Else if its 32 BPP
		type	= GL_RGBA;//Set image type to GL_RGBA
		cout<<"Alpha channel detected in \""<<filename<<"\"!"<<endl;
	}
	tga.bytesPerPixel	= (tga.Bpp / 8);							// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);// Compute the total amout ofmemory needed to store data
	imageData	= (GLubyte *)malloc(tga.imageSize);			// Allocate that much memory

	if(imageData == NULL){
		cerr<<"Could not allocate memory for image :O\n";	// Display Error
		fclose(fTGA);										// Close the file
		return false;										// Return failed
	}
	
	char garbageByte = 0;
	for(int i = 0; i<0+1; i++)
		fread(&garbageByte, 1, 1, fTGA);//Eats the extra phase-shift
	
	//cout<<"The image size is: "<<tga.imageSize<<endl;

	if(fread(imageData, 1, tga.imageSize, fTGA) != tga.imageSize){	// Attempt to read image data
		cerr<<"Error: The correct amount of data was not read in!\n";		// Display Error
		if(imageData != NULL){										// If imagedata has data in it
			free(imageData);										// Delete data from memory
		}
		fclose(fTGA);														// Close file
		return false;														// Return failed
	}

	// Now, the bytes are not in the order OpenGL wants them in, but a little XOR can fix that...
	// Byte Swapping Optimized Algorithm based off of Steve Thomas' algorithm (NeHe)
	
	for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel){
		imageData[cswap] ^= imageData[cswap+2] ^=
		imageData[cswap] ^= imageData[cswap+2];
	}

	fclose(fTGA);//done with file
	return true;//success!
}

bool Texture::loadCompressedTGA(char * filename, FILE * fTGA)		// Load COMPRESSED TGAs
{ 
	int size = sizeof(tga.header);
	int garbage = fread(tga.header, sizeof(tga.header), 1, fTGA);
	cout<<"WARNING: The image \""<<filename<<"\" is compressed and may not load properly!"<<endl;
	
	if(garbage == 0){// Attempt to read header
		cerr<<"Could not read info header for compressed TGA!\n";
		if(fTGA != NULL){
			fclose(fTGA);
		}
		return false;
	}

	width  = tga.header[1] * 256 + tga.header[0];// Determine The TGA Width	(highbyte*256+lowbyte)
	height = tga.header[3] * 256 + tga.header[2];// Determine The TGA Height	(highbyte*256+lowbyte)
	bpp	= tga.header[4];					// Determine Bits Per Pixel
	tga.Width		= width;					// Copy width to local structure
	tga.Height		= height;					// Copy width to local structure
	tga.Bpp			= bpp;						// Copy width to local structure

	if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp !=32))){	//Make sure all texture info is ok
		cerr<<"Invalid texture information\n";	// If it isnt...Display error
		if(fTGA != NULL){
			fclose(fTGA);
		}
		return false;
	}

	if(bpp == 24)		// If the BPP of the image is 24...
		type	= GL_RGB;//Set Image type to GL_RGB
	else{						// Else if its 32 BPP
		type	= GL_RGBA;//Set image type to GL_RGBA
		cout<<"Alpha channel detected in \""<<filename<<"\"!";
	}
	tga.bytesPerPixel	= (tga.Bpp / 8);// Compute BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);// Compute amout of memory needed to store image
	imageData	= (GLubyte *)malloc(tga.imageSize);// Allocate that much memory

	if(imageData == NULL){// If it wasnt allocated correctly..
		cerr<<"Could not allocate memory for image. :O\n";
		fclose(fTGA);
		return false;
	}

	GLuint pixelcount	= tga.Height * tga.Width;							// Nuber of pixels in the image
	GLuint currentpixel	= 0;												// Current pixel being read
	GLuint currentbyte	= 0;												// Current byte 
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);			// Storage for 1 pixel

	do
	{
		GLubyte chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0){				// Read in the 1 byte header
			cerr<<"Could not read RLE header.\n";
			if(fTGA != NULL){
				fclose(fTGA);
			}
			if(imageData != NULL){
				free(imageData);
			}
			return false;
		}

// If the header is < 128, it means that is the number of RAW color packets minus 1 that follow the header
		if(chunkheader < 128){
			chunkheader++;// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++){// Read RAW color values
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel){ // Try to read 1 pixel
					cerr<<"Could not read image data!\n";
					if(fTGA != NULL)
						fclose(fTGA);
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(imageData != NULL)
						free(imageData);
					return false;
				}
																		// write to memory
				imageData[currentbyte		] = colorbuffer[2];	// Flip R and B vcolor values around in the process 
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)// if its a 32 bpp image
					imageData[currentbyte + 3] = colorbuffer[3];// copy the 4th byte

				currentbyte += tga.bytesPerPixel;// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;					// Increase current pixel by 1

				if(currentpixel > pixelcount){// Make sure we havent read too many pixels
					cerr<<"Too many pixels to read!\n";
					if(fTGA != NULL)
						fclose(fTGA);
					return true;
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(imageData != NULL)
						free(imageData);
					return false;
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel){		// Attempt to read following color values
					cerr<<"Could not read from file!\n";
					if(fTGA != NULL)
						fclose(fTGA);
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(imageData != NULL)
						free(imageData);
					return false;
			}

			for(short counter = 0; counter < chunkheader; counter++){// copy the color into the image data as many times as dictated by the header																			//
				imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)								// If TGA images is 32 bpp
					imageData[currentbyte + 3] = colorbuffer[3];//Copy 4th byte

				currentbyte += tga.bytesPerPixel;// Increase current byte by the number of bytes per pixel
				currentpixel++;					// Increase pixel count by 1

				if(currentpixel > pixelcount){	// Make sure we havent written too many pixels
					cerr<<"Too many pixels read!\n";
					if(fTGA != NULL)
						fclose(fTGA);
					return true;
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(imageData != NULL)
						free(imageData);
					return false;
				}
			}
		}
	}

	while(currentpixel < pixelcount);// Loop while there are still pixels left
	fclose(fTGA);					// Close the file
	return true;					// return success
}
*/
