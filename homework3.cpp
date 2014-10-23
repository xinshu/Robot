#include <windows.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>

#include <iostream>
#include <fstream>

using namespace std;

/**
* @author Shaola Ren
*/

#define RESET 0
#define PAN 1
#define ZOOM 2
#define ROTATE 3
#define CHANGESCENE 4
#define CHANGEMODE 5

GLuint texture[6];

double eyePosition[] = {0, 0, 2.0};
double viewAngle = 60, factor = 1.0, alpha = 0, beta = 0, gamma = 0;
int winWidth = 640, winHeight = 480;
double angle = 0;
int rotateAxis = 0;
bool pan = false, zoom = false;
int oldx = 0, oldy = 0;
bool keyState[256];
double headA = 0, neckA = 0, leftShoulderA = 0, rightShoulderA = 0;
double leftUpperArmA = 0, rightUpperArmA = 0, leftLowerArmA = 0, rightLowerArmA = 0;
double chestA = 0, abdomenA = 0, leftLegJointA = 0, rightLegJointA = 0;
double leftUpperLegA = 0, rightUpperLegA = 0, leftLowerLegA = 0, rightLowerLegA = 0;

int choice = -1;
GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat mat_diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[] = {50.0f};

GLubyte checkImage[64][64][3];
int floorScene = 0;
int viewMode = 0;
int leftButton = -1;

void displayRobot();

void makeCheckImage(void)
{
   int i, j, c;   
   for (i = 0; i < 64; i++) {
      for (j = 0; j < 64; j++) {
         if (((i&8) == 0) ^ ((j&8) == 0)) c = 255;
         else c = 0;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
      }
   }
}

void initState() {
    floorScene = 0;
    viewMode = 0;
    choice = -1;
    leftButton = -1;
    eyePosition[0] = 0.3;
    eyePosition[1] = 0;
    eyePosition[2] = 2.0;
    viewAngle = 60; 
    factor = 1.0;
    alpha = 0;
    beta = 0;
    gamma = 0;
    winWidth = 640;
    winHeight = 480;
    angle = 0;
    rotateAxis = 0;
    pan = false;
    zoom = false;
    for (int i = 0; i < 256; i++) keyState[i] = false;
    headA = 0; neckA = 5; leftShoulderA = 30; rightShoulderA = 30;
    leftUpperArmA = 5; rightUpperArmA = - 5; leftLowerArmA = 90; rightLowerArmA = 20;
    chestA = - 5; abdomenA = 5; leftLegJointA = 35; rightLegJointA = 15;
    leftUpperLegA = 3; rightUpperLegA = 3; leftLowerLegA = 75; rightLowerLegA = 10;
}

// load texture from image
// from http://www.cplusplus.com/articles/GwvU7k9E/, modified
int loadBMP(const char* location, GLuint &textureName) {
    char* datBuff[2] = {nullptr, nullptr}; // Header buffers
	char* pixels = nullptr; // Pixels
	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	BITMAPINFOHEADER* bmpInfo   = nullptr; // Info
    // The file... We open it with it's constructor
	ifstream file(location, ios::binary);
	if(!file)
	{
		cout << "Failure to open bitmap file.\n";
		return 1;
	}
    // Allocate byte memory that will hold the two headers
	datBuff[0] = new char[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new char[sizeof(BITMAPINFOHEADER)];
	file.read(datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read(datBuff[1], sizeof(BITMAPINFOHEADER));
    // Construct the values from the buffers
	bmpHeader = (BITMAPFILEHEADER*) datBuff[0];
	bmpInfo   = (BITMAPINFOHEADER*) datBuff[1];
    // Check if the file is an actual BMP file
	if(bmpHeader->bfType != 0x4D42)
	{
		cout << "File \"" << location << "\" isn't a bitmap file\n";
		return 2;
	}
    // First allocate pixel memory
	pixels = new char[bmpInfo->biSizeImage];
	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read(pixels, bmpInfo->biSizeImage);
    // We're almost done. We have our image loaded, however it's not in the right format.
	// .bmp files store image data in the BGR format, and we have to convert it to RGB.
	// Since we have the value in bytes, this shouldn't be to hard to accomplish	
    char tmpRGB = 0; // Swap buffer
	for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
	{
		tmpRGB        = pixels[i];
		pixels[i]     = pixels[i + 2];
		pixels[i + 2] = tmpRGB;
	}
	// Set width and height to the values loaded from the file
	GLuint w = bmpInfo->biWidth;
	GLuint h = bmpInfo->biHeight;
    /*******************GENERATING TEXTURES*******************/
	glBindTexture(GL_TEXTURE_2D, textureName); // Bind that texture temporarily
	GLint mode = GL_RGB;                   // Set the mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	 
	// Create the texture. We get the offsets from the image, then we use it with the image's
	// pixel data to create it.
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, pixels);
    // Delete the two buffers.
	delete[] datBuff[0];
	delete[] datBuff[1];
	delete[] pixels;
	// Output a successful message
	cout << "Texture \"" << location << "\" successfully loaded.\n";
	return 0; // Return success code 
}

// create texture
void generateTexture() {
    glGenTextures(6, &texture[0]);
    loadBMP("texture/grass.bmp", texture[0]);
    loadBMP("texture/oak.bmp", texture[1]);
    loadBMP("texture/water.bmp", texture[2]);
    makeCheckImage();
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
    loadBMP("texture/steel.bmp", texture[4]);
    loadBMP("texture/woodiron.bmp", texture[5]);
}

// assistant methods
double changeAngleSmall(double oldA, int oldV, int newV) {
    oldA += (newV - oldV) * 180.0 / glutGet(GLUT_SCREEN_WIDTH);
    if (oldA < -90) oldA = -90.0;
    else if (oldA > 90) oldA = 90.0;
    return oldA;
}

double changeAngleBig(double oldA, int oldV, int newV) {
    oldA -= 2.0 * (newV - oldV);
    return oldA;
}

void setAmbient(GLfloat mat_amb[4], GLfloat f0, GLfloat f1, GLfloat f2) {
    mat_amb[0] = f0;
    mat_amb[1] = f1;
    mat_amb[2] = f2;
}

void setDiffuse(GLfloat mat_dif[4], GLfloat f0, GLfloat f1, GLfloat f2) {
    mat_dif[0] = f0;
    mat_dif[1] = f1;
    mat_dif[2] = f2;
}

// mouse
void myMouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        oldx = x;
        oldy = y;
        leftButton = GLUT_LEFT_BUTTON;
    }
}

void myMouseMotion(int x, int y) {
    if (leftButton != GLUT_LEFT_BUTTON) return;
    if (keyState[97]) {
        choice = -1;
        headA = changeAngleSmall(headA, x, oldx);
    }
    else if (keyState[98]) {
        choice = -1;
        neckA = changeAngleSmall(neckA, x, oldx);
    }
    else if (keyState[99]) {
        choice = -1;
        leftShoulderA = changeAngleBig(leftShoulderA, x, oldx);
    }
    else if (keyState[100]) {
        choice = -1;
        rightShoulderA = changeAngleBig(rightShoulderA, x, oldx);
    }
    else if (keyState[101]) {
        choice = -1;
        leftUpperArmA = changeAngleBig(leftUpperArmA, x, oldx);
    }
    else if (keyState[102]) {
        choice = -1;
        rightUpperArmA = changeAngleBig(rightUpperArmA, x, oldx);
    }
    else if (keyState[103]) {
        choice = -1;
        leftLowerArmA = 180.0 * (double) x / glutGet(GLUT_SCREEN_WIDTH);
    }
    else if (keyState[104]) {
        choice = -1;
        rightLowerArmA = 180.0 * x / glutGet(GLUT_SCREEN_WIDTH);
    }
    else if (keyState[105]) {
        choice = -1;
        chestA = changeAngleSmall(chestA, x, oldx);
    }
    else if (keyState[106]) {
        choice = -1;
        abdomenA = changeAngleBig(abdomenA, x, oldx);
    }
    else if (keyState[107]) {
        choice = -1;
        leftLegJointA = changeAngleBig(leftLegJointA, x, oldx);
    }
    else if (keyState[108]) {
        choice = -1;
        rightLegJointA = changeAngleBig(rightLegJointA, x, oldx);
    }
    else if (keyState[109]) {
        choice = -1;
        leftUpperLegA = changeAngleBig(leftUpperLegA, x, oldx);
    }
    else if (keyState[110]) {
        choice = -1;
        rightUpperLegA = changeAngleBig(rightUpperLegA, x, oldx);
    }
    else if (keyState[111]) {
        choice = -1;
        leftLowerLegA = 180.0 * x / glutGet(GLUT_SCREEN_WIDTH);
    }
    else if (keyState[112]) {
        choice = -1;
        rightLowerLegA = 180.0 * x / glutGet(GLUT_SCREEN_WIDTH);
    }
    switch (choice) {
    case 1:
        if (pan) {
            if (abs(x - oldx) > abs(y - oldy))
                eyePosition[0] += (x - oldx) * 5.0 / glutGet(GLUT_SCREEN_WIDTH);
            else 
                eyePosition[1] -= (y - oldy) * 5.0 / glutGet(GLUT_SCREEN_HEIGHT);
        }
        break;
    case 2:
        /*factor += (x - oldx) * 2.0 / glutGet(GLUT_SCREEN_WIDTH);
        if (factor > 3.0) factor = 3.0;
        else if (factor < 0.3) factor = 0.3;
        viewAngle = 60 * factor;*/
        if (zoom) eyePosition[2] += (x - oldx) * 5.0 / glutGet(GLUT_SCREEN_WIDTH);
        break;
    case 3:
        angle -= 2.0 * ((double)x - oldx);
        switch (rotateAxis) {
        case 1:
            alpha = angle;
            break;
        case 2:
            beta = angle;
            break;
        case 3:
            gamma = angle;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    displayRobot();
    oldx = x;
    oldy = y;
}

// key board
void myKeyUp(unsigned char key, int x, int y) {
    keyState[key] = false;
}

void myKeyboard(unsigned char key, int x, int y) {
    keyState[key] = true;
    if (choice == 1 && key == 'p') pan = true;
    if (choice == 2 && key == 'z') zoom = true;
    if (choice == 3) {
        switch (key) {
        case 'x':
            rotateAxis = 1;
            break;
        case 'y':
            rotateAxis = 2;
            break;
        case 'z':
            rotateAxis = 3;
            break;
        default:
            break;
        }
    }
}

// menu
void processMenuEvents(int option) {
    switch (option) {
    case RESET:
        initState();
        displayRobot();
        choice = -1;
        break;
    case PAN: 
        choice = 1;
        pan = false;
        break;
    case ZOOM: 
        choice = 2;
        zoom = false;
        break;
    case ROTATE: 
        choice = 3;
        break;
    case CHANGESCENE:
        floorScene = rand() % 4;
        displayRobot();
        choice = -1;
        break;
    case CHANGEMODE:
        viewMode = viewMode == 0 ? 1 : 0;
        displayRobot();
        choice = -1;
        break;
    default:
        break;
    }
}

// wrap a cube with texture
void wrapWithTexture(double x, double y, double z) {
    glPushMatrix();
    glScaled(x, y, z);
    glBegin(GL_QUADS);
        // front
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
		// back
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
		// top
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
		// bottom
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
		// right
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
		// left
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
	glEnd();
    glPopMatrix();
}

// objects
void drawFloor(double thickness) {
    glPushMatrix();
    glTranslated(0, - 0.75, 0);
    glScaled(1, 0.5 * thickness, 1);
    setAmbient(mat_ambient, 5.0f, 5.0f, 5.0f);
    //setAmbient(mat_ambient, 0.7f, 0.7f, 0.7f);
    setDiffuse(mat_diffuse, 1.0f, 1.0f, 1.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glBindTexture(GL_TEXTURE_2D, texture[floorScene]); // set texture
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 3.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(3.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(3.0f, 3.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);    
    glutSolidCube(2);
    glPopMatrix();
}

void robotHead(double sz) {
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    wrapWithTexture(sz, sz, sz);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glScaled(sz, sz, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

// neck, upper arm, lower arm, upper leg, lower leg
void robotVerticalPart(double length, double radius) {
    glBindTexture(GL_TEXTURE_2D, texture[3]); // set texture
    wrapWithTexture(radius, length, radius);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glScaled(radius, length, radius);
    glutSolidCube(1.0);
    glPopMatrix();
}

// shoulder, leg joints
void robotJoint(double length, double radius) {
    glBindTexture(GL_TEXTURE_2D, texture[4]); // set texture
    wrapWithTexture(length, radius, radius);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glScaled(length, radius, radius);
    glutSolidCube(1.0);
    glPopMatrix();
}

// chest, abdomen, pelvis, foot
void robotBody(double width, double height, double thickness) {
    glBindTexture(GL_TEXTURE_2D, texture[5]); // set texture
    wrapWithTexture(width, height, thickness);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPushMatrix();
    glScaled(width, height, thickness);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawRobot() {
    // upper body
    glPushMatrix();
    glTranslated(0, 0.175, 0);
        //setAmbient(mat_ambient, 0.0f, 0.5f, 1.0f);
        setAmbient(mat_ambient, 0.0f, 5.0f, 10.0f);
        setDiffuse(mat_diffuse, 0.0f, 0.5f, 1.0f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glTranslated(0, - 0.075, 0); //
        glRotated(abdomenA, 1, 0, 0); //
        glTranslated(0, 0.075, 0); // bend body
        robotBody(0.1, 0.15, 0.1); // abdomen
        glPushMatrix();
        glTranslated(0, 0.15, 0);
            setAmbient(mat_ambient, 0.0f, 2.0f, 5.0f);
            setDiffuse(mat_diffuse, 0.0f, 0.2f, 0.5f);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glRotated(chestA, 0, 1, 0); // rotate chest
            robotBody(0.2, 0.15, 0.125); // chest
            glPushMatrix();
            glTranslated(0.1025, 0.025, 0);
            setAmbient(mat_ambient, 5.0f, 5.0f, 0.0f);
            setDiffuse(mat_diffuse, 0.5f, 0.5f, 0.0f);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glRotated(- leftShoulderA, 1, 0, 0); // rotate shoulder
            robotJoint(0.05, 0.1); // shoulder
                glPushMatrix();
                glTranslated(0.05, - 0.1, 0);
                    setAmbient(mat_ambient, 1.0f, 5.0f, 0.0f);
                    setDiffuse(mat_diffuse, 0.1f, 0.5f, 0.0f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    glTranslated(0, 0.1, 0); //
                    glRotated(leftUpperArmA, 0, 0, 1); //
                    glTranslated(0, - 0.1, 0); // rotate upper arm
                    robotVerticalPart(0.25, 0.05); // upper arm
                    glPushMatrix();
                    glTranslated(0, - 0.25, 0);
                        setAmbient(mat_ambient, 5.0f, 1.0f, 0.0f);
                        setDiffuse(mat_diffuse, 0.5f, 0.1f, 0.0f);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                        glTranslated(0, 0.125, 0); //
                        glRotated(- leftLowerArmA, 1, 0, 0); //
                        glTranslated(0, - 0.125, 0); // rotate lower arm
                        robotVerticalPart(0.25, 0.04); // lower arm
                        glTranslated(0, -0.15, 0);
                        glRotated(- 90, 1, 0, 0);
                        glColor3d(0, 0, 0);
                        glutWireCone(0.05, 0.05, 5, 5); // hand
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
            glTranslated(- 0.1025, 0.025, 0);
                setAmbient(mat_ambient, 5.0f, 5.0f, 0.0f);
                setDiffuse(mat_diffuse, 0.5f, 0.5f, 0.0f);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                glRotated(rightShoulderA, 1, 0, 0); // rotate shoulder
                robotJoint(0.05, 0.1); // shoulder
                glPushMatrix();
                glTranslated(- 0.05, - 0.1, 0);
                    glTranslated(0, 0.1, 0); //
                    glRotated(rightUpperArmA, 0, 0, 1); //
                    glTranslated(0, - 0.1, 0); // rotate upper arm
                    setAmbient(mat_ambient, 1.0f, 5.0f, 0.0f);
                    setDiffuse(mat_diffuse, 0.1f, 0.5f, 0.0f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    robotVerticalPart(0.25, 0.05); // upper arm
                    glPushMatrix();
                    glTranslated(0, - 0.25, 0);
                        setAmbient(mat_ambient, 5.0f, 1.0f, 0.0f);
                        setDiffuse(mat_diffuse, 0.5f, 0.1f, 0.0f);
                        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                        glTranslated(0, 0.125, 0); //
                        glRotated(- rightLowerArmA, 1, 0, 0); //
                        glTranslated(0, - 0.125, 0); // rotate lower arm
                        robotVerticalPart(0.25, 0.04); // lower arm
                        glTranslated(0, -0.15, 0);
                        glRotated(- 90, 1, 0, 0);
                        glColor3d(0, 0, 0);
                        glutWireCone(0.05, 0.05, 5, 5); // hand
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
            glPushMatrix();
            glTranslated(0, 0.1, 0);
                setAmbient(mat_ambient, 0.0f, 5.0f, 0.0f);
                setDiffuse(mat_diffuse, 0.0f, 0.5f, 0.0f);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                glTranslated(0, - 0.025, 0); //
                glRotated(neckA, 1, 0, 0); //
                glTranslated(0, 0.025, 0); // bend neck
                robotVerticalPart(0.05, 0.05); // neck
                glPushMatrix();
                glTranslated(0, 0.1, 0);
                    //setAmbient(mat_ambient, 0.0f, 0.5f, 0.7f);
                    setAmbient(mat_ambient, 0.0f, 5.0f, 7.0f);
                    setDiffuse(mat_diffuse, 0.0f, 0.5f, 0.7f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    glRotated(headA, 0, 1, 0); // rotate head
                    robotHead(0.15); // head
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
    
    // lower body
    glPushMatrix();
    glTranslated(0, 0.05, 0);
        setAmbient(mat_ambient, 0.0f, 7.0f, 2.0f);
        setDiffuse(mat_diffuse, 0.0f, 0.7f, 0.2f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        robotBody(0.04, 0.1, 0.1); // pelvis
        glPushMatrix();
        setAmbient(mat_ambient, 5.0f, 5.0f, 0.0f);
        setDiffuse(mat_diffuse, 0.5f, 0.5f, 0.0f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glTranslated(0.045, 0, 0);
            glRotated(leftLegJointA, 1, 0, 0); // rotate leg joint
            robotJoint(0.05, 0.1); // leg joint
            glPushMatrix();
            glTranslated(0.02, - 0.2, 0);
                setAmbient(mat_ambient, 3.0f, 5.0f, 0.0f);
                setDiffuse(mat_diffuse, 0.3f, 0.5f, 0.0f);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                glTranslated(0, 0.175, 0); //
                glRotated(leftUpperLegA, 0, 0, 1); //
                glTranslated(0, - 0.175, 0); // rotate upper leg
                robotVerticalPart(0.35, 0.075); // upper leg
                glPushMatrix();
                glTranslated(0, - 0.35, 0);
                    setAmbient(mat_ambient, 9.0f, 0.0f, 5.0f);
                    setDiffuse(mat_diffuse, 0.9f, 0.0f, 0.5f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    glTranslated(0, 0.175, 0); //
                    glRotated(leftLowerLegA, 1, 0, 0); //
                    glTranslated(0, - 0.175, 0); // rotate lower leg
                    robotVerticalPart(0.35, 0.05); // lower leg
                    glTranslated(0, -0.2, 0);
                    setAmbient(mat_ambient, 0.0f, 0.0f, 0.0f);
                    setDiffuse(mat_diffuse, 0.0f, 0.0f, 0.0f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    robotBody(0.075, 0.05, 0.1); // foot
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glPushMatrix();
        setAmbient(mat_ambient, 5.0f, 5.0f, 0.0f);
        setDiffuse(mat_diffuse, 0.5f, 0.5f, 0.0f);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glTranslated(- 0.045, 0, 0);
            glRotated(- rightLegJointA, 1, 0, 0); // rotate leg joint
            robotJoint(0.05, 0.1); // leg joint
            glPushMatrix();
            glTranslated(- 0.02, - 0.2, 0);
                setAmbient(mat_ambient, 3.0f, 5.0f, 0.0f);
                setDiffuse(mat_diffuse, 0.3f, 0.5f, 0.0f);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                glTranslated(0, 0.175, 0); //
                glRotated(- rightUpperLegA, 0, 0, 1); //
                glTranslated(0, - 0.175, 0); // rotate upper leg
                robotVerticalPart(0.35, 0.075); // upper leg
                glPushMatrix();
                glTranslated(0, - 0.35, 0);
                    setAmbient(mat_ambient, 9.0f, 0.0f, 5.0f);
                    setDiffuse(mat_diffuse, 0.9f, 0.0f, 0.5f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    glTranslated(0, 0.175, 0); //
                    glRotated(rightLowerLegA, 1, 0, 0); //
                    glTranslated(0, - 0.175, 0); // rotate lower leg
                    robotVerticalPart(0.35, 0.05); // lower leg
                    glTranslated(0, -0.2, 0);
                    setAmbient(mat_ambient, 0.0f, 0.0f, 0.0f);
                    setDiffuse(mat_diffuse, 0.0f, 0.0f, 0.0f);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
                    robotBody(0.075, 0.05, 0.1); // foot
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

// refrence object to distinguish left and right
void drawReference() {
    glTranslated(0.75, 0, 0);
    setAmbient(mat_ambient, 5.0f, 2.5f, 0.0f);
    setDiffuse(mat_diffuse, 1.0f, 0.5f, 0.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glutSolidSphere(0.1, 15, 15);
}

void displayRobot(void) {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double ratio = (double)winWidth / winHeight;
    if (viewMode) glOrtho(-2.0 * ratio, 2.0 * ratio, -2, 2, 0.1, 100);
    else gluPerspective(viewAngle, ratio, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyePosition[0], eyePosition[1], eyePosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glRotated(alpha, 1, 0, 0);
    glRotated(beta, 0, 1, 0);
    glRotated(gamma, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    GLfloat lightIntensity[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_position[] = {3.0f, 3.0f, 3.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);

    drawRobot();

    drawFloor(0.05);

    drawReference();

    glutSwapBuffers();
}

void main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A Simple Robot");

    initState();

    generateTexture();

    glutDisplayFunc(displayRobot);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMouseMotion);
    glutKeyboardFunc(myKeyboard);
    glutKeyboardUpFunc(myKeyUp);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glutCreateMenu(processMenuEvents);
    glutAddMenuEntry("Reset", RESET);
    glutAddMenuEntry("Pan", PAN);
    glutAddMenuEntry("Zoom", ZOOM);
    glutAddMenuEntry("Rotate", ROTATE);
    glutAddMenuEntry("Change Scene", CHANGESCENE);
    glutAddMenuEntry("Change View Mode", CHANGEMODE);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, winWidth, winHeight);
    glutMainLoop();
}