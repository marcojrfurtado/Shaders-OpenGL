//CMPUT 411 Assignment
//Author (TA): Nathaniel Rossol

#ifndef ADJUSTABLE_CAMERA_H
#define ADJUSTABLE_CAMERA_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

class AdjustableCamera
{
public: //You can safely read and write these values, and the camera will behave appropriately

	GLdouble x;
	GLdouble y;
	GLdouble z;
	
	//Note: these values are all in degrees
	double pitch;
	double yaw;
	double roll;
	
	GLdouble moveSpeed;
	GLdouble lookSpeed;

private:

	//Default forward vector
	float DEFAULT_FORWARD_X,DEFAULT_FORWARD_Y,DEFAULT_FORWARD_Z;
	//Default up vector
	float DEFAULT_UP_X,DEFAULT_UP_Y,DEFAULT_UP_Z;
	//Default right vector
	float DEFAULT_RIGHT_X,DEFAULT_RIGHT_Y,DEFAULT_RIGHT_Z;

	//Camera's forward Vector
	float forward_x,forward_y,forward_z;
	//Camera's up Vector
	float up_x,up_y,up_z;
	//Camera's right Vector
	float right_x,right_y,right_z;

public:

	inline static double PI() { return 3.14159265358979323846264338327950; }//C++ doesn't let you define static const doubles in .h files, hence this
	inline static double degreeToRadians(double degrees){ return (PI()/180.0)*degrees;}
	inline static float degreeToRadiansf(float degrees){ return (PI()/180.0f)*degrees;}

	AdjustableCamera()
	{
		//By default look in the +X direction
		forward_x = DEFAULT_FORWARD_X = 1;
		forward_y = DEFAULT_FORWARD_Y = 0;
		forward_z = DEFAULT_FORWARD_Z = 0;

		//By default +Z is "up"
		up_x = DEFAULT_UP_X = 0;
		up_y = DEFAULT_UP_Y = 0;
		up_z = DEFAULT_UP_Z = 1;

		//By default, -Y is "right"
		right_x = DEFAULT_RIGHT_X = 0;
		right_y = DEFAULT_RIGHT_Y = -1;
		right_z = DEFAULT_RIGHT_Z = 0;

		x = 0;
		y = 0;
		z = 400;

		pitch = 0;
		yaw = 0;
		roll = 0;

		moveSpeed = 50.1;
		lookSpeed = 10.0;
	}

	void convertAxisAngleToQuaternion(float angle_radians, float unit_x,float unit_y,float unit_z,float& qw,float& qx,float& qy,float& qz){
		
		float s = sin(angle_radians/2.0f);
		
		qw = cos(angle_radians/2.0f);
		qx = unit_x * s;
		qy = unit_y * s;
		qz = unit_z * s;
	}

	void quaternionMultiplication(float q1_w,float q1_x,float q1_y,float q1_z,float q2_w,float q2_x,float q2_y,float q2_z,
			float& result_w,float& result_x,float& result_y,float& result_z){
		float A, B, C, D, E, F, G, H;
		A = (q1_w + q1_x)*(q2_w + q2_x);
		B = (q1_z - q1_y)*(q2_y - q2_z);
		C = (q1_w - q1_x)*(q2_y + q2_z); 
		D = (q1_y + q1_z)*(q2_w - q2_x);
		E = (q1_x + q1_z)*(q2_x + q2_y);
		F = (q1_x - q1_z)*(q2_x - q2_y);
		G = (q1_w + q1_y)*(q2_w - q2_z);
		H = (q1_w - q1_y)*(q2_w + q2_z);
		result_w = B + (-E - F + G + H) /2.0f;
		result_x = A - (E + F + G + H)/2.0f; 
		result_y = C + (E - F + G - H)/2.0f; 
		result_z = D + (E - F - G + H)/2.0f;
	}

	void quaternionConjugate(float qw, float qx, float qy, float qz, float& result_w,float& result_x,float& result_y,float& result_z)
	{
		result_w = qw;
		result_x = -qx;
		result_y = -qy;
		result_z = -qz;
	}

	//i.e. this is governed by the equation: v' = q*v*conjugate(q)
	void applyQuaternionRotationToVector(float x,float y,float z, float qw,float qx, float qy, float qz, float& result_x, float& result_y, float& result_z)
	{
		float temp_qw,temp_qx,temp_qy,temp_qz;
		quaternionMultiplication(qw,qx,qy,qz,0,x,y,z,temp_qw,temp_qx,temp_qy,temp_qz);//First calculate q*v
		float conj_qw,conj_qx,conj_qy,conj_qz;
		quaternionConjugate(qw,qx,qy,qz,conj_qw,conj_qx,conj_qy,conj_qz);//Find conjugate(q)

		float discard = 0;
		//calculate (q*v)*conjugate(q)
		quaternionMultiplication(temp_qw,temp_qx,temp_qy,temp_qz,conj_qw,conj_qx,conj_qy,conj_qz,discard,result_x,result_y,result_z);
	}

	void updateCamera()
	{
		GLdouble cameraCenterX;
		GLdouble cameraCenterY;
		GLdouble cameraCenterZ;

		float pitch_qw, pitch_qx, pitch_qy, pitch_qz = 0.0f;
		convertAxisAngleToQuaternion(degreeToRadians(pitch),0,-1.0f,0,pitch_qw,pitch_qx,pitch_qy,pitch_qz);//Build a quaternion for pitching around Y axis
		float yaw_qw, yaw_qx, yaw_qy, yaw_qz = 0.0f;
		convertAxisAngleToQuaternion(degreeToRadians(yaw),0,0,1.0f,yaw_qw,yaw_qx,yaw_qy,yaw_qz);//Build a quaternion for yawing camera around Z axis
		float roll_qw, roll_qx, roll_qy, roll_qz = 0.0f;
		convertAxisAngleToQuaternion(degreeToRadians(roll),1.0f,0,0,roll_qw,roll_qx,roll_qy,roll_qz);//Build a quaternion for rolling camera around Z axis

		float temp_qw, temp_qx, temp_qy, temp_qz;//Calculate yaw * pitch
		quaternionMultiplication(yaw_qw,yaw_qx,yaw_qy,yaw_qz,pitch_qw,pitch_qx,pitch_qy,pitch_qz,temp_qw, temp_qx, temp_qy, temp_qz);
		float rotation_qw,rotation_qx,rotation_qy,rotation_qz;//Calculate (yaw*pitch) * roll, i.e. a quaternion representing the camera's orientation
		quaternionMultiplication(temp_qw, temp_qx, temp_qy, temp_qz,roll_qw, roll_qx, roll_qy, roll_qz,rotation_qw,rotation_qx,rotation_qy,rotation_qz);

		//apply the quaternion to the camera's "forward" vector
		applyQuaternionRotationToVector(DEFAULT_FORWARD_X,DEFAULT_FORWARD_Y,DEFAULT_FORWARD_Z,
				rotation_qw,rotation_qx,rotation_qy,rotation_qz,  forward_x,forward_y,forward_z);

		//apply the quaternion to the camera's "up" vector
		applyQuaternionRotationToVector(DEFAULT_UP_X,DEFAULT_UP_Y,DEFAULT_UP_Z,
				rotation_qw,rotation_qx,rotation_qy,rotation_qz,  up_x,up_y,up_z);

		//apply the quaternion to the camera's "right" vector
		applyQuaternionRotationToVector(DEFAULT_RIGHT_X,DEFAULT_RIGHT_Y,DEFAULT_RIGHT_Z,
				rotation_qw,rotation_qx,rotation_qy,rotation_qz,  right_x,right_y,right_z);

		cameraCenterX = x + forward_x;
		cameraCenterY = y + forward_y;
		cameraCenterZ = z + forward_z;

		gluLookAt (x,y,z,cameraCenterX,cameraCenterY,cameraCenterZ,up_x,up_y,up_z);
	}

	void turnLeft(double degrees)
	{
		yaw += degrees;
		if(yaw > 180)
			yaw -= 360;//prevents yaw from getting too big
	}

	void turnRight(double degrees)
	{
		yaw -= degrees;
		if(yaw < -180)
			yaw += 360;//prevents yaw from getting too small
	}

	void lookUp(double degrees)
	{
		pitch += degrees;
		if(pitch > 180)
			pitch -= 360;//prevents pitch from getting too big
	}

	void lookDown(double degrees)
	{
		pitch -= degrees;
		if(pitch < -180)
			pitch += 360;//prevents pitch from getting too small
	}

	void rollLeft(double degrees)
	{
		roll += degrees;
		if(roll > 180)
			roll -= 360;//prevents roll from getting too big
	}

	void rollRight(double degrees)
	{
		roll -= degrees;
		if(roll < -180)
			roll += 360;//prevents roll from getting too small
	}

	void moveForward(double distance)
	{
		x += forward_x * distance;
		y += forward_y * distance;
		z += forward_z * distance;
	}

	void moveBackward(double distance)
	{
		x += forward_x * -distance;
		y += forward_y * -distance;
		z += forward_z * -distance;
	}

	void panRight(double distance)
	{
		x += right_x * distance;
		y += right_y * distance;
		z += right_z * distance;
	}

	void panLeft(double distance)
	{
		x += right_x * -distance;
		y += right_y * -distance;
		z += right_z * -distance;
	}

	void panUp(double distance)
	{
		x += up_x * distance;
		y += up_y * distance;
		z += up_z * distance;
	}

	void panDown(double distance)
	{
		x += up_x * -distance;
		y += up_y * -distance;
		z += up_z * -distance;
	}

	void keyInput(unsigned char key)
	{
		switch (key)
		{
			case 'w': moveForward(moveSpeed);	break;
			case 's': moveBackward(moveSpeed);	break;
			case 'a': panLeft(moveSpeed);		break;
			case 'd': panRight(moveSpeed);		break;
			case ' ': panUp(moveSpeed);			break;
			case 'c': panDown(moveSpeed);		break;
		 }
	}

	void specialKeys(int key)
	{
		switch(key) 
		{
			case GLUT_KEY_LEFT:	turnLeft(lookSpeed);		break;
			case GLUT_KEY_RIGHT:turnRight(lookSpeed);		break;
			case GLUT_KEY_UP:	lookUp(lookSpeed);			break;
			case GLUT_KEY_DOWN: lookDown(lookSpeed);		break;
		}
	}
};

#endif