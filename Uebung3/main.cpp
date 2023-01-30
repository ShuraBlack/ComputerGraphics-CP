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
#include "Uebung3/Uebung3/glut.h"

////////////////////////////////////////////////////////////
// Pre init
void reset();
////////////////////////////////////////////////////////////
// System relevant
const int g_iWidth = 600;
const int g_iHeight = 600;
int g_iTimerMSecs;
////////////////////////////////////////////////////////////
// Coordinate System
float focus;
CVec4f eyePoint;
CVec4f viewDir;
CVec4f viewUp;
/////////////////////////////////////////////////////////////
// Init functions
void init() {
	g_iTimerMSecs = 10;
	reset();
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(g_iTimerMSecs, timer, 0);
}

void initGL() {
	glViewport(0, 0, g_iWidth, g_iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-g_iWidth / 2, g_iWidth / 2, -g_iHeight / 2, g_iHeight / 2, 0, 1);

	glDrawBuffer(GL_BACK);

	glClearColor(0, 0, 0, 1);
}

////////////////////////////////////////////////////////////
// Primitiv Structs
class Color {
public:
	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	float r, g, b;
};
////////////////////////////////////////////////////////////
// Drawing
void bhamLine(CVec2i p1, CVec2i p2, Color c) {
	glBegin(GL_POINTS);
	glColor3f(c.r, c.g, c.b);

	glVertex2i(p1(0), p1(1));

	int x = 0, y = 0;
	int realX = p1(0), realY = p1(1);

	int dx = abs(p2(0) - p1(0));
	int dy = abs(p2(1) - p1(1));
	int Calcdx = dx, Calcdy = dy;

	if (dx < dy) { int tempdx = Calcdx; Calcdx = Calcdy; Calcdy = tempdx; }

	int dNE = 2 * (Calcdy - Calcdx);
	int dE = 2 * Calcdy;

	int d = 2 * Calcdy - Calcdx;

	while ((realX != p2(0) || realY != p2(1))) {
		if (d >= 0) { d += dNE; x++; y++; }
		else { d += dE; x++; }

		if (dx < dy) { realX = y; realY = x; }
		else { realX = x; realY = y; }

		if ((p2(0) - p1(0)) < 0) realX *= -1;
		if ((p2(1) - p1(1)) < 0) realY *= -1;
		realX += p1(0);
		realY += p1(1);

		glVertex2i(realX, realY);
	}

	glVertex2i(p2(0), p2(1));

	glEnd();
}

////////////////////////////////////////////////////////////
// Helper
float normalize(CVec4f v) {
	return sqrt(v(0) * v(0) + v(1) * v(1) + v(2) * v(2));
}

CVec4f product(CVec4f v1, CVec4f v2) {
	return CVec4f(
		new float[4] {
			v1(1) * v2(2) - v1(2) * v2(1),
			v1(2) * v2(0) - v1(0) * v2(2),
			v1(0) * v2(1) - v1(1) * v2(0), 
			0
		}
	);
}

CVec4f xAxis() {
	return product(viewUp, viewDir);
}
////////////////////////////////////////////////////////////

/* 
computes the 4x4- transformation-matrix to converts view coordinates to world coordinates.
The inverse of this matrix transforms world coordinates to view coordinates 
*/
CMat4f getTransform(CVec4f _viewOrigin, CVec4f _viewDir, CVec4f _viewUp) {
	CVec4f r3 = _viewDir;
	r3(0) /= normalize(_viewDir);
	r3(1) /= normalize(_viewDir);
	r3(2) /= normalize(_viewDir);
	r3(3) = 0;
	CVec4f r1 = xAxis();
	r1(0) /= normalize(xAxis());
	r1(1) /= normalize(xAxis());
	r1(2) /= normalize(xAxis());
	r1(3) = 0;
	CVec4f r2 = _viewUp;
	r2(0) /= normalize(_viewUp);
	r2(1) /= normalize(_viewUp);
	r2(2) /= normalize(_viewUp);
	r2(3) = 0;

	CMat4f rotationT = new float[4][4] {
		{r1(0),r1(1),r1(2),0},
		{r2(0),r2(1),r2(2),0},
		{r3(0),r3(1),r3(2),0},
		{   0 ,   0 ,   0 ,1}
	};

	CVec4f transform = rotationT * _viewOrigin;
	return new float[4][4] {
 		{r1(0),r1(1),r1(2),transform(0)},
		{r2(0),r2(1),r2(2),transform(1)},
		{r3(0),r3(1),r3(2),transform(2)},
		{	 0,	   0,    0,     	  1} 
	};
}

////////////////////////////////////////////////////////////
// Projection
CVec4f projectZ(float fFocus, CVec4f pView) {
	CMat4f proj(
		new float[4][4] {
			{1, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 1 / -fFocus, 1}
		}
	);

	CVec4f tmp (proj * pView);
	tmp(0) /= tmp(3);
	tmp(1) /= tmp(3);
	tmp(2) = 0;
	tmp(3) = 1;

	return tmp;
}

/*
transforms the point pWorld in world coordinates via matTransf to view coordinates and 
projects it onto the image plane using projectZ
*/
CVec4f projectZallg(CMat4f transformation, float fFocus, CVec4f pWorld) {
	return projectZ(fFocus, transformation * pWorld);
}

CVec4f toHom(CVec3f v) {
	return new float[4]{v(0),v(1),v(2),1};
}

CVec3f fromHom(CVec4f v) {
	return new float[3]{v(0),v(1),v(2)};
}

void drawProjektedZ(CVec3f Points[8], Color c) {
	CVec2i p[8];
	for (int i = 0; i < 8; i++) {
		p[i] = (new int[2]{ (int)Points[i](0), (int)Points[i](1) });
	}

	bhamLine(p[0], p[1], c);
	bhamLine(p[1], p[2], c);
	bhamLine(p[2], p[3], c);
	bhamLine(p[3], p[0], c);

	bhamLine(p[4], p[5], c);
	bhamLine(p[5], p[6], c);
	bhamLine(p[6], p[7], c);
	bhamLine(p[7], p[4], c);

	bhamLine(p[0], p[4], c);
	bhamLine(p[1], p[5], c);
	bhamLine(p[2], p[6], c);
	bhamLine(p[3], p[7], c);
}

void drawQuader(CVec3f Cuboid[8], float fFocus, Color c) {

	CVec3f points[8];
	CMat4f M = getTransform(eyePoint, viewDir, viewUp);

	for (int i = 0 ; i < 8 ; i++) {
		CVec4f hom = toHom(Cuboid[i]);
		CVec4f proj = projectZallg(M, fFocus, hom);
		points[i] = fromHom(proj);
	}

	drawProjektedZ(points, c);
}
////////////////////////////////////////////////////////////
// display callback function
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	CVec3f cub[8];

	cub[0] = (new float[3]{ 40,10,-10 }); // links unten
	cub[1] = (new float[3]{ 40,70,-10 }); // links oben
	cub[2] = (new float[3]{ 90,70,-10 }); // rechts oben
	cub[3] = (new float[3]{ 90,10,-10 }); // rechts unten
	cub[4] = (new float[3]{ 40,10,-30 }); // links unten
	cub[5] = (new float[3]{ 40,70,-30 }); // links oben
	cub[6] = (new float[3]{ 90,70,-30 }); // rechts oben
	cub[7] = (new float[3]{ 90,10,-30 }); // rechts unten
	drawQuader(cub, focus, Color(1.0f,.35f,.35f));

	cub[0] = (new float[3]{ -45,-60,-30 }); // links unten
	cub[1] = (new float[3]{ -45,-30,-30 }); // links oben
	cub[2] = (new float[3]{ 20,-30,-30 }); // rechts oben
	cub[3] = (new float[3]{ 20,-60,-30 }); // rechts unten
	cub[4] = (new float[3]{ -45,-60,-55 }); // links unten
	cub[5] = (new float[3]{ -45,-30,-55 }); // links oben
	cub[6] = (new float[3]{ 20,-30,-55 }); // rechts oben
	cub[7] = (new float[3]{ 20,-60,-55 }); // rechts unten
	drawQuader(cub, focus, Color(.35f,1.0f,.35f));

	cub[0] = (new float[3]{ -50,0,0 }); // links unten
	cub[1] = (new float[3]{ -50,50,0 }); // links oben
	cub[2] = (new float[3]{ 0,50,0 }); // rechts oben
	cub[3] = (new float[3]{ 0,0,0 }); // rechts unten
	cub[4] = (new float[3]{ -50,0,-50 }); // links unten
	cub[5] = (new float[3]{ -50,50,-50 }); // links obens
	cub[6] = (new float[3]{ 0,50,-50 }); // rechts oben
	cub[7] = (new float[3]{ 0,0,-50 }); // rechts unten
	drawQuader(cub, focus, Color(.35f,.35f,1.0f));

	glFlush();
	glutSwapBuffers();
}
////////////////////////////////////////////////////////////
// Interaction
void reset() {
	focus = 300;
	eyePoint = new float[4]{ 0,0,0,1 };
	viewDir = new float[4]{ 0,0,-1,0 };
	viewUp = new float[4]{ 0,1,0,0 };
}

CMat4f translation(float step) {
	if (step < 0)
		return CMat4f(
			new float[4][4] {
				{1,0,0,-eyePoint(0) + abs(step)},
				{0,1,0,-eyePoint(1) + abs(step)},
				{0,0,1,-eyePoint(2) + abs(step)},
				{0,0,0,1}
			}
		);
	else
		return CMat4f(
			new float[4][4] {
				{1,0,0,eyePoint(0) - step},
				{0,1,0,eyePoint(1) - step},
				{0,0,1,eyePoint(2) - step},
				{0,0,0,1}
			}
		);
}

CMat4f rotation(char axis, float w) {
	w = w * 3.141 / 180; // to radians
	switch (axis) {
	case 'x':
	case 'X':
		return CMat4f(
			new float[4][4] {
				{1,0,0,0},
				{0,(float)cos(w),(float)-sin(w),0},
				{0,(float)sin(w),(float)cos(w),0},
				{0,0,0,1}
			}
		);
	case 'y':
	case 'Y':
		return CMat4f(
			new float[4][4] {
				{(float)cos(w),0,(float)sin(w),0},
				{0,1,0,0},
				{(float)-sin(w),0,(float)cos(w),0},
				{0,0,0,1}
			}
		);
	case 'z':
	case 'Z':
		return CMat4f(
			new float[4][4] {
				{(float)cos(w),(float)-sin(w),0,0},
				{(float)sin(w),(float)cos(w),0,0},
				{0,0,1,0},
				{0,0,0,1}
			}
		);
	}
}

CMat4f rotationCam(char dir, float w) {
	CVec4f ax;

	switch (dir) {
	case 'u':
	case 'U':
		ax = viewDir;
		break;
	case 'd':
	case 'D':
		ax = viewUp;
		break;
	case 'x':
	case 'X':
		ax = xAxis();
		break;
	default:
		return CMat4f(
			new float[4][4]{
				 	{1,0,0,0},
				 	{0,1,0,0},
					{0,0,1,0},
					{0,0,0,1} 
				}
			);
		break;
	}

	ax = ax * pow(normalize(ax), -1);

	float d = sqrt(pow(ax(0), 2) + pow(ax(1), 2));

	CMat4f R1, R2, R1_rev, R2_rev;
	if (d != 0) {
		R1 = new float[4][4]{ 
			{ax(0) / d,ax(1) / d,0,0},
			{-ax(1) / d,ax(0) / d,0,0},
			{0,0,1,0},
			{0,0,0,1} 
		};
		R2 = new float[4][4]{ 
			{ax(2),0,-d,0},
			{0,1,0,0},
			{d,0,ax(2),0},
			{0,0,0,1} 
		};
		R1_rev = new float[4][4]{ 
			{ax(0) / d,-ax(1) / d,0,0},
			{ax(1) / d,ax(0) / d,0,0},
			{0,0,1,0},
			{0,0,0,1} 
		};
		R2_rev = new float[4][4]{ 
			{ax(2),0,d,0},
			{0,1,0,0},
			{-d,0,ax(2),0},
			{0,0,0,1}
		};
	} else {
		R1 = new float[4][4]{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
		};
		R2 = R1;
		R1_rev = R1;
		R2_rev = R1;
	}
	return translation(1) * R1_rev * R2_rev * rotation('z', w) * R2 * R1 * translation(-1);
}

void generalTransformation(CMat4f transMat) {
	eyePoint = transMat * eyePoint;
	viewDir = transMat * viewDir;
	viewUp = transMat * viewUp;
} 
////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y) {

	float angle = 30;
	float translationStep = 30;
	float focusStep = 20;

	switch (key) {
	case 'q':
	case 'Q':
		exit(0); // quit program
		break;
	// --------------------------------------------------------------------
	case 'f': // - Focus Length
		if (focus < 40) {
			break;
		}
		focus -= focusStep;
		break;
	case 'F': // + Focus Length
		focus += focusStep;
		break;
	// --------------------------------------------------------------------
	case 'x':
	case 'y':
	case 'z':
		generalTransformation(rotation(key,-angle));
		break;
	case 'X':
	case 'Y':
	case 'Z':
		generalTransformation(rotation(key,angle));
		break;
	// --------------------------------------------------------------------
	case 'a':
		viewUp = rotationCam('u',-angle) * viewUp;
		break;
	case 'A':
		viewUp = rotationCam('u',angle) * viewUp;
		break;
	case 'b':
		viewDir = rotationCam('d',-angle) * viewDir;
		break;
	case 'B':
		viewDir = rotationCam('d',angle) * viewDir;
		break;
	case 'c':
		viewUp = rotationCam('x', -angle) * viewUp;
		viewDir = rotationCam('x', -angle) * viewDir;
		break;
	case 'C':
		viewUp = rotationCam('x', angle) * viewUp;
		viewDir = rotationCam('x', angle) * viewDir;
		break;
	// --------------------------------------------------------------------
	case 'r':
	case 'R': // Resets
		reset();
		break;
	// --------------------------------------------------------------------
	case 'u':
		eyePoint(0)--;
		break;
	case 'U':
		eyePoint(0)++;
		break;
	case 'v':
		eyePoint(1)--;
		break;
	case 'V':
		eyePoint(1)++;
		break;
	case 'w':
		eyePoint(2)--;
		break;
	case 'W':
		eyePoint(2)--;
		break;
	// --------------------------------------------------------------------
	default:
		// do nothing ...
		break;
	};
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Uebung 3");

	init();
	initGL();

	glutTimerFunc(10, timer, 0);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	glutReshapeWindow(800,800);

	glutMainLoop();

	return 0;
}
