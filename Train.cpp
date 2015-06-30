//  ========================================================================
//  COSC363: Computer Graphics (2015);  University of Canterbury.
//
//  FILE NAME: Train.cpp
//  See Lab02.pdf for details
//  ========================================================================

//#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath> 
#include <GL/freeglut.h>
#include "loadBMP.h"
#include "loadTGA.h"

#define GL_CLAMP_TO_EDGE 0x812F 

GLuint txId[9];
GLUquadric *q;    //Required for creating cylindrical objects
float ENGINETHETA = 180;
float engineCounter = 100;

float WAGONTHETA1 = 180;
float wagonCounter1 = 78;

float WAGONTHETA2 = 180;
float wagonCounter2 = 56;

float BARRIERTHETA = 180;
int barrierFlag = 0;
int barrierCounter = 0;

float HUMANTHETA = 20;
int humanFlag = 0;
int humanX = 300;
int humanY = 0;


float trainY = 0;

int CAM_HGT = 85;
int CAM_CTRL = 0;
int zoom = 5;

int parallelTrainX1 = 0;
float parallelTrainX2 = -300;




//-- Ground Plane --------------------------------------------------------
void floor()
{
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0};
	glColor4f(0.7, 0.7, 0.7, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);  //No specular reflections from the floor.

	//The floor is made up of several tiny squares on a 200x200 grid. Each square has a unit size.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	
	glBegin(GL_QUADS);	
	for(int i = -400; i < 400; i+=50)
	{
		for(int j = -400;  j < 400; j+=50)
		{
			glTexCoord2f(0., 0.);
			glVertex3f(i, 0.0, j);
			glTexCoord2f(1., 0.);
			glVertex3f(i, 0.0, j+50);
			glTexCoord2f(1., 1.);
			glVertex3f(i+50, 0.0, j+50);
			glTexCoord2f(0., 1.);
			glVertex3f(i+50, 0.0, j);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);  //Enable specular reflections for remaining objects
}

//------- Rail Track ----------------------------------------------------
// A single circular track of specified radius
void trackCorner(float radius)  
{
	float angle1,angle2, ca1,sa1, ca2,sa2;
	float x1,z1, x2,z2, x3,z3, x4,z4;  //four points of a quad
    float toRad=3.14159265/180.0;  //Conversion from degrees to radians

    glBegin(GL_QUADS);
	for(int i = 0; i < 180; i += 5)    //5 deg intervals
	{
		angle1 = i * toRad;       //Computation of angles, cos, sin etc
		angle2 = (i+5) * toRad;
		ca1=cos(angle1); ca2=cos(angle2);
		sa1=sin(angle1); sa2=sin(angle2);
		x1=(radius-0.5)*sa1; z1=(radius-0.5)*ca1; 
		x2=(radius+0.5)*sa1; z2=(radius+0.5)*ca1;
		x3=(radius+0.5)*sa2; z3=(radius+0.5)*ca2;
		x4=(radius-0.5)*sa2; z4=(radius-0.5)*ca2;

		glNormal3f(0., 1., 0.);       //Quad 1 facing up
		glVertex3f(x1, 1.0, z1);
		glVertex3f(x2, 1.0, z2);
		glVertex3f(x3, 1.0, z3);
		glVertex3f(x4, 1.0, z4); 

		glNormal3f(-sa1, 0.0, -ca1);   //Quad 2 facing inward
		glVertex3f(x1, 0.0, z1);
		glVertex3f(x1, 1.0, z1);
		glNormal3f(-sa2 ,0.0, -ca2);
		glVertex3f(x4, 1.0, z4);
		glVertex3f(x4, 0.0, z4);

		glNormal3f(sa1, 0.0, ca1);   //Quad 3 facing outward
		glVertex3f(x2, 1.0, z2);
		glVertex3f(x2, 0.0, z2);
		glNormal3f(sa2, 0.0, ca2);
		glVertex3f(x3, 0.0, z3);
		glVertex3f(x3, 1.0, z3);
	}
	glEnd();
}

//------- Rail Track ----------------------------------------------------
// A single circular track of specified radius
void trackStraight(int len)  
{

    glBegin(GL_QUADS);
	for(int i = 0; i <= len; i += 5)    //5 deg intervals
	{
		glNormal3f(0., 1., 0.);       //Quad 1 facing up
		glVertex3f(i+0, 1.0, 1.0);
		glVertex3f(i+5, 1.0, 1.0);
		glVertex3f(i+5, 1.0, 0.0);
		glVertex3f(i+0, 1.0, 0.0); 
		
		glNormal3f(0.0, 0.0, 1);      //Quad 2 facing inward
		glVertex3f(i+0, 0.0, 1.0);
		glVertex3f(i+5, 0.0, 1.0);
		glVertex3f(i+5, 1.0, 1.0);
		glVertex3f(i+0, 1.0, 1.0);		
		
		glNormal3f(0.0, 0.0, -1);      //Quad 2 facing outward
		glVertex3f(i+0, 1.0, 0.0);
		glVertex3f(i+5, 1.0, 0.0);
		glVertex3f(i+5, 0.0, 0.0);
		glVertex3f(i+0, 0.0, 0.0);	

	}
	glEnd();
}

//--------Base structure for the wagon including wheels----------------------------
void passengerbase()
{
	float xp[4] = {-27.0, -27.0, 27.0, 27.0};
	float zp[4] = {-5.0,    5.0, -5.0,  5.0};

	glColor4f(0.3, 0.3, 0.3, 1.0);
	glDisable(GL_TEXTURE_2D); 
    glPushMatrix();
      glTranslatef(0.0, 4.5, 0.0);
      glScalef(72.0, 1.0, 12.0);  
      glutSolidCube(1.0);
    glPopMatrix();

	//Wheel assembly:  4 per wagon
	for(int i = 0; i < 4; i++)
	{
		glPushMatrix();
		  glTranslatef(xp[i], 3., zp[i]);
		  glScalef(16.0, 2.0, 2.0);    
		  glutSolidCube(1.0);
		glPopMatrix();
	    glPushMatrix();
		  glTranslatef(xp[i]-6.0, 2.0, zp[i]);
		  gluDisk(q, 0.0, 2.0, 20, 2);
	    glPopMatrix();
	    glPushMatrix();
		  glTranslatef(xp[i]+6.0, 2.0, zp[i]);
		  gluDisk(q, 0.0, 2.0, 20, 2);
	    glPopMatrix();
	}
}

//--------------------------------------------------------------------------------
void passengerwagon()
{
	passengerbase();

	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[2]);

	//3 large polygons (front, back, top)
	glBegin(GL_QUADS);
	
	  glNormal3f(0.0, 0.0, 1.0);   //Facing +z (Front side)
	  glTexCoord2f(0., 0.);
	  glVertex3f(-35.0, 5.0, 6.0);
	  glTexCoord2f(1., 0.);	
	  glVertex3f(35.0, 5.0, 6.0);
	  glTexCoord2f(1., 0.34);	
	  glVertex3f(35.0, 17.0, 6.0);
	  glTexCoord2f(0., 0.34);	
	  glVertex3f(-35.0, 17.0, 6.0);

	  glNormal3f(0.0, 0.0, -1.0);   //Facing -z (Back side)
	  glTexCoord2f(0., 0.); //bottom left
	  glVertex3f(35.0, 5.0, -6.0);
	  glTexCoord2f(1., 0.); //bottom right
	  glVertex3f(-35.0, 5.0,-6.0);
	  glTexCoord2f(1., 0.34);	//top left
	  glVertex3f(-35.0, 17.0,-6.0);
	  glTexCoord2f(0., 0.34);	//bottom right
	  glVertex3f(35.0, 17.0, -6.0);

	  glNormal3f(0.0, 1.0, 0.0);   //Facing +y (Top side)
	  glTexCoord2f(0., 0.449);	 //bottom left
	  glVertex3f(-35.0, 17.0, 6.0);
	  glTexCoord2f(1., 0.449);	// bottom right
	  glVertex3f(35.0, 17.0,  6.0);
	  glTexCoord2f(1., 0.828); //top right
	  glVertex3f(35.0, 17.0, -6.0);
	  glTexCoord2f(0., 0.828); //top left
	  glVertex3f(-35.0, 17.0, -6.0);
	glEnd();

	//2 small side polygons (left, right)
	glColor4f(0., 0.0, 0.0, 0.0);
	glDisable(GL_TEXTURE_2D); 
	glBegin(GL_QUADS);
	  glNormal3f(-1.0, 0.0, 0.0);   //Facing -x (Left side)
	  glVertex3f(-35.0, 5.0, -6.0);
	  glVertex3f(-35.0, 5.0, 6.0);
	  glVertex3f(-35.0, 17.0, 6.0);
	  glVertex3f(-35.0, 17.0, -6.0);

	  glNormal3f(1.0, 0.0, 0.0);   //Facing +x (Right side)
	  glVertex3f(35.0, 5.0, 6.0);
	  glVertex3f(35.0, 5.0, -6.0);
	  glVertex3f(35.0, 17.0, -6.0);
	  glVertex3f(35.0, 17.0, 6.0);
	glEnd();
}
//-------- Tracks  ----------------------------------------------------
void tracks(int straightLen)
{
	//Straight parallel tracks
	glPushMatrix();
	 	glColor3f(0.0, 0.0, 0.0);
	 	glTranslatef(-400, 0, -80);
	 	trackStraight(straightLen);
	 	glTranslatef(0, 0, -10);
	 	trackStraight(straightLen);
		glTranslatef(0, 0, -40);
		trackStraight(straightLen);	
		glTranslatef(0, 0, -10);
		trackStraight(straightLen);	
	glPopMatrix();
	
	//Zero shaped tracs
	glPushMatrix();	
		glColor3f(0.0, 0.0, 0.0);
		glPushMatrix();
			glTranslatef(140, 0, 150);
			trackCorner(60.0);   
			trackCorner(70.0);   
		glPopMatrix();
		
		glPushMatrix();
			glTranslatef(-140, 0, 150);
			trackCorner(-60.0);   
			trackCorner(-70.0);   
		glPopMatrix();
			
		glPushMatrix();
			glTranslatef(-140, 0, 79.5);
			trackStraight(280);	
			glTranslatef(0, 0, 10);
			trackStraight(280);		
			glTranslatef(0, 0, 120);
			trackStraight(280);
			glTranslatef(0, 0, 10);
			trackStraight(280);
		glPopMatrix();
	glPopMatrix();
	
}

void trafficLight()
{   
	//Red Light Surrounder
	glPushMatrix();
	 glColor3f(0, 0, 0);
	 glTranslatef(-3.5, 23, 0);
	 glRotatef(90, 0 , 1, 0);
	 gluCylinder(q, 1.5, 1.5, 3, 20, 5);
	glPopMatrix();	
	
	//Red Light
	glPushMatrix();
	 glColor3f(0.9, 0.137, 0.137);
	 glTranslatef(-2.75, 23, 0);
	 glRotatef(90, 0 , 1, 0);
	 glutSolidCone(1.5, 3, 20, 5);
	glPopMatrix();

	//Green Light Surrounder
	glPushMatrix();
	 glColor3f(0, 0, 0);
	 glTranslatef(-3.5, 28, 0);
	 glRotatef(90, 0 , 1, 0);
	 gluCylinder(q, 1.5, 1.5, 3, 20, 5);
	glPopMatrix();	
	
	//Green Light
	glPushMatrix();
	 glColor3f(0.137, 0.9, 0.137);
	 glTranslatef(-2.75, 28, 0);
	 glRotatef(90, 0 , 1, 0);
	 glutSolidCone(1.5, 3, 20, 5);
	glPopMatrix();
	
	//Box
	glPushMatrix();
     glColor3f(0, 0, 0);
	 glTranslatef(0, 25, 0);
	 glScalef(5, 10, 5);
	 glutSolidCube(1.0);
	glPopMatrix();
	
	//Pole
	glPushMatrix();
	 glColor3f(0.5, 0.5, 0.5);
	 glTranslatef(0, 20, 0);
	 glRotatef(90, 1, 0, 0);
	 gluCylinder(q, 1.0, 1.0, 20, 20, 5);
	glPopMatrix();
}
//------- Base of engine, wagons (including wheels) --------------------
void base()
{
	
    glColor4f(0.2, 0.2, 0.2, 1.0);   //The base is nothing but a scaled cube!
    glPushMatrix();
		
		glPushMatrix();
		  glTranslatef(0.0, 4.0, 0.0);
		  glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
		  glutSolidCube(1.0);
		glPopMatrix();

		glPushMatrix();					//Connector between wagons
		  glTranslatef(11.0, 4.0, 0.0);
		  glutSolidCube(2.0);
		glPopMatrix();

		//Wheels
		glColor4f(0.5, 0., 0., 1.0);
		glPushMatrix();
		  glTranslatef(-8.0, 2.0, 5.1);
		  gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
		glPushMatrix();
		  glTranslatef(8.0, 2.0, 5.1);
		  gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
		glPushMatrix();
		  glTranslatef(-8.0, 2.0, -5.1);
		  glRotatef(180.0, 0., 1., 0.);
		  gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
		glPushMatrix();
		  glTranslatef(8.0, 2.0, -5.1);
		  glRotatef(180.0, 0., 1., 0.);
		  gluDisk(q, 0.0, 2.0, 20, 2);
		glPopMatrix();
	glPopMatrix();
}

//-- Locomotive ------------------------------------------------
void straightEngine()
{
		
	glPushMatrix();
		base();
		
		glPushMatrix();
			//Cab
			glColor4f(0.8, 0.8, 0.0, 1.0);
			glPushMatrix();
			  glTranslatef(7.0, 8.5, 0.0);
			  glScalef(6.0, 7.0, 10.0);
			  glutSolidCube(1.0);
			glPopMatrix();

			glPushMatrix();
			  glTranslatef(6.0, 14.0, 0.0);
			  glScalef(4.0, 4.0, 8.0);
			  glutSolidCube(1.0);
			glPopMatrix();

			//Boiler
			glPushMatrix();
			  glColor4f(0.5, 0., 0., 1.0);
			  glTranslatef(4.0, 10.0, 0.0);
			  glRotatef(-90.0, 0., 1., 0.);
			  gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
			  glTranslatef(0.0, 0.0, 14.0);
			  gluDisk(q, 0.0, 5.0, 20, 3);
			  glColor4f(1.0, 1.0, 0.0, 1.0);
			  glTranslatef(0.0, 4.0, 0.2);
			  gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

//-- Locomotive ------------------------------------------------
void engine()
{
		
	glPushMatrix();
		if (ENGINETHETA == 180){
			glTranslatef(engineCounter, 0, 0);
			glTranslatef(-140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(ENGINETHETA, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);	
		}
		else if (ENGINETHETA == 360){
			glTranslatef(engineCounter, 0, 0);
			glTranslatef(140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(ENGINETHETA, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);
		}
		else if (engineCounter == 1){
			glTranslatef(-140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(ENGINETHETA, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);			
		}
		else
		{
			glTranslatef(140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(ENGINETHETA, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);				
		}
		
				
		base();
		
		glPushMatrix();
			//Cab
			glColor4f(0.8, 0.8, 0.0, 1.0);
			glPushMatrix();
			  glTranslatef(7.0, 8.5, 0.0);
			  glScalef(6.0, 7.0, 10.0);
			  glutSolidCube(1.0);
			glPopMatrix();

			glPushMatrix();
			  glTranslatef(6.0, 14.0, 0.0);
			  glScalef(4.0, 4.0, 8.0);
			  glutSolidCube(1.0);
			glPopMatrix();

			//Boiler
			glPushMatrix();
			  glColor4f(0.5, 0., 0., 1.0);
			  glTranslatef(4.0, 10.0, 0.0);
			  glRotatef(-90.0, 0., 1., 0.);
			  gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
			  glTranslatef(0.0, 0.0, 14.0);
			  gluDisk(q, 0.0, 5.0, 20, 3);
			  glColor4f(1.0, 1.0, 0.0, 1.0);
			  glTranslatef(0.0, 4.0, 0.2);
			  gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}


//--- A rail wagon ---------------------------------------------------
void straightWagon()
{
	glPushMatrix();
		base();
		glPushMatrix();
			glColor4f(0.0, 1.0, 1.0, 1.0);
			glPushMatrix();
			  glTranslatef(0.0, 10.0, 0.0);
			  glScalef(18.0, 10.0, 10.0);
			  glutSolidCube(1.0);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

void wagon(float wagonTheta, float wagonCounter)
{
	glPushMatrix();
		if (wagonTheta == 180){
			glTranslatef(wagonCounter, 0, 0);
			glTranslatef(-140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(wagonTheta, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);	
		}
		else if (wagonTheta == 360){
			glTranslatef(wagonCounter, 0, 0);
			glTranslatef(140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(wagonTheta, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);
		}
		else if (wagonCounter == 1){
			glTranslatef(-140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(wagonTheta, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);			
		}
		else
		{
			glTranslatef(140, 0, 85);
			glTranslatef(0.0, 0.0, 65);
			glRotatef(wagonTheta, 0.0, 1.0, 0.0); 
			glTranslatef(0.0, 1.0, -65);				
		}
		base();
		glPushMatrix();
			glColor4f(0.0, 1.0, 1.0, 1.0);
			glPushMatrix();
			  glTranslatef(0.0, 10.0, 0.0);
			  glScalef(18.0, 10.0, 10.0);
			  glutSolidCube(1.0);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}


void barrierArm()
{
	//Arm
	glPushMatrix();
	
	
	 glTranslatef(0, 13, 0);
	 glRotatef(BARRIERTHETA, 1, 0, 0);
	 
	 glTranslatef(0, 0, 20);
	 glRotatef(90, 1, 0, 0);
	 
	 
	 
	 //Red Strips
	 int stripPos = -18;
	 for (int i = 0; i < 7; i++)
	 {
		 glPushMatrix();
			 glTranslatef(0, -stripPos, 0);
			 glColor3f(1, 0, 0);
			 glScalef(2.001,3,2.001);
			 glutSolidCube(1.0);
		 glPopMatrix();
		 stripPos += 6; 
	 }
	 
     glPushMatrix();
		 glColor3f(1, 1, 1);
		 glScalef(2, 40, 2);
		 glutSolidCube(1.0);
	 glPopMatrix();
	glPopMatrix();
	
	//Base
	glPushMatrix();
	 glColor3f(255, 0.886, 0);
	 glTranslatef(0, 7.5, 0);
	 glScalef(4, 15, 4);
	 glutSolidCube(1.0);
	glPopMatrix();
}

void column()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glColor3f(1.0 ,1.0, 1.0);
	
	glBegin(GL_QUADS);
		//Quad 1 face z+
		glTexCoord2f(0., 0.);
		glVertex3f(-2.5, -17, 2.5);
		glTexCoord2f(0.25, 0.);
		glVertex3f(2.5, -17, 2.5);
		glTexCoord2f(0.25, 1.);
		glVertex3f(2.5, 17, 2.5);
		glTexCoord2f(0., 1.);
		glVertex3f(-2.5, 17, 2.5); 
		
		//Quad 3 face x+
		glTexCoord2f(0.25, 0.);
		glVertex3f(2.5, -17, 2.5);
		glTexCoord2f(0.5, 0.);
		glVertex3f(2.5, -17, -2.5);
		glTexCoord2f(0.5, 1.);
		glVertex3f(2.5, 17, -2.5);
		glTexCoord2f(0.25, 1.);
		glVertex3f(2.5, 17, 2.5); 
		
		//Quad 2 face z-
		glTexCoord2f(0.5, 0.);
		glVertex3f(2.5, -17, -2.5);
		glTexCoord2f(0.75, 0.);
		glVertex3f(-2.5, -17, -2.5);
		glTexCoord2f(0.75, 1.);
		glVertex3f(-2.5, 17, -2.5);
		glTexCoord2f(0.5, 1.);
		glVertex3f(2.5, 17, -2.5); 
		
		//Quad 4 face x-
		glTexCoord2f(0.75, 0.);
		glVertex3f(-2.5, -17, -2.5);
		glTexCoord2f(1., 0.);
		glVertex3f(-2.5, -17, 2.5);
		glTexCoord2f(1., 1.);
		glVertex3f(-2.5, 17, 2.5);
		glTexCoord2f(0.75, 1.);
		glVertex3f(-2.5, 17, -2.5); 
	glEnd();
	glDisable(GL_TEXTURE_2D); 
}

void skyscraper(float x, float y, float z)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[8]);
	glColor3f(1.0 ,1.0, 1.0);
	
	glPushMatrix();
	 glTranslatef(0, y + 5, 0); 
	 glRotatef(270, 1, 0, 0);
	 glutSolidCone(1, 50, 20, 5);
	glPopMatrix();
	
	glPushMatrix();
	 glTranslatef(0, y + 2.5, 0);
	 glScalef(x, 5, z);
	 glutSolidCube(1.0);
	glPopMatrix();
	glBegin(GL_QUADS);
		//Quad 1 face z+
		glTexCoord2f(0., 0.);
		glVertex3f(-x, -y, z);
		glTexCoord2f(1, 0.);
		glVertex3f(x, -y, z);
		glTexCoord2f(1, 1.);
		glVertex3f(x, y, z);
		glTexCoord2f(0., 1.);
		glVertex3f(-x, y, z); 
		
		//Quad 3 face x+
		glTexCoord2f(0., 0.);
		glVertex3f(x, -y, z);
		glTexCoord2f(1, 0.);
		glVertex3f(x, -y, -z);
		glTexCoord2f(1, 1.);
		glVertex3f(x, y, -z);
		glTexCoord2f(0., 1.);
		glVertex3f(x, y, z); 
		
		//Quad 2 face z-
		glTexCoord2f(0., 0.);
		glVertex3f(x, -y, -z);
		glTexCoord2f(1, 0.);
		glVertex3f(-x, -y, -z);
		glTexCoord2f(1, 1.);
		glVertex3f(-x, y, -z);
		glTexCoord2f(0., 1.);
		glVertex3f(x, y, -z); 
		
		//Quad 4 face x-
		glTexCoord2f(0., 0.);
		glVertex3f(-x, -y, -z);
		glTexCoord2f(1, 0.);
		glVertex3f(-x, -y, z);
		glTexCoord2f(1, 1.);
		glVertex3f(-x, y, z);
		glTexCoord2f(0., 1.);
		glVertex3f(-x, y, -z); 
		
		//Quad 5 face y+
		glVertex3f(-x, y, -z);
		glVertex3f(x, y, -z);
		glVertex3f(x, y, z);
		glVertex3f(-x, y, z); 
	glEnd();
	glDisable(GL_TEXTURE_2D); 
}
//---- A Train Station ------------------------------------------------
void station()
{
	glPushMatrix();						//Traffic light
	  glTranslatef(0, 0, -100);
	  trafficLight();
	glPopMatrix();
	
	glPushMatrix();
	  glTranslatef(60, 20, -50);		//Column 4
	  column();
	glPopMatrix();
	
	glPushMatrix();
	  glTranslatef(-60, 20, -50);		//Column 3   
	  column();
	glPopMatrix();

	glPushMatrix();
	  glTranslatef(-60, 20, 50);		//Column 2
	  column();
	glPopMatrix();
		
	glPushMatrix();
	  glTranslatef(60, 20, 50);		//Column 1
	  column();
	glPopMatrix();	

	glPushMatrix(); 
	  glColor3f(169.0/255, 162.0/255, 162.0/255);
	  glTranslatef(0.0, 39.0, 0.0);		//Roof part 1
	  glScalef(130.0, 4.0, 110.0);     
	  glutSolidCube(1.0);
	glPopMatrix();	

	glPushMatrix();
	  glColor3f(169.0/255, 162.0/255, 162.0/255);
	  glTranslatef(0.0, 0.5, 0.0);
	  glScalef(170.0, 1.0, 90.0); 		//Stairs 1    
	  glutSolidCube(1.0);
	glPopMatrix();	
	
	glPushMatrix();
	  glColor3f(169.0/255, 162.0/255, 162.0/255);
	  glTranslatef(0.0, 1.5, 0.0);
	  glScalef(160.0, 1.0, 90.0);       //Stars 2
	  glutSolidCube(1.0);
	glPopMatrix();	
	
	glPushMatrix();
	  glColor4f(0.5, 0., 0., 1.0);
	  glTranslatef(0.0, 21, 0.0);		//Station
	  glScalef(100.0, 34.0, 60.0);     
	  glutSolidCube(1.0);
	glPopMatrix();
	
	glPushMatrix();
	  glColor3f(169.0/255, 162.0/255, 162.0/255);
	  glTranslatef(0.0, 1.5, 0);	
	  glScalef(150, 3, 140);			//Base
	  glutSolidCube(1.0);
	glPopMatrix();
}

//---------------------------------------------------------------------
//--Draws a character model constructed using GLUT object --------------
void drawHuman(int theta)
{
	glColor3f(1., 0.78, 0.06);		//Head
	glPushMatrix();
	  glTranslatef(0, 7.7, 0);
	  glutSolidCube(1.4);
	glPopMatrix();

	glColor3f(1., 0., 0.);			//Torso
	glPushMatrix();
	  glTranslatef(0, 5.5, 0);
	  glScalef(3, 3, 1.4);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Right leg
	glPushMatrix();
	  glTranslatef(0.8, 4, 0);
	  glRotatef(-theta, 1, 0, 0);
	  glTranslatef(-0.8, -4, 0);
	  glTranslatef(-0.8, 2.2, 0);
	  glScalef(1, 4.4, 1);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Left leg
	glPushMatrix();
	  glTranslatef(0.8, 4, 0);
	  glRotatef(theta, 1, 0, 0);
	  glTranslatef(-0.8, -4, 0);
	  glTranslatef(0.8, 2.2, 0);
	  glScalef(1, 4.4, 1);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Right arm
	glPushMatrix();
	  glTranslatef(-2, 6.5, 0);
	  glRotatef(theta, 1, 0, 0);
	  glTranslatef(2, -6.5, 0);
	  glTranslatef(-2, 5, 0);
	  glScalef(1, 4, 1);
	  glutSolidCube(1);
	glPopMatrix();

	glColor3f(0., 0., 1.);			//Left arm
	glPushMatrix();
	  glTranslatef(-2, 6.5, 0);
	  glRotatef(-theta, 1, 0, 0);
	  glTranslatef(2, -6.5, 0);
	  glTranslatef(2, 5, 0);
	  glScalef(1, 4, 1);
	  glutSolidCube(1);
	glPopMatrix();
}

void loadTexture()				
{
	glGenTextures(9, txId); 				// Create a Texture object
	
	glBindTexture(GL_TEXTURE_2D, txId[0]);		//Use this texture
    loadBMP("BrickTexture.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glBindTexture(GL_TEXTURE_2D, txId[1]);		//Use this texture
    loadTGA("CementaryDN.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glBindTexture(GL_TEXTURE_2D, txId[2]);		//Use this texture
    loadBMP("WagonTexture.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	// *** left ***
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	loadTGA("CementaryLT.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** front ***
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	loadTGA("CementaryFT.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** right ***
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	loadTGA("CementaryRT.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** back***
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	loadTGA("CementaryBK.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** top ***
	glBindTexture(GL_TEXTURE_2D, txId[7]);
	loadTGA("CementaryUP.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	
	//skyscraper
	glBindTexture(GL_TEXTURE_2D, txId[8]);
	loadBMP("BuildingTexture.bmp");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

}

//---------------------------------------------------------------------
void initialize(void) 
{
    //float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float whiteAmb[4] = {0.3, 0.3, 0.3, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    float green[4] = {0.0, 1.0, 0.0, 1.0};
    float red[4] = {1.0, 0.0, 0.0, 1.0};

	q = gluNewQuadric();
	loadTexture();
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, whiteAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, red);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, red);
    glLightfv(GL_LIGHT1, GL_SPECULAR, red);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0);
	
	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, green);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, green);
    glLightfv(GL_LIGHT2, GL_SPECULAR, green);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 0);
	

	
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    gluQuadricDrawStyle(q, GLU_FILL);
	glClearColor (0.0, 0.0, 0.0, 0.0);  //Background colour

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60., 1.0, 10.0, 1000.0);   //Perspective projection
}

void special (int key, int x, int y)
{
	if (key == GLUT_KEY_UP && CAM_HGT < 85)
	{
		CAM_HGT += 5;
	}
	else if (key == GLUT_KEY_DOWN && CAM_HGT > 5)
	{
		CAM_HGT -= 5;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		CAM_CTRL -= 4;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		CAM_CTRL += 4;
	}
	else if (key == GLUT_KEY_PAGE_UP and zoom > 3)
	{
		zoom -= 1;
	}
	else if (key == GLUT_KEY_PAGE_DOWN && zoom < 7)
	{
		zoom += 1;
	}
	glutPostRedisplay();

}

void human()
{
	glPushMatrix();
	 glTranslatef(-1, 3, 50);
	 drawHuman(0);
	glPopMatrix();	
	glPushMatrix();
	 glTranslatef(0, 3, 55);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	 glTranslatef(1, 3, 60);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	 glTranslatef(0, 3, 65);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	 glTranslatef(51.5, 3, -10);
	 glRotatef(90, 0, 1, 0);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	 glTranslatef(53, 3, 20);
	 glRotatef(-160, 0, 1, 0);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	 glTranslatef(53, 3, 25);
	 glRotatef(160, 0, 1, 0);
	 drawHuman(0);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(humanX, humanY, 0);
	  glTranslatef(0, 0, 35);
	  glRotatef(90, 0, 1, 0);
	  drawHuman(HUMANTHETA);
	glPopMatrix();
}

void tunnel()
{
	//disc back
	glPushMatrix();
	 glTranslatef(0, 0, 80);
	 glColor3f(0, 0, 0);
	 gluDisk(q, 0, 30, 20, 3);
	glPopMatrix();	
	
	//disc front
	glPushMatrix();
	 glColor3f(0.27, 0.176, 0.055);
	 gluDisk(q, 25, 30, 20, 3);
	glPopMatrix();
	
	//cylinder
	glPushMatrix();
	 glColor3f(0.27, 0.176, 0.055);
	 gluCylinder(q, 30, 30, 80, 20, 5);
	glPopMatrix();
}

void parallelTrain1()
{
   glPushMatrix();
    glTranslatef(parallelTrainX1, 0, 0);
    glTranslatef(-100, 0, -135);
	straightEngine();
	glTranslatef(22, 0, 0);
	straightWagon();
	glTranslatef(22, 0, 0);
	straightWagon();
	glTranslatef(22, 0, 0);
	straightWagon();
   glPopMatrix();
}	
	
void parallelTrain2()
{
   glPushMatrix();
    glTranslatef(parallelTrainX2, 0, 0);
    glTranslatef(-100, 0, -85);
	
	glTranslatef(-72, 0, 0);
	passengerwagon();
	glTranslatef(-72, 0, 0);
	passengerwagon();
	glTranslatef(120, 0, 0);
	glRotatef(180, 0, 1, 0);
	straightEngine();

   glPopMatrix();	
}	

void tunnels()
{
	
	glPushMatrix();
	 glTranslatef(319.9, 0, -110);
	 glRotatef(90, 0, 1, 0);
	 glScalef(2,1.5,1);
	 tunnel();
	glPopMatrix();
	
	glPushMatrix();
	 glTranslatef(-319.9, 0, -110);
	 glRotatef(270, 0, 1, 0);
	 glScalef(2,1.5,1);
	 tunnel();
	glPopMatrix();	
}

//========================================================================================

void skybox(){
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0 ,1.0, 1.0);

  ////////////////////// LEFT WALL ///////////////////////
  glBindTexture(GL_TEXTURE_2D, txId[5]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.);
  glVertex3f(-400,  -100, 400);
  glTexCoord2f(1., 0.);
  glVertex3f(-400, -100., -400);
  glTexCoord2f(1., 1.);
  glVertex3f(-400, 300., -400);
  glTexCoord2f(0., 1.);
  glVertex3f(-400, 300, 400);
  glEnd();

  ////////////////////// FRONT WALL ///////////////////////
  glBindTexture(GL_TEXTURE_2D, txId[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.);
  glVertex3f(-400,  -100, -400);
  glTexCoord2f(1., 0.);
  glVertex3f(400, -100., -400);
  glTexCoord2f(1., 1.);
  glVertex3f(400, 300, -400);
   glTexCoord2f(0., 1.);
  glVertex3f(-400,  300, -400);
  glEnd();

 ////////////////////// RIGHT WALL ///////////////////////
  glBindTexture(GL_TEXTURE_2D, txId[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.);
  glVertex3f(400,  -100, -400);
  glTexCoord2f(1., 0.);
  glVertex3f(400, -100, 400);
  glTexCoord2f(1., 1.);
  glVertex3f(400, 300,  400);
   glTexCoord2f(0., 1.);
  glVertex3f(400,  300,  -400);
  glEnd();


  ////////////////////// REAR WALL ////////////////////////
  glBindTexture(GL_TEXTURE_2D, txId[6]);
  glBegin(GL_QUADS);
  glTexCoord2f(0., 0.);
  glVertex3f( 400, -100, 400);
  glTexCoord2f(1., 0.);
  glVertex3f(-400, -100,  400);
  glTexCoord2f(1., 1.);
  glVertex3f(-400, 300,  400);
  glTexCoord2f(0., 1.);
  glVertex3f( 400, 300, 400);
  glEnd();
  
  /////////////////////// TOP //////////////////////////
  glBindTexture(GL_TEXTURE_2D, txId[7]);
  glBegin(GL_QUADS);
  glTexCoord2f(1., 0.);
  glVertex3f(-400, 300, -400);
  glTexCoord2f(1., 1.);
  glVertex3f(400, 300,  -400);
  glTexCoord2f(0., 1.);
  glVertex3f(400, 300,  400);
   glTexCoord2f(0., 0.);
  glVertex3f(-400, 300, 400);
  glEnd();
  
}


//-------------------------------------------------------------------
void display(void)
{
   float lgt_pos[] = {0.0f, 380.0f, 0.0f, 1.0f};  //light0 position (directly above the origin)
   
   float redlgt_pos[] = {-2.75f, 23.0f, -100.0f, 1.0};
   float redspotdir[]={-1.0, -1.0, 0};
   float greenlgt_pos2[] = {-2.75f, 28.0f, -100.0f, 1.0};
   float greenspotdir2[]={-1.0, -1.0, 0};
   
   float xpos, zpos;
   float radConv=3.14159265/180.0;	//Conversion from degrees to radians
   glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   	xpos = -100.0*sin(CAM_CTRL*radConv);
	zpos = -100.0*cos(CAM_CTRL*radConv);
   
   gluLookAt (xpos * zoom/2, CAM_HGT * zoom/2, zpos * zoom/2, 0.0, 20, 0.0, 0.0, 1.0, 0.0);
   //gluLookAt (0, CAM_HGT, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
   glLightfv(GL_LIGHT0, GL_POSITION, lgt_pos);   //light position
   glLightfv(GL_LIGHT1, GL_POSITION, redlgt_pos);
   glLightfv(GL_LIGHT2, GL_POSITION, greenlgt_pos2);
   
   
   skybox();
   floor();
   tracks(800);  
   station();
   human();
   tunnels();
   glPushMatrix();
    glTranslatef(130, 0, -70);
    barrierArm();
   glPopMatrix();
   glPushMatrix();
    glTranslatef(0, 50, -180);
    skyscraper(22, 100, 22);
   glPopMatrix();
   
   //Parallel train #1
	parallelTrain1();
   
   //Parallel train #2
	parallelTrain2();
	
    glLightfv(GL_LIGHT1, GL_POSITION, redlgt_pos);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, redspotdir);
	glLightfv(GL_LIGHT2, GL_POSITION, greenlgt_pos2);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, greenspotdir2);      		
    
	engine();
	wagon(WAGONTHETA1, wagonCounter1);
	wagon(WAGONTHETA2, wagonCounter2);
   

   glutSwapBuffers();   //Useful for animation
}

void myTimer(int value)
{
	if ((ENGINETHETA > 0 && ENGINETHETA < 180) || (ENGINETHETA > 180 && ENGINETHETA < 360)){
		ENGINETHETA++;
	}
	else
	{
		if (engineCounter < 280 && engineCounter > 0)
		{
			engineCounter += 1;
		}
		else if (engineCounter <0 && engineCounter > -280){
			engineCounter -= 1;
		}
		else
		{
			if (engineCounter == 280)
			{
				engineCounter = -1;
			}
			else
			{
				engineCounter = 1;
			}
			
			if (ENGINETHETA == 180)
			{
				ENGINETHETA = 181;
			}
			else
			{
				ENGINETHETA = 1;
			}
		}		
	}
	
	if ((WAGONTHETA1 > 0 && WAGONTHETA1 < 180) || (WAGONTHETA1 > 180 && WAGONTHETA1 < 360)){
		WAGONTHETA1++;
	}
	else
	{
		if (wagonCounter1 < 280 && wagonCounter1 > 0)
		{
			wagonCounter1 += 1;
		}
		else if (wagonCounter1 <0 && wagonCounter1 > -280){
			wagonCounter1 -= 1;
		}
		else
		{
			if (wagonCounter1 == 280)
			{
				wagonCounter1 = -1;
			}
			else
			{
				wagonCounter1 = 1;
			}
			
			if (WAGONTHETA1 == 180)
			{
				WAGONTHETA1 = 181;
			}
			else
			{
				WAGONTHETA1 = 1;
			}
		}		
	}
	
	if ((WAGONTHETA2 > 0 && WAGONTHETA2 < 180) || (WAGONTHETA2 > 180 && WAGONTHETA2 < 360)){
		WAGONTHETA2++;
	}
	else
	{
		if (wagonCounter2 < 280 && wagonCounter2 > 0)
		{
			wagonCounter2 += 1;
		}
		else if (wagonCounter2 <0 && wagonCounter2 > -280){
			wagonCounter2 -= 1;
		}
		else
		{
			if (wagonCounter2 == 280)
			{
				wagonCounter2 = -1;
			}
			else
			{
				wagonCounter2 = 1;
			}
			
			if (WAGONTHETA2 == 180)
			{
				WAGONTHETA2 = 181;
			}
			else
			{
				WAGONTHETA2 = 1;
			}
		}		
	}
	
	if (humanFlag == 0)
	{
		HUMANTHETA -= 5;
		if (HUMANTHETA == -30)
		{
			humanFlag = 1;
		}
	}
	else if (humanFlag == 1)
	{
		HUMANTHETA += 5;
		if (HUMANTHETA == 30)
		{
			humanFlag = 0;
		}
	}
	
	if (humanX == 85 || humanX == -80)
	{
		humanY = 1;
	}
	else if (humanX == 80 || humanX == -75)
	{
		humanY = 2;
	}
	else if (humanX == 75 || humanX == -70)
	{
		humanY = 3;
	}
	else if (humanX == 90 || humanX == -85)
	{
		humanY = 0;
	}
	
	if (humanX == -500) 
	{
		humanX = 400;
	}	
	
	if (parallelTrainX1 == -600)
	{
		parallelTrainX1 = 600;
	}
	
	if (parallelTrainX2 == 800)
	{
		parallelTrainX2 = -800;
	}
	

	if (parallelTrainX2 >= -90 && parallelTrainX2  <= 90)
	{
		BARRIERTHETA += 1;
	}
	else if (parallelTrainX2 >= 430 && parallelTrainX2  <= 610)
	{
		BARRIERTHETA -= 1;
	}
		
	if (BARRIERTHETA >= 270)
	{
		//Green Light
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}
	else
	{
		//Red Light
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT1);
	}		
	
	
	
	
	parallelTrainX1 -= 2;
	parallelTrainX2 += 2;
	
	
	humanX -= 1;
	glutPostRedisplay();
	glutTimerFunc(10, myTimer, 0);
}

//---------------------------------------------------------------------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE|GLUT_DEPTH);
   glutInitWindowSize (600, 600); 
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("Faversham Express");
   initialize ();
   glutSpecialFunc(special);
   glutDisplayFunc(display); 
   glutTimerFunc(50, myTimer, 0);
   glutMainLoop();
   
   return 0;
}
