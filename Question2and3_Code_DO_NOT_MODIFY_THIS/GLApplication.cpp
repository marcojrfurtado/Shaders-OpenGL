//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#include <stdlib.h>
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <cmath>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "utilityFunctions.h"
#include "AdjustableCamera.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include "ActiveMesh.h"
#include "Texture.h"
#include "ColouredTextureMaterial.h"
#include "BasicShaderMaterial.h"
#include "TextureShaderMaterial.h"

using namespace std;

//Global variables:

//TODO: delete these:
static StaticMesh* gSpinningMesh = 0;
static StaticMesh* gSunMesh = 0;
static Material* gTestShader1;
static BasicShaderMaterial* gTestShader2;

static bool bModelRotating = true;

static float secondsSinceAppStart;

static const float DEFAULT_NEAR_CLIPPING_PLANE_DISTANCE = 10.0f;
static const float DEFAULT_FAR_CLIPPING_PLANE_DISTANCE  = 200000.0f;
static const float DEGREES_PER_RADIAN = 57.2958f;
static const float RADIANS_PER_DEGREE = 0.0174533f;

static AdjustableCamera camera;//Our camera in the virtual world
static bool bShowFramerate = false; // by default, do we want the current fps displayed?
static GLuint texture[5];

static vector<Texture*> textures;//The giant struct of points to textures
static vector<Mesh*> meshes;//Our giant struct of pointers to meshes

static vector<TextureShaderMaterial*> timeBasedMaterials;//Materials that use the time variable

static vector<StaticMesh*> staticMeshes;
static vector<ActiveMesh*> activeMeshes;

char strafeLeftKey = 'A';
char strafeRightKey= 'D';
char moveForwardKey ='W';
char moveBackwardKey='S';
char lookLeftKey   = (char)37;
char lookRightKey  = (char)39;
char lookUpwardsKey =(char)38;
char lookDownwardsKey=(char)40;
char hoverUpwardsKey = ' ';
char hoverDownwardsKey='C';

//Sun controls
char sunRiseKey = 'I';
char sunSetKey = 'J';

bool bSunRising;
bool bSunSetting;
float sunPitchAngle;
float sunYawAngle;
const float sunDistance = 96000;

GLfloat gSunlightAmbientColor[4]  = { 0.1f, 0.1f, 0.3f, 1.0f };
GLfloat gSunlightDiffuseColor[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat gSunlightSpecularColor[4] = { 1.0f, 0.1f, 0.1f, 1.0f };
GLfloat gSunlightDirectionVector[4]={ 0.577f, 0.577f, 0.577f, 0.0f };//Note: this is a directional light

char toggleFPSKey   ='F';
char toggleRotationKey='R';


static bool gbSceneNeedsReinitializing = true;
static GLsizei currentViewportWidth;
static GLsizei currentViewportHeight;
static long gTimeOfLastFrame;

GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
void initGL();
void deviceResetGL();
void displayGL();
void resizeGL(GLsizei width, GLsizei height);
void keyDown(char key);
void keyUp(char key);
int randInt(int a, int b){return rand()%(b+1-a)+a;}
long getTime(){return glutGet(GLUT_ELAPSED_TIME);}
void calculateAndShowFPS();
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);

//TODO: remove all of these!
void drawTriangleAndSquare();
void draw_WierdBox();
void drawTree(GLfloat x =0.0, GLfloat y=0.0, GLfloat z=0.0, 
			  GLfloat zrotation =0.0, GLfloat xscale =1.0, GLfloat yscale =1.0, GLfloat zscale =1.0);

void draw_stuff();
void initializeScene();
void init_resources();
void updateSunlightPositionAndColor();
void quitProgram();
void idle();

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);
	glutInitWindowSize (1280, 768);
	glutInitWindowPosition (50, 50);
	glutCreateWindow("CMPUT411 Assn4: Shaders");

	initGL();

	glutDisplayFunc(displayGL);
	glutIdleFunc(idle);
	glutReshapeFunc(resizeGL);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutMainLoop();
	return 0;
}


// Resize And Initialize The GL Window
void resizeGL(GLsizei width, GLsizei height){
	if (height==0){
		height=1;// Prevent A Divide By Zero By
	}

	currentViewportWidth = width;
	currentViewportHeight = height;

	glViewport(0, 0, width, height);// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);// Select The Projection Matrix
	glLoadIdentity();// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(90.0f,(GLfloat)width/(GLfloat)height,DEFAULT_NEAR_CLIPPING_PLANE_DISTANCE,DEFAULT_FAR_CLIPPING_PLANE_DISTANCE);

	glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
	glLoadIdentity();// Reset The Modelview Matrix
}



void initGL(){
	glClearColor(0.5, 0.5, 1.0, 0.0);//Set the back ground color (light, soothing blue).
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH);//Gives objects that 3D shading look

	glEnable(GL_LIGHTING);//enable lighting in the scene

	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);//set the properties of LIGHT 0
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	glLightfv(GL_LIGHT0, GL_AMBIENT, gSunlightAmbientColor);//set the properties of LIGHT 0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, gSunlightDiffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR,gSunlightSpecularColor);
	glLightfv(GL_LIGHT0, GL_POSITION,gSunlightDirectionVector);
	sunYawAngle = 45.0f;
	sunPitchAngle = 30.0f;
	gSunlightDirectionVector[0] = cosf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
	gSunlightDirectionVector[1] = sinf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
	gSunlightDirectionVector[2] = cosf(sunPitchAngle*RADIANS_PER_DEGREE);
	gSunlightDirectionVector[3] = 0;

	glEnable(GL_LIGHT0);//Turn on light 0

	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER,0.5f);
	//glEnable(GL_ALPHA_TEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// Really Nice Perspective Calculations

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);//REENABLE THIS

	GLenum err = glewInit();
	if(err != 0){
		std::cout<<"WARNING! Glew errorcode: "<<err<<"\n";
	}

	if(gbSceneNeedsReinitializing){
		initializeScene();
		gbSceneNeedsReinitializing = false;
	}

	return;							// Initialization Went OK
}

void initializeScene(){
	gTimeOfLastFrame = 0;
	secondsSinceAppStart = 0;
	
	init_resources();
	updateSunlightPositionAndColor();
}

void init_resources()
{

	//Textures
	//Texture* eyeBallTexture = new Texture("images/greenEyeBall.tga");
	//Texture* islandTexture = new Texture("images/islandMap.tga");
	Texture* airplaneTexture =			new Texture("images/planeTexture.tga");
	Texture* waterTexture =				new Texture("images/scene/water.tga");
	Texture* skyTexture =				new Texture("images/scene/bigSky.tga");
	//Texture* spiralTexture =			new Texture("images/goldSpiral.tga");
	Texture* blackRockTexture =			new Texture("images/scene/blackRock.tga");
	Texture* spikeyLeavesTexture =		new Texture("images/scene/spikeyLeaves.tga",false,true,true);
	Texture* palmBarkTexture =			new Texture("images/scene/palmWood_D.tga");
	Texture* islandSandTexture =		new Texture("images/scene/superBrightBeachSand.tga");
	Texture* grassWithMaskTexture =		new Texture("images/scene/grassWithMask.tga");
	Texture* shallowBlueSandTexture =	new Texture("images/scene/deepSand.tga");
	Texture* metalSheetTexture =		new Texture("images/scene/rustySheetMetalTile.tga");
	Texture* metalSheetLightMapTexture =new Texture("images/scene/metalSheetLightMap.tga");
	Texture* boneTexture =				new Texture("images/scene/brightBones.tga");
	Texture* rustedMetalTexture =		new Texture("images/scene/darkSteel.tga");
	Texture* nastyOldMetalTexture=		new Texture("images/scene/rustedMetal.tga");
	Texture* rustySpindles=				new Texture("images/scene/rustySpindles_alphaMapped.tga");
	

	//textures.push_back(eyeBallTexture);
	//textures.push_back(islandTexture);
	textures.push_back(airplaneTexture);
	textures.push_back(waterTexture);
	textures.push_back(skyTexture);
	//textures.push_back(spiralTexture);
	textures.push_back(islandSandTexture);
	
	
	//Materials
	//ColouredTextureMaterial* pinkEyeMaterial = new ColouredTextureMaterial(eyeBallTexture,1.0f,0.7f,0.7f);
	//ColouredTextureMaterial* islandMaterial = new ColouredTextureMaterial(islandTexture);
	//ColouredTextureMaterial* lightMaterial = new ColouredTextureMaterial(NULL,1.0f,1.0f,0.6f);
	//ColouredTextureMaterial* airplaneMaterial = new ColouredTextureMaterial(airplaneTexture);
	//ColouredTextureMaterial* waterMaterial = new ColouredTextureMaterial(waterTexture);
	//ColouredTextureMaterial* skyMaterial = new ColouredTextureMaterial(skyTexture);
	//ColouredTextureMaterial* spiralMaterial = new ColouredTextureMaterial(spiralTexture);
	//ColouredTextureMaterial* islandSandMaterial = new ColouredTextureMaterial(islandSandTexture);
	//ColouredTextureMaterial* blackRockMaterial = new ColouredTextureMaterial(blackRockTexture);
	//ColouredTextureMaterial* spikeyLeavesMaterial = new ColouredTextureMaterial(spikeyLeavesTexture);
	//ColouredTextureMaterial* palmBarkMaterial = new ColouredTextureMaterial(palmBarkTexture);
	//ColouredTextureMaterial* shallowSandMaterial = new ColouredTextureMaterial(shallowBlueSandTexture);
	//ColouredTextureMaterial* metalSheetMaterial = new ColouredTextureMaterial(metalSheetTexture);
	//ColouredTextureMaterial* brightBoneMaterial = new ColouredTextureMaterial(boneTexture);
	
	TextureShaderMaterial* airplaneMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,airplaneTexture->getTextureID());
	
	TextureShaderMaterial* waterMaterial = new TextureShaderMaterial("shaders/oceanWater.vert",
		"shaders/oceanWater.frag",true,true, waterTexture->getTextureID());
	timeBasedMaterials.push_back(waterMaterial);
	
	TextureShaderMaterial* skyMaterial = new TextureShaderMaterial("shaders/skyShader.vert",
		"shaders/skyShader.frag",false,true,skyTexture->getTextureID());
	timeBasedMaterials.push_back(skyMaterial);

	TextureShaderMaterial* sunMaterial = new TextureShaderMaterial("shaders/theSun.vert",
		"shaders/theSun.frag",true,false);
	
	TextureShaderMaterial* islandSandMaterial = new TextureShaderMaterial("shaders/islandTerrain.vert",
		"shaders/islandTerrain.frag",false,false,islandSandTexture->getTextureID(), grassWithMaskTexture->getTextureID());
	
	TextureShaderMaterial* blackRockMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,blackRockTexture->getTextureID());
	
	TextureShaderMaterial* spikeyLeavesMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,spikeyLeavesTexture->getTextureID());
	
	TextureShaderMaterial* palmBarkMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,palmBarkTexture->getTextureID());
	
	TextureShaderMaterial* shallowSandMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,shallowBlueSandTexture->getTextureID());
	
	TextureShaderMaterial* metalSheetMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,metalSheetTexture->getTextureID());
	
	TextureShaderMaterial* brightBoneMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,boneTexture->getTextureID());
	
	TextureShaderMaterial* lightMappedMetalSheetMaterial = new TextureShaderMaterial("shaders/lightmappedMetal.vert",
		"shaders/lightmappedMetal.frag",false,true,metalSheetTexture->getTextureID(),
		metalSheetLightMapTexture->getTextureID());
	timeBasedMaterials.push_back(lightMappedMetalSheetMaterial);
	
	TextureShaderMaterial* radioBoxMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,nastyOldMetalTexture->getTextureID());
	
	TextureShaderMaterial* radioDetailsMaterial = new TextureShaderMaterial("shaders/diffuseTexture.vert",
		"shaders/diffuseTexture.frag",false,false,rustedMetalTexture->getTextureID());
	
	TextureShaderMaterial* radioSpindlesMaterial = new TextureShaderMaterial("shaders/tapeSpindles.vert",
		"shaders/tapeSpindles.frag",false,true,rustySpindles->getTextureID());
	timeBasedMaterials.push_back(radioSpindlesMaterial);
	
	TextureShaderMaterial* radioBeaconMaterial = new TextureShaderMaterial("shaders/beaconLight.vert",
		"shaders/beaconLight.frag",false,true);
	timeBasedMaterials.push_back(radioBeaconMaterial);
	
	TextureShaderMaterial* radioButtonsMaterial = new TextureShaderMaterial("shaders/buttons.vert",
		"shaders/buttons.frag",false,false);

	//Shaders 
	//BasicShaderMaterial* testShader1 = new BasicShaderMaterial("shaders/toon.vert","shaders/toon.frag",false);
	//BasicShaderMaterial* testShader1 = new BasicShaderMaterial("shaders/light.vert","shaders/light.frag",false);
	//BasicShaderMaterial* testShader1 = new BasicShaderMaterial("shaders/lightS2.vert","shaders/lightS2.frag",false);
	//BasicShaderMaterial* testShader1 = new BasicShaderMaterial("shaders/specular.vert","shaders/specular.frag",false);
	//TextureShaderMaterial* testShader1 = new TextureShaderMaterial("shaders/toon.vert","shaders/toon.frag",
	//	false,false,spiralTexture->getTextureID(), waterTexture->getTextureID());
	//TextureShaderMaterial* testShader1 = new TextureShaderMaterial("shaders/toon.vert",
	//	"shaders/toon.frag",false,false,spiralTexture->getTextureID(), waterTexture->getTextureID());

	//timeBasedMaterials.push_back(testShader1);
	//gTestShader1 = testShader1;

	//Meshes
	Mesh* airplaneMesh = new Mesh("meshes/airplane.obj");
	Mesh* eyeBallMesh = new Mesh("meshes/eyeBall.obj");
	Mesh* waterMesh = new Mesh("meshes/waterSurface.obj");
	Mesh* skyMesh = new Mesh("meshes/skyDome.obj");
	Mesh* sandyIslandMesh = new Mesh("meshes/scene/sandyIsland.obj");
	Mesh* blackRocksMesh = new Mesh("meshes/scene/blackRocks.obj");
	Mesh* palmTreeTrunks = new Mesh("meshes/scene/treeTrunks.obj");
	Mesh* palmTreeTops = new Mesh("meshes/scene/treeTops.obj");
	Mesh* smallSpikeyPlants = new Mesh("meshes/scene/smallPlants.obj");
	Mesh* underwaterSlope = new Mesh("meshes/scene/underwaterSlope.obj");
	Mesh* metalSheetOnBeachMesh = new Mesh("meshes/scene/metalSheetOnBeach.obj");
	Mesh* metalSheetByRadioMesh = new Mesh("meshes/scene/metalSheetByRadio.obj");
	Mesh* bonyHandMesh  = new Mesh("meshes/scene/bonyHand.obj");
	Mesh* radioBoxMesh  = new Mesh("meshes/scene/radioBox.obj");
	Mesh* radioDetailsMesh  = new Mesh("meshes/scene/radioDetails.obj");
	Mesh* radioSpindlesMesh  = new Mesh("meshes/scene/radioSpindles.obj");
	Mesh* radioBeaconLightMesh  = new Mesh("meshes/scene/radioBeaconLight.obj");
	Mesh* radioButtonsMesh  = new Mesh("meshes/scene/radioButtons.obj");
	
	//Mesh* islandMesh = new Mesh("meshes/oldIsland.obj");
	//Mesh* boulderMesh = new Mesh("meshes/singleBoulder.obj");
	//Mesh* boulderMesh = new Mesh("meshes/skull.obj");
	//Mesh* spikeyBranchesMesh = new Mesh("meshes/palmTreeTopSpikeyLeaves.obj");
	//Mesh* palmTrunkMesh = new Mesh("meshes/palmTreeTrunkSpikey.obj");
	
	
	
	meshes.push_back(airplaneMesh);
	meshes.push_back(eyeBallMesh);
	meshes.push_back(waterMesh);
	meshes.push_back(skyMesh);
	meshes.push_back(sandyIslandMesh);
	
	//meshes.push_back(islandMesh);

	//Static meshes
	//staticMeshes.push_back(new StaticMesh("Air Plane", planeMesh, planeMaterial,500,-400,400,100,100,100));
	//Shader meshes
	//gSpinningMesh = new StaticMesh("PlaneShaded", planeMesh, testShader1,500,400,400,100,100,100);
	//gSpinningMesh = new StaticMesh("Air Plane", planeMesh, planeMaterial,500,-400,400,100,100,100);
	//staticMeshes.push_back(new StaticMesh("Eye Ball", eyeBallMesh, pinkEyeMaterial,0,0,4,1.0f,1.0f,1.0f));
	//staticMeshes.push_back(new StaticMesh("light", eyeBallMesh, lightMaterial,500,400,900,10,10,10));
	//	gLightPosition[0],gLightPosition[1],gLightPosition[2],10,10,10));
	//staticMeshes.push_back(new StaticMesh("Island", islandMesh, islandMaterial,0,0,0,100.0f,100.0f,100.0f));
	//	staticMeshes.push_back(new StaticMesh("Black Boulder", boulderMesh, blackRockMaterial,0,-100,20,1,1,1,90));
	//staticMeshes.push_back(new StaticMesh("Tree Trunk", palmTrunkMesh, simplePalmBarkMaterial));
	//staticMeshes.push_back(new StaticMesh("Tree Leaves", spikeyBranchesMesh, spikeyLeavesMaterial));

	
	staticMeshes.push_back(new StaticMesh("Sky", skyMesh, skyMaterial,0,0,-500,1000,1000,1000));
	staticMeshes.push_back(new StaticMesh("Water", waterMesh, waterMaterial,0,0,0,1500.0f,1500.0f,1.0f));
	staticMeshes.push_back(new StaticMesh("Sea floor", waterMesh, shallowSandMaterial,0,0,-500,1500.0f,1500.0f,1.0f,0,90));
	staticMeshes.push_back(new StaticMesh("Sandy Island", sandyIslandMesh, islandSandMaterial));
	staticMeshes.push_back(new StaticMesh("Black Rocks", blackRocksMesh, blackRockMaterial));
	staticMeshes.push_back(new StaticMesh("Palm Tree Trunks", palmTreeTrunks, palmBarkMaterial));
	staticMeshes.push_back(new StaticMesh("Palm Tree Tops", palmTreeTops, spikeyLeavesMaterial));
	staticMeshes.push_back(new StaticMesh("Spikey Plants", smallSpikeyPlants, spikeyLeavesMaterial));
	staticMeshes.push_back(new StaticMesh("Shallow Sandy Depths", underwaterSlope, shallowSandMaterial));
	staticMeshes.push_back(new StaticMesh("Metal Sheet on Beach", metalSheetOnBeachMesh, metalSheetMaterial));
	staticMeshes.push_back(new StaticMesh("Metal Sheet by Radio", metalSheetByRadioMesh, lightMappedMetalSheetMaterial));
	staticMeshes.push_back(new StaticMesh("Bony Hand", bonyHandMesh, brightBoneMaterial));
	staticMeshes.push_back(new StaticMesh("Crashed Airplane",airplaneMesh,airplaneMaterial,
		2500,-1400,0,90,90,90,-60,150,30));
		//9000,-3000,4000,90,90,90,-60,150,30));
	staticMeshes.push_back(new StaticMesh("Radio Box", radioBoxMesh, radioBoxMaterial));
	staticMeshes.push_back(new StaticMesh("Radio Details", radioDetailsMesh, radioDetailsMaterial));
	staticMeshes.push_back(new StaticMesh("Radio Spindles", radioSpindlesMesh, radioSpindlesMaterial));
	staticMeshes.push_back(new StaticMesh("Radio Beacon", radioBeaconLightMesh, radioBeaconMaterial));
	staticMeshes.push_back(new StaticMesh("Radio Buttons", radioButtonsMesh, radioButtonsMaterial));
	

	//Special meshes!!
	//Our Sun!!
	gSunMesh = new StaticMesh("The sun", eyeBallMesh, sunMaterial,
		gSunlightDirectionVector[0]*sunDistance,
		gSunlightDirectionVector[1]*sunDistance,
		gSunlightDirectionVector[2]*sunDistance,600.0f,600.0f,600.0f);
	staticMeshes.push_back(gSunMesh);

	//Spinning Airplane
	//gSpinningMesh = new StaticMesh("Airplane Shaded", airplaneMesh, testShader1,500,400,400,40,40,40);
	//staticMeshes.push_back(gSpinningMesh);

	return;
}

void idle()
{	
	//We now want to compute how much time has elapsed since the last time this function was called
	double secondsElapsedSinceLastUpdateCall = 0;
	long int currentTimeInMilliseconds = glutGet(GLUT_ELAPSED_TIME);
	if(currentTimeInMilliseconds == gTimeOfLastFrame){
		return;
	}

	if(gTimeOfLastFrame > 0){
		secondsElapsedSinceLastUpdateCall = ((double)(currentTimeInMilliseconds - gTimeOfLastFrame))/((double)1000.0);
	}
	
	//Update the time variable in materials that use the time variable for animation
	secondsSinceAppStart += (float)secondsElapsedSinceLastUpdateCall;
	for(unsigned int i = 0; i<timeBasedMaterials.size();i++){
		timeBasedMaterials[i]->updateTime(secondsSinceAppStart);
	}
	//cout<<"time:"<<secondsElapsedSinceLastFrameWasDrawn<<"\n";

	gTimeOfLastFrame = currentTimeInMilliseconds;
	

	//spinning object hack
	//if(gSpinningMesh != NULL && bModelRotating){
	//	gSpinningMesh->yaw += 25*(float)secondsElapsedSinceLastUpdateCall;
	//}

	glutPostRedisplay();
}

void displayGL(){								// Here's Where We Do All The Drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And The Depth Buffer
	glLoadIdentity();							// Reset The Current Modelview Matrix

	calculateAndShowFPS();
	camera.updateCamera();
	//camera.updatePositionAndOrientation(secondsElapsedSinceLastFrameWasDrawn);
	//camera.updateProjection();

	//Lights need to be positioned at "draw-time"
	//float directionalLight[] = {0.577f,0.577f,0.577f,0.0};
	//glLightfv(GL_LIGHT0, GL_POSITION,directionalLight);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, gSunlightAmbientColor);//set the properties of LIGHT 0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, gSunlightDiffuseColor);
	glLightfv(GL_LIGHT0, GL_POSITION,gSunlightDirectionVector);

	//Rendering code goes here:

	//OPAQUE PASS: render all non alpha-blended stuff first
	glEnable(GL_ALPHA_TEST);
	//glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	for(unsigned int i = 0; i< staticMeshes.size(); i++){
		staticMeshes[i]->draw(true,false);
	}
	

	//float r = sin(((double)currentTimeInTicks)/1000.0);
	//glEnable(GL_BLEND);
	glColor4f(1.0f,0.5,0.5f,1.0f);


	//TRANSLUCENT PASS: disable writing to the depth buffer and render all alpha-blended stuff
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDepthMask(false);//prevents translucent meshes from hiding each other
	for(unsigned int i = 0; i< staticMeshes.size(); i++){
			staticMeshes[i]->draw(false,true);
	}
	glDepthMask(true);

	int error = glGetError();
	if(error){
		std::cout<<"WARNING! OPENGL ERROR FOUND!"<<gluErrorString(error)<<"\n";
	}// */

	glutSwapBuffers();// Enables Double buffering
}

void updateSunlightPositionAndColor(){
		gSunlightDirectionVector[0] = cosf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[1] = sinf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[2] = cosf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[3] = 0;

		gSunlightAmbientColor[0] = 0.3f-((abs(sunPitchAngle))*0.0025f);
		gSunlightAmbientColor[1] = 0.3f-((abs(sunPitchAngle))*0.0025f);
		gSunlightAmbientColor[2] = 0.3f;//0.5f-((abs(sunPitchAngle))*0.0025f);
		
		gSunlightDiffuseColor[0] = 1.4f-((abs(sunPitchAngle))*0.0025f);
		gSunlightDiffuseColor[1] = 1.3f-((abs(sunPitchAngle))*0.0100f);
		gSunlightDiffuseColor[2] = 1.3f-((abs(sunPitchAngle))*0.0100f);

		if(gSunlightDiffuseColor[0] > 1.0f)
			gSunlightDiffuseColor[0] = 1.0f;
		if(gSunlightDiffuseColor[1] > 1.0f)
			gSunlightDiffuseColor[1] = 1.0f;
		if(gSunlightDiffuseColor[2] > 0.9f)
			gSunlightDiffuseColor[2] = 0.9f;
		
		//GLfloat gSunlightDiffuseColor[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
		//GLfloat gSunlightSpecularColor[4] = { 1.0f, 0.1f, 0.1f, 1.0f };

		if(gSunMesh != NULL){
			gSunMesh->x = gSunlightDirectionVector[0]*sunDistance;
			gSunMesh->y = gSunlightDirectionVector[1]*sunDistance;
			gSunMesh->z = gSunlightDirectionVector[2]*sunDistance;
		}
}

void keyboard(unsigned char key, int x, int y)
{
	camera.keyInput(key);//The camera also has its own keyboard controls that it listens for
	switch (key)
	{
		case 'i': //Sunrise control
			sunPitchAngle += 2.0f;
			if(sunPitchAngle > 80.0f){
				sunPitchAngle = 80.0f;
			}
			updateSunlightPositionAndColor();
			break;

		case 'j': //Sunset control
			sunPitchAngle   -= 2.0f;
			if(sunPitchAngle < -80.0f){
				sunPitchAngle = -80.0f;
			}
			updateSunlightPositionAndColor();
			break;

		case 'q': exit(0); break;
		case 27: exit(0); break;//(i.e. Esc key)
	 }
}

void specialKeyboard(int key, int x, int y)//This handles the special keys like the arrow keys
{	
	camera.specialKeys(key);//The camera is interested in the arrow keys for camera movement
}

//void keyDown(char key){
//	
//	/* This callback is invoked when a user presses a key down.
//	* Note: ascii key 27 is "ESC" */
//
//	if(key == lookLeftKey){
//		camera.setTurningLeft(true);
//	}else if(key == lookRightKey){
//		camera.setTurningRight(true);
//	}else if(key == lookUpwardsKey){
//		camera.setTurningUpwards(true);
//	}else if(key == lookDownwardsKey){
//		camera.setTurningDownwards(true);
//	}else if(key == moveForwardKey){
//		camera.setMovingForwards(true);
//	}else if(key == moveBackwardKey){
//		camera.setMovingBackwards(true);
//	}else if(key == strafeLeftKey){
//		camera.setStrafingLeft(true);
//	}else if(key == strafeRightKey){
//		camera.setStrafingRight(true);
//	}else if(key == hoverUpwardsKey){
//		camera.setHoveringUpwards(true);
//	}else if(key == hoverDownwardsKey){
//		camera.setHoveringDownwards(true);
//
//	}else if(key == sunRiseKey){
//		bSunRising = true;
//	}else if(key == sunSetKey){
//		bSunSetting = true;
//
//	}else if(key == toggleFPSKey){
//		bShowFramerate = !bShowFramerate;
//	}else if(key == toggleRotationKey){
//		bModelRotating = !bModelRotating;
//	}else{
//		cout<<"The key \'"<<key<<"\' With integer value "<<(int)key<<"is not bound.\n";
//	}
//	return;
//}
//
//void keyUp(char key){
//	//cout<<"Key up!"<<key<<"\n";
//	if(key == lookLeftKey){
//		camera.setTurningLeft(false);
//	}else if(key == lookRightKey){
//		camera.setTurningRight(false);
//	}else if(key == moveForwardKey){
//		camera.setMovingForwards(false);
//	}else if(key == moveBackwardKey){
//		camera.setMovingBackwards(false);
//	}else if(key == lookUpwardsKey){
//		camera.setTurningUpwards(false);
//	}else if(key == lookDownwardsKey){
//		camera.setTurningDownwards(false);
//	}else if(key == strafeLeftKey){
//		camera.setStrafingLeft(false);
//	}else if(key == strafeRightKey){
//		camera.setStrafingRight(false);
//	}else if(key == hoverUpwardsKey){
//		camera.setHoveringUpwards(false);
//	}else if(key == hoverDownwardsKey){
//		camera.setHoveringDownwards(false);
//	
//	}else if(key == sunRiseKey){
//		bSunRising = false;
//	}else if(key == sunSetKey){
//		bSunSetting = false;
//	}
//}
//
void calculateAndShowFPS()
{
	static bool firstTime = true;
	if(firstTime){
		firstTime = false;
	}

	static int renders = 0;//Helps count fps
	static long lastCheckin = 0;
	renders++;

	if(getTime()> (lastCheckin + 1000) && bShowFramerate)//displays current framerate
	{
		lastCheckin = getTime();
		cout<<"fps:"<<renders<<endl;
		renders=0;
	}
}
/*
void drawTree(GLfloat x, GLfloat y, GLfloat z, GLfloat zrotation, GLfloat zscale, GLfloat yscale, GLfloat xscale)
{
const int SIDESnum = 5;

glPushMatrix();
//GLfloat leaves[] = {0.1, 0.8, 0.0};
//GLfloat bark[]  = {0.8, 0.4, 0.0};
//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bark);
glTranslatef(x, y, z);
glRotatef(zrotation, 0.0 , 0.0 , 1.0f);
glScalef(xscale, yscale, zscale);

glBindTexture(GL_TEXTURE_2D, texture[3]);
glutSolidCone(0.2, 3.0, 5.0, 1.0);
//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, leaves);
glPushMatrix();
glTranslatef(0,0,0.6);
glRotatef(0,0,0,1);
glutSolidCone(0.8, 1.1, SIDESnum, 1.0);
glPopMatrix();
glBindTexture(GL_TEXTURE_2D, texture[4]);
glPushMatrix();
glTranslatef(0,0,1.6);
glRotatef(90,0,0,1);
glutSolidCone(0.7, 0.9, SIDESnum, 1.0);
glPopMatrix();
glPushMatrix();
glTranslatef(0,0,2.4);
glRotatef(30,0,0,1);
glutSolidCone(0.6, 0.7, SIDESnum, 1.0);
glPopMatrix();

glPopMatrix();
}*/
void draw_WierdBox(){
	glPushMatrix();{
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glBegin(GL_QUADS);{
			// Front Face
			glNormal3f( 0.0f, 0.0f, 1.0f);					// Normal Pointing Towards Viewer
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 1 (Front)
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 2 (Front)
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Front)
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 4 (Front)
			// Back Face
			glNormal3f( 0.0f, 0.0f,-1.0f);					// Normal Pointing Away From Viewer
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Back)
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 2 (Back)
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 3 (Back)
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 4 (Back)
			// Top Face
			glNormal3f( 0.0f, 1.0f, 0.0f);					// Normal Pointing Up
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 1 (Top)
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 2 (Top)
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Top)
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 4 (Top)
			// Bottom Face
			glNormal3f( 0.0f,-1.0f, 0.0f);					// Normal Pointing Down
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Bottom)
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 2 (Bottom)
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 3 (Bottom)
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 4 (Bottom)
			// Right face
			glNormal3f( 1.0f, 0.0f, 0.0f);					// Normal Pointing Right
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Point 1 (Right)
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Point 2 (Right)
			glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Right)
			glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 4 (Right)
			// Left Face
			glNormal3f(-1.0f, 0.0f, 0.0f);					// Normal Pointing Left
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Point 1 (Left)
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 2 (Left)
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 3 (Left)
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Point 4 (Left)
		}glEnd();
	}
	glPopMatrix();
}

void drawTriangleAndSquare(){
	glTranslatef(-1.5f,0.0f,-6.0f);					// Move Left 1.5 Units And Into The Screen 6.0
	glBegin(GL_TRIANGLES);{						// Drawing Using Triangles
		glVertex3f( 0.0f, 1.0f, 0.0f);				// Top
		glVertex3f(-1.0f,-1.0f, 0.0f);				// Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);				// Bottom Right
	}glEnd();
	glTranslatef(3.0f,0.0f,0.0f);					// Move Right 3 Units
	glBegin(GL_QUADS);{						// Draw A Quad
		glVertex3f(-1.0f,-1.0f, 0.0f);				// Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);				// Bottom Right
		glVertex3f( 1.0f, 1.0f, 0.0f);				// Top Right
		glVertex3f(-1.0f, 1.0f, 0.0f);				// Top Left	
	}glEnd();	
}


void clearAllLoadedResources(){
	//clear out all data in the vectors
	for(unsigned int i = 0; i< textures.size(); i++)
		delete textures[i];
	for(unsigned int i = 0; i< meshes.size(); i++)
		delete meshes[i];
	for(unsigned int i = 0; i< staticMeshes.size(); i++)
		delete staticMeshes[i];
	for(unsigned int i = 0; i< activeMeshes.size(); i++)
		delete activeMeshes[i];
	textures.clear();
	meshes.clear();
	staticMeshes.clear();
	activeMeshes.clear();
	return;
}

void deviceResetGL(){
	resizeGL(currentViewportWidth, currentViewportHeight);
	initGL();
	return;								// Initialization Went OK
}
