// Sample code for �bung 2

// Include-File für Windows creation
#include "windows.h"

#include "vec.h"
#include "mat.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
using namespace std;

// might be you have to swith to
// #include "glut.h" depending on your GLUT installation
#define GLUT_DISABLE_ATEXIT_HACK
#include "Uebung2/Uebung2/glut.h"

////////////////////////////////////////////////////////////
//
// system relevant global variables
//

// window width and height (choose an appropriate size)
const int g_iWidth  = 1200;
const int g_iHeight = 1200;

// global variable to tune the timer interval
int g_iTimerMSecs;

/////////////////////////////////////////////////////////////
CVec2f sun;
CVec2f planet;
CVec2f moon;

// General variables
float angle_planet;
float angle_moon;
float angle_increment_planet;
float angle_increment_moon;
/////////////////////////////////////////////////////////////

// function to initialize our own variables
void init () {
	// init timer interval
	g_iTimerMSecs = 10;

	// init variables for display1
	float sun_pos [2] = {0,0}; 
	sun.setData(sun_pos);
	float planet_pos [2] = {350,0}; 
	planet.setData(planet_pos);
	float moon_pos [2] = {470,0}; 
	moon.setData(moon_pos);

	// init variables for both
	angle_planet = 0;
	angle_moon = 0;
	angle_increment_planet = 0.01;
	angle_increment_moon = 0.04;
}

// function to initialize the view to ortho-projection
void initGL () {
	glViewport (0, 0, g_iWidth, g_iHeight);	// Establish viewing area to cover entire window.

	glMatrixMode (GL_PROJECTION);			// Start modifying the projection matrix.
	glLoadIdentity ();						// Reset project matrix.
	glOrtho (-g_iWidth/2, g_iWidth/2, -g_iHeight/2, g_iHeight/2, 0, 1);	// Map abstract coords directly to window coords.

	// tell GL that we draw to the back buffer and
	// swap buffers when image is ready to avoid flickering
	glDrawBuffer (GL_BACK);

	// tell which color to use to clear image
	glClearColor (0,0,0,1);
}

// Eine überaus primitive Farbklasse
class Color {
public:
	Color (float r=1.0f, float g=1.0f, float b=1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
	}
	float r, g, b;
};

CMat3f vecToMat (CVec2f vec) {
	float mat [3][3] = {
		{1,0,vec(0)},
		{0,1,vec(1)},
		{0,0,1}
	};
	return CMat3f(mat);
}

CMat3f rotMat (float theta) {
	float mat [3][3] = {
		{cos(theta)	,-sin(theta),0},
		{sin(theta)	,cos(theta)	,0},
		{0			,0			,1}
	};
	return CMat3f(mat);
}

void bhamCircle (CVec2i p, int r, Color c) {

	glBegin (GL_POINTS);
	glColor3f (c.r, c.g, c.b);

	int x, y, d, se, e;
	x = 0; y = r; d = 5 - (r << 2);

	glVertex2i(p(0),p(1) + r);
	glVertex2i(p(0),p(1) - r);
	glVertex2i(p(0) + r,p(1));
	glVertex2i(p(0) - r,p(1));
	
	while (y > x) {
		if (d >= 0) { se = (((x-y) << 1) + 5) << 2; d += se; x++; y--; }
		else { e = ((x << 1) + 3) << 2; d += e; x++; }

		// Pixel in Oktant.2 setzen und auf andere spiegeln
		glVertex2i(p(0) + x,p(1) + y);
		glVertex2i(p(0) - x,p(1) + y);
		glVertex2i(p(0) + x,p(1) - y);
		glVertex2i(p(0) - x,p(1) - y);
		glVertex2i(p(0) + y,p(1) + x);
		glVertex2i(p(0) - y,p(1) + x);
		glVertex2i(p(0) + y,p(1) - x);
		glVertex2i(p(0) - y,p(1) - x);
	}

	glEnd ();
}

CVec2f rotateOrigin(CVec2f pos, float theta) {
	float r [2][2] = {
		{cos(theta),-sin(theta)},
		{sin(theta),cos(theta)}
	};
	CMat2f rotation = CMat2f(r);
	
	return rotation * pos;
}

CVec2f rotateArbitrary(CVec2f source, CVec2f target, float theta) {
	float r [2][2] = {
		{cos(theta),-sin(theta)},
		{sin(theta),cos(theta)}
	};
	CMat2f rotation = CMat2f(r);

	return rotation * (source - target) + source;
}

CVec3f toHom(CVec2f source) {
	float src_data [3] = {source(0),source(1),1};
	CVec3f src;
	src.setData(src_data);
	return src;
}

CVec2f fromHom(CVec3f source) {
	float src_data [2] = {source(0),source(1)};
	CVec2f src;
	src.setData(src_data);
	return src;
}

CVec2f rotateMatrix(CVec2f source, CVec2f target, float theta) {
	CVec3f src = toHom(target);
	src = (vecToMat(source) * rotMat(theta) * vecToMat(-source)) * src;
	return fromHom(src);
}

// timer callback function
void timer (int value) {
	///////
	// update your variables here ...
	//
	angle_planet += angle_increment_planet;
	angle_moon += angle_increment_moon;
	//
	///////
	// the last two lines should always be
	glutPostRedisplay ();
	glutTimerFunc (g_iTimerMSecs, timer, 0);	// call timer for next iteration
}

// display callback function
void display1 (void) {
	CVec2f g_planet = rotateArbitrary(sun,planet,angle_planet);
	CVec2f g_moon = moon + (g_planet - planet);
	g_moon = rotateArbitrary(g_planet, g_moon, angle_moon);

	glClear (GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//
	CVec2i location;

	int pos_sun [2] = {(int)sun(0), (int)sun(1)};
	location.setData(pos_sun);
	Color color_sun (.97f,.78f,0);
	bhamCircle (location,100,color_sun);

	int pos_planet [2] = {(int)g_planet(0), (int)g_planet(1)};
	location.setData(pos_planet);

	Color color_planet (.35f,.71f,0);
	bhamCircle (location,50,color_planet);

	int pos_moon [2] = {(int)g_moon(0), (int)g_moon(1)};
	location.setData(pos_moon);
	Color color_moon (.7f,.7f,.7f);
	bhamCircle (location,30,color_moon);
	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

// display callback function
void display2 (void) {
	CVec2f g_planet = rotateMatrix(sun,planet,angle_planet);
	CVec2f g_moon = fromHom(vecToMat(g_planet - planet) * toHom(moon));
	g_moon = rotateMatrix(g_planet, g_moon, angle_moon);

	glClear (GL_COLOR_BUFFER_BIT);

	///////
	// display your data here ...
	//
	CVec2i location;

	int pos_sun [2] = {(int)sun(0), (int)sun(1)};
	location.setData(pos_sun);
	Color color_sun (.97f,.58f,.2f);
	bhamCircle (location,100,color_sun);

	int pos_planet [2] = {(int)g_planet(0), (int)g_planet(1)};
	location.setData(pos_planet);

	Color color_planet (.35f,.71f,0);
	bhamCircle (location,50,color_planet);

	int pos_moon [2] = {(int)g_moon(0), (int)g_moon(1)};
	location.setData(pos_moon);
	Color color_moon (.7f,.7f,.7f);
	bhamCircle (location,30,color_moon);
	//
	///////

	// In double buffer mode the last
	// two lines should alsways be
	glFlush ();
	glutSwapBuffers (); // swap front and back buffer
}

void keyboard (unsigned char key, int x, int y) 
{
	switch (key) {
		case 'q':
		case 'Q':
			exit (0); // quit program
			break;
		case '1':
			angle_moon += M_PI;
			angle_planet += M_PI;
			glutDisplayFunc (display1);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		case '2':
			angle_moon -= M_PI;
			angle_planet -= M_PI;
			glutDisplayFunc (display2);
			//glutPostRedisplay ();	// not needed since timer triggers redisplay
			break;
		default:
			// do nothing ...
			break;
	};
}

int main (int argc, char **argv) {
	glutInit (&argc, argv);
	// we have to use double buffer to avoid flickering
	// TODO: lookup "double buffer", what is it for, how is it used ...
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(800,800);
	glutCreateWindow ("Uebung 2");

	init ();	// init my variables first
	initGL ();	// init the GL (i.e. view settings, ...)

	// assign callbacks
	glutTimerFunc (10, timer, 0);
	glutKeyboardFunc (keyboard);
	glutDisplayFunc (display1);
	// you might want to add a resize function analog to
	// �bung1 using code similar to the initGL function ...

	// start main loop
	glutMainLoop ();

	return 0;
}
