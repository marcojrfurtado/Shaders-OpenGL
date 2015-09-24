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

#include "AdjustableCamera.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include "Material.h"

//Dirty hack to separate student code from rest of the code base:
#include "shaders/question1_student_code.h"

using namespace std;

//Global variables:

//TODO: delete these:
static StaticMesh* gSpinningMesh = 0;
static StaticMesh* gSunMesh = 0;

static bool bModelRotating = true;

static float secondsSinceAppStart;

static const float DEFAULT_NEAR_CLIPPING_PLANE_DISTANCE = 10.0f;
static const float DEFAULT_FAR_CLIPPING_PLANE_DISTANCE  = 200000.0f;
static const float DEGREES_PER_RADIAN = 57.2958f;
static const float RADIANS_PER_DEGREE = 0.0174533f;

static AdjustableCamera camera;//Our camera in the virtual world
static bool bShowFramerate = false; // by default, do we want the current fps displayed?

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
long getTime(){return glutGet(GLUT_ELAPSED_TIME);}
void calculateAndShowFPS();

void draw_stuff();
void initializeScene();
void init_resources();
void updateSunlightPositionAndColor();
void quitProgram();
void idle();
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);

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
	//glClearColor(0.5, 0.5, 1.0, 0.0);//Set the back ground color (light, soothing blue).
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
	sunPitchAngle = -45.0f;
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
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

	return;// Initialization Went OK
}

void initializeScene(){
	gTimeOfLastFrame = 0;
	secondsSinceAppStart = 0;

	camera.x = -300;
	camera.y = 0;
	camera.z = 100;
	
	init_resources();
	updateSunlightPositionAndColor();
}

void init_resources()
{
	load_shaders();
	gSpinningMesh = new StaticMesh("Toon shaded airplane", new Mesh("meshes/airplane.obj"), NULL,0,0,0,40,40,40);
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
	
	secondsSinceAppStart += (float)secondsElapsedSinceLastUpdateCall;
	gTimeOfLastFrame = currentTimeInMilliseconds;
	

	//spinning object hack
	if(gSpinningMesh != NULL && bModelRotating){
		gSpinningMesh->yaw += 25*(float)secondsElapsedSinceLastUpdateCall;
	}

	glutPostRedisplay();
}

void displayGL(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	camera.updateCamera();
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, gSunlightAmbientColor);//set the properties of LIGHT 0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, gSunlightDiffuseColor);
	glLightfv(GL_LIGHT0, GL_POSITION,gSunlightDirectionVector);

	calculateAndShowFPS();

	//Draw everything
	gSpinningMesh->draw();

	glutSwapBuffers();// Enables Double buffering
}

void updateSunlightPositionAndColor(){
		gSunlightDirectionVector[0] = cosf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[1] = sinf(sunYawAngle*RADIANS_PER_DEGREE)*sinf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[2] = cosf(sunPitchAngle*RADIANS_PER_DEGREE);
		gSunlightDirectionVector[3] = 0;

		gSunlightAmbientColor[0] = 0.3f;
		gSunlightAmbientColor[1] = 0.3f;
		gSunlightAmbientColor[2] = 0.3f;
		
		gSunlightDiffuseColor[0] = 1.0f;
		gSunlightDiffuseColor[1] = 1.0f;
		gSunlightDiffuseColor[2] = 1.0f;

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

void calculateAndShowFPS()
{
	static bool firstTime = true;
	if(firstTime){
		firstTime = false;
	}

	static int renders = 0;//Helps count fps
	static long lastCheckin = 0;
	renders++;

	if(getTime()> lastCheckin + 1000 && bShowFramerate)//displays current framerate
	{
		lastCheckin = getTime();
		cout<<"fps:"<<renders<<endl;
		renders=0;
	}
}

void clearAllLoadedResources(){
	return;
}

void deviceResetGL(){
	resizeGL(currentViewportWidth, currentViewportHeight);
	initGL();
	return;
}

