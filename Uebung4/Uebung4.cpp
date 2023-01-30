/////////////////////////////////////////////////////////////
//
// Framework for computer graphics assignement 4
//
//
// (c) Georg Umlauf, 2022
//
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//
// INCLUDES
//
/////////////////////////////////////////////////////////////

// include-file for text in-/output
#include <iostream>
using namespace std;
// Include-File für Windows creation
#include "windows.h"
// include-file for GLUT-Library
#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"

// my own include-files
#include "vec.h"
#include "color.h"
#include "sphere.h"
#include "cg_math.h"

/////////////////////////////////////////////////////////////
//
// GLOBAL VARIABLES
//
/////////////////////////////////////////////////////////////

// resolution of texture
// !!!ATTENTION!!! not every resolution works!
#define TEX_RES_X 400
#define TEX_RES_Y 400
// Number of pixel of texture
#define TEX_RES TEX_RES_X *TEX_RES_Y
// length of texture's axis is asymmetric: -TexRes#/2 to TexRes#/2-1

// global texture buffer
char g_Buffer[3 * TEX_RES];
// texture ID
GLuint g_TexID = 0;

// resolution of main window (can be changed by user)
int g_WinWidth = 400;
int g_WinHeight = 400;

// increments for interaction
float illum_inc = 0.05; // increment for illumination values
float color_inc = 0.05; // increment for color        values
float light_inc = 0.1;	// increment for angles in spherical angles for light source position

// scene parameters
Sphere sphere(CVec3f(0, 0, 100), 50);
unsigned material = 0; // default = 0, brass = 1, bronze = 2

// light source and camera parameters
int A = -1;							// eye point
CVec3f affLight(0, 1000, 0);		// affine    coordinates of light source, i.e. (x,y,z)
CVec3f sphLight(1000, 1.5 * PI, 0); // spherical coordinates of light source, i.e. (radius, phi, psi)
Color backColor = Color(0.8, 0.8, 0.8);

// default illumination levels
CVec3f ambientIllumination(0.1, 0.1, 0.1);
CVec3f diffuseIllumination(1.0, 1.0, 1.0);
CVec3f specularIllumination(0.5, 0.5, 0.5);

// default reflection coefficients
Color specularColor(0.5, 0.5, 0.5); // specular color, aka color of light source
Color ambientColor(0.5, 0.5, 0.5);	// ambient color , aka color of environment
Color diffuseColor(1.0, 1.0, 1.0);	// diffuse color , aka color of object
int shininess = 28;					// shininess

// other reflection coefficients
// brass ( dt. Messing)
Color ambientBrass = Color(0.33, 0.22, 0.03);
Color diffuseBrass = Color(0.78, 0.57, 0.11);
Color specularBrass = Color(0.99, 0.94, 0.81);
int shininessBrass = 28;
// bronze
Color ambientBronze = Color(0.21, 0.13, 0.05);
Color diffuseBronze = Color(0.71, 0.43, 0.18);
Color specularBronze = Color(0.39, 0.27, 0.17);
int shininessBronze = 26;

// forward declarations
void clearTexture(const Color &c = backColor);
void setPixel(const CVec2i &p, const Color &c = backColor);

/////////////////////////////////////////////////////////////
//
// YOUR FUNCTIONS
//
/////////////////////////////////////////////////////////////

CVec3f intersectSphere(const CVec3f &EyePos, const CVec3f &ViewDir)
{
	////////////////////////////////////////////////////////////
	//
	// This is the place to implement a sphere-ray-intersection
	//
	////////////////////////////////////////////////////////////

	double radius = sphere.getRadius();
	CVec3f center = sphere.getCenter();

	float a = ViewDir.dot(ViewDir);
	float b = 2 * EyePos.dot(ViewDir) - 2 * ViewDir.dot(center);
	float c = EyePos.dot(EyePos) + center.dot(center) + 2 * center.dot(EyePos) - radius * radius;

	// Calculate the discriminant
	float discriminant = b * b - 4 * a * c;

	// Check if there are real solutions
	if (discriminant < 0)
	{
		// There are no real solutions, return a point with negative z coordinate
		return CVec3f(0, 0, -1);
	}
	else
	{
		// There are real solutions, calculate the intersection points
		float t1 = (-b + sqrt(discriminant)) / (2 * a);
		float t2 = (-b - sqrt(discriminant)) / (2 * a);

		// Return the point with the smaller t value (closer to the eye)
		if (t1 < t2)
		{
			CVec3f intersect = ViewDir * t1;
			return EyePos + intersect;
		}
		else
		{
			CVec3f intersect = ViewDir * t2;
			return EyePos + intersect;
		}
	}
}

Color illumination(const CVec3f &HitPos, const CVec3f &ViewDir)
{
	// reflection coefficients
	CVec3f Ka, Kd, Ks;
	int shi;
	switch (material)
	{
	default:
	case 0:					// default color
		Ka = ambientColor;	// ambient
		Kd = diffuseColor;	// diffuse
		Ks = specularColor; // specular
		shi = shininess;	// shininess
		break;
	case 1:					  // brass
		Ka = ambientBrass;	  // ambient
		Kd = diffuseBrass;	  // diffuse
		Ks = specularBrass;	  // specular
		shi = shininessBrass; // shininess
		break;
	case 2:					   // bronze
		Ka = ambientBronze;	   // ambient
		Kd = diffuseBronze;	   // diffuse
		Ks = specularBronze;   // specular
		shi = shininessBronze; // shininess
		break;
	}

	// illumination levels
	CVec3f Ia = ambientIllumination;  // ambient
	CVec3f Id = diffuseIllumination;  // diffuse
	CVec3f Is = specularIllumination; // specular

	////////////////////////////////////////////////////////////
	//
	// This is the place to implement Phong illumination
	//
	CVec3f LightPos = affLight;

	CVec3f light_vector = LightPos - HitPos; // Light DIRECTION (Hit -> LightPos: L in the image.
	light_vector = light_vector.normalize();

	CVec3f center = sphere.getCenter();
	// Calculate the normal at the intersection point
	CVec3f N = (HitPos - center).normalize();

	// Calculate the reflection vector
	float cosTheta = max(0.0f, N.dot(light_vector));

	CVec3f diffuseContrib(0, 0, 0);
	CVec3f specularContrib(0, 0, 0);
	CVec3f ambient(Ka[0] * Ia[0], Ka[1] * Ia[1], Ka[2] * Ia[2]);

	if (cosTheta > 0.0)
	{
		float light_normal_dot = -2 * light_vector.dot(N);
		CVec3f projection(N);
		projection *= light_normal_dot;

		CVec3f R = -light_vector - projection;

		// Calculate the diffuse and specular components
		CVec3f diffuse_tmp(Id[0] * cosTheta * Kd[0], Id[1] * cosTheta * Kd[1], Id[2] * cosTheta * Kd[2]);
		diffuseContrib = diffuse_tmp;

		float specular = max(R.dot(-ViewDir.normalize()), 0.0f);
		specular = pow(specular, shininess);

		CVec3f spec_tmp(Is[0] * specular * Ks[0], Is[1] * specular * Ks[1], Is[2] * specular * Ks[2]);
		specularContrib = spec_tmp;
	}
	// Return the final color
	CVec3f final_color_vec = ambient + diffuseContrib + specularContrib;
	final_color_vec = crop(0.0, 1.0, final_color_vec);
	Color color_out(final_color_vec[0], final_color_vec[1], final_color_vec[2]);
	return color_out;
	//
	// Please try different parameter settings for Ka, Ks, Kd, shi, Ia, Id, Is
	//
	////////////////////////////////////////////////////////////
}

// This function controls and triggers the rendering process
void rayCast()
{
	clearTexture(backColor);

	CVec3f eyePoint = CVec3f(0.0, 0.0, A); // eye point
	CVec3f viewDir = CVec3f(0.0, 0.0, -A); // view direction
	CVec3f hitPoint;
	Color pixelColor;

	// iterate over the pixels
	for (int x = 0; x < TEX_RES_X; x++)
	{
		for (int y = 0; y < TEX_RES_Y; y++)
		{
			// convert viewport to view window
			viewDir[0] = -1 + 2 * x / static_cast<float>(TEX_RES_X - 1);
			viewDir[1] = -1 + 2 * y / static_cast<float>(TEX_RES_Y - 1);

			// compute scene point covered by pixel
			hitPoint = intersectSphere(eyePoint, viewDir);

			// compute pixel color
			pixelColor = backColor; // background color
			if (hitPoint[2] != -1)
				pixelColor = illumination(hitPoint, viewDir);

			// draw the pixel
			setPixel(CVec2i(x, y), pixelColor);
		}
	}
	cout << "raycast done" << endl;
}

/////////////////////////////////////////////////////////////
//
// PLEASE-DO-NOT-CHANGE FUNCTIONS
//
/////////////////////////////////////////////////////////////

// Functions to set up the texture.
// Please do not change!
void manageTexture()
{
	glEnable(GL_TEXTURE_2D);

	if (g_TexID == 0)
		glGenTextures(1, &g_TexID);

	glBindTexture(GL_TEXTURE_2D, g_TexID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, g_Buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
}

void mapTexture()
{
	glBindTexture(GL_TEXTURE_2D, g_TexID);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glTexCoord2f(0, 0);
	glVertex2f(-g_WinWidth / 2, -g_WinHeight / 2);
	glTexCoord2f(1, 0);
	glVertex2f(g_WinWidth / 2, -g_WinHeight / 2);
	glTexCoord2f(1, 1);
	glVertex2f(g_WinWidth / 2, g_WinHeight / 2);
	glTexCoord2f(0, 1);
	glVertex2f(-g_WinWidth / 2, g_WinHeight / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

// Callback function to adjust the window size.
// Please do not change!
void reshape(int w, int h)
{
	g_WinWidth = w;
	g_WinHeight = h;

	glViewport(0, 0, w, h); // Establish viewing area to cover entire window.

	glMatrixMode(GL_PROJECTION);				 // Start modifying the projection matrix.
	glLoadIdentity();							 // Reset project matrix.
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 1); // Map abstract coords directly to window coords.

	glutPostRedisplay();
}

// Function clears the display and fills it with color c
// Usage e.g. clearImage (Color (1,1,1))
// Please do not change!
void clearTexture(const Color &c)
{
	for (int i = 0; i < TEX_RES; i++)
	{
		g_Buffer[3 * i] = 255.0 * c.r();
		g_Buffer[3 * i + 1] = 255.0 * c.g();
		g_Buffer[3 * i + 2] = 255.0 * c.b();
	}
}

// Function plots a point p with color c
// Usage e.g.: addPoint (Point(10, 5), Color (1,0,0))
//
// Please do not change it, but use it!
void setPixel(const CVec2i &p, const Color &c)
{
	if (p[0] < 0 || p[1] < 0 || p[0] > TEX_RES_X - 1 || p[1] > TEX_RES_Y - 1)
	{
		cerr << "Illegal pixel co-ordinates (" << p[0] << ", " << p[1] << ")\n"
			 << flush;
		return;
	}

	g_Buffer[3 * TO_LINEAR(p[0], p[1])] = 255.0 * c.r();
	g_Buffer[3 * TO_LINEAR(p[0], p[1]) + 1] = 255.0 * c.g();
	g_Buffer[3 * TO_LINEAR(p[0], p[1]) + 2] = 255.0 * c.b();
}

// Callback function controlling the repaint of the display
// Please do not change!
void display(void)
{
	manageTexture();
	glClear(GL_COLOR_BUFFER_BIT);
	mapTexture();
	glFlush();
	glutSwapBuffers();
}

// Callback function initializing the repaint of the display
// Please do not change!
void init()
{
	affLight = SphericalToAffine(sphLight);
	rayCast();
}

// Callback function to handle keyboard input
// Please do not change!
void keyboard(unsigned char c, int x, int y)
{
	CVec3f inc(illum_inc, illum_inc, illum_inc);
	switch (c)
	{

	/////////////////////////////////
	// Light source
	/////////////////////////////////
	case 'x':
		sphLight[1] = crop(PI, 2 * PI, sphLight[1] - light_inc);
		break;
	case 'X':
		sphLight[1] = crop(PI, 2 * PI, sphLight[1] + light_inc);
		break;
	case 'y':
		sphLight[2] = crop(0, PI, sphLight[2] - light_inc);
		break;
	case 'Y':
		sphLight[2] = crop(0, PI, sphLight[2] + light_inc);
		break;

	/////////////////////////////////
	// Color of sphere
	/////////////////////////////////
	case 'r':
		diffuseColor[0] = crop(0, 1, diffuseColor[0] - color_inc);
		break;
	case 'R':
		diffuseColor[0] = crop(0, 1, diffuseColor[0] + color_inc);
		break;
	case 'g':
		diffuseColor[1] = crop(0, 1, diffuseColor[1] - color_inc);
		break;
	case 'G':
		diffuseColor[1] = crop(0, 1, diffuseColor[1] + color_inc);
		break;
	case 'b':
		diffuseColor[2] = crop(0, 1, diffuseColor[2] - color_inc);
		break;
	case 'B':
		diffuseColor[2] = crop(0, 1, diffuseColor[2] + color_inc);
		break;

	/////////////////////////////////
	// Color of light source
	/////////////////////////////////
	case 'c':
		specularColor[0] = crop(0, 1, specularColor[0] - color_inc);
		break;
	case 'C':
		specularColor[0] = crop(0, 1, specularColor[0] + color_inc);
		break;
	case 'e':
		specularColor[1] = crop(0, 1, specularColor[1] - color_inc);
		break;
	case 'E':
		specularColor[1] = crop(0, 1, specularColor[1] + color_inc);
		break;
	case 'f':
		specularColor[2] = crop(0, 1, specularColor[2] - color_inc);
		break;
	case 'F':
		specularColor[2] = crop(0, 1, specularColor[2] + color_inc);
		break;

	/////////////////////////////////
	// Color of environment
	/////////////////////////////////
	case 'u':
		ambientColor[0] = crop(0, 1, ambientColor[0] - color_inc);
		break;
	case 'U':
		ambientColor[0] = crop(0, 1, ambientColor[0] + color_inc);
		break;
	case 'v':
		ambientColor[1] = crop(0, 1, ambientColor[1] - color_inc);
		break;
	case 'V':
		ambientColor[1] = crop(0, 1, ambientColor[1] + color_inc);
		break;
	case 'w':
		ambientColor[2] = crop(0, 1, ambientColor[2] - color_inc);
		break;
	case 'W':
		ambientColor[2] = crop(0, 1, ambientColor[2] + color_inc);
		break;

	/////////////////////////////////
	// Shininess
	/////////////////////////////////
	case 's':
		shininess -= 5;
		shininess = max(0, shininess);
		break;
	case 'S':
		shininess += 5;
		break;

	/////////////////////////////////
	// Illumination level
	/////////////////////////////////
	case '1':
		diffuseIllumination = crop(0, 1, diffuseIllumination - inc);
		break; // diffuse  illumination
	case '2':
		diffuseIllumination = crop(0, 1, diffuseIllumination + inc);
		break; // diffuse  illumination
	case '3':
		specularIllumination = crop(0, 1, specularIllumination - inc);
		break; // specular illumination
	case '4':
		specularIllumination = crop(0, 1, specularIllumination + inc);
		break; // specular illumination
	case '5':
		ambientIllumination = crop(0, 1, ambientIllumination - inc);
		break; // ambient  illumination
	case '6':
		ambientIllumination = crop(0, 1, ambientIllumination + inc);
		break; // ambient  illumination

	/////////////////////////////////
	// Radius
	/////////////////////////////////
	case 'd':
		sphere.setRadius(sphere.getRadius() - 1);
		break;
	case 'D':
		sphere.setRadius(sphere.getRadius() + 1);
		break;

	/////////////////////////////////
	// Material properties of sphere
	/////////////////////////////////
	case 'M':
		material = (material + 1) % 3;
		break;

	/////////////////////////////////
	// Quit
	/////////////////////////////////
	case 'q':
	case 'Q':
		exit(0);
		break;

	/////////////////////////////////
	// Help menue
	/////////////////////////////////
	default:
		cout << "Use 'x', 'X', 'y', and 'Y'           to move   the light source" << endl;
		cout << "Use 'r', 'R', 'g', 'G', 'b', and 'B' to change the diffuse  color" << endl;
		cout << "Use 'c', 'C', 'e', 'E', 'f', and 'F' to change the specular color" << endl;
		cout << "Use 'u', 'U', 'v', 'V', 'w', and 'W' to change the ambient  color" << endl;
		cout << "Use '1' and '2'                      to change the diffuse  illumination level" << endl;
		cout << "Use '3' and '4'                      to change the specular illumination level" << endl;
		cout << "Use '5' and '6'                      to change the ambient  illumination level" << endl;
		cout << "Use 's' and 'S'                      to change the shininess" << endl;
		cout << "Use 'd' and 'D'                      to change the radius" << endl;
		cout << "Use 'M'                              to change the material (normal/Messing/Bronze)" << endl;
		cout << "Use 'q' or 'Q'                       to quit" << endl;
		break;
	}
	cout << "Diffuse  color and illumination level: " << diffuseColor << ", " << diffuseIllumination << endl;
	cout << "Specular color and illumination level: " << specularColor << ", " << specularIllumination << endl;
	cout << "Ambient  color and illumination level: " << ambientColor << ", " << ambientIllumination << endl;
	cout << "Shininess :" << shininess << endl;
	cout << "Material: " << material << endl;

	affLight = SphericalToAffine(sphLight);
	rayCast();
	glutPostRedisplay();
}

// This is the main function, triggering the GLUT-Main-Loop
// Please do not change!
int main(int argc, char **argv)
{

	init();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(g_WinWidth, g_WinHeight);

	glutCreateWindow("Assignement 4: Phong");

	glutKeyboardFunc(keyboard); // handles keyboard input
	glutReshapeFunc(reshape);	// handles changes of window size
	glutDisplayFunc(display);	// handles repainting of the display

	glutMainLoop(); // transfering the control to GLUT

	glDeleteTextures(1, &g_TexID); // deletes the defined textures

	return 0;
}