#include <iostream>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <assert.h>
#include <fstream>
#include <cmath>
#include <GL/glu.h>
using namespace std;

class Image {
public:
    Image(char* ps, int w, int h);
    ~Image();
    char* pixels;
    int width;
    int height;
};

Image* loadBMP(const char* filename);

Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {}

Image::~Image() {
    delete[] pixels;
}

namespace {
    int toInt(const char* bytes) {
        return (int)(((unsigned char)bytes[3] << 24) | ((unsigned char)bytes[2] << 16) | ((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
    }

    short toShort(const char* bytes) {
        return (short)(((unsigned char)bytes[1] << 8) | (unsigned char)bytes[0]);
    }

    int readInt(ifstream &input) {
        char buffer[4];
        input.read(buffer, 4);
        return toInt(buffer);
    }

    short readShort(ifstream &input) {
        char buffer[2];
        input.read(buffer, 2);
        return toShort(buffer);
    }

    template<class T>
    class auto_array {
    private:
        T* array;
        mutable bool isReleased;
    public:
        explicit auto_array(T* array_ = NULL) :
            array(array_), isReleased(false) {}

        auto_array(const auto_array<T> &aarray) {
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        ~auto_array() {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
        }

        T* get() const {
            return array;
        }

        T &operator*() const {
            return *array;
        }

        void operator=(const auto_array<T> &aarray) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        T* operator->() const {
            return array;
        }

        T* release() {
            isReleased = true;
            return array;
        }

        void reset(T* array_ = NULL) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = array_;
        }

        T* operator+(int i) {
            return array + i;
        }

        T &operator[](int i) {
            return array[i];
        }
    };
}

Image* loadBMP(const char* filename) {
    ifstream input;
    input.open(filename, ifstream::binary);
    assert(!input.fail() || !"Could not find file");
    char buffer[2];
    input.read(buffer, 2);
    assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
    input.ignore(8);
    int dataOffset = readInt(input);

    int headerSize = readInt(input);
    int width;
    int height;
    switch (headerSize) {
        case 40:
            width = readInt(input);
            height = readInt(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            assert(readShort(input) == 0 || !"Image is compressed");
            break;
        case 12:
            width = readShort(input);
            height = readShort(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            break;
        case 64:
            assert(!"Can't load OS/2 V2 bitmaps");
            break;
        case 108:
            assert(!"Can't load Windows V4 bitmaps");
            break;
        case 124:
            assert(!"Can't load Windows V5 bitmaps");
            break;
        default:
            assert(!"Unknown bitmap format");
    }

    int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
    int size = bytesPerRow * height;
    auto_array<char> pixels(new char[size]);
    input.seekg(dataOffset, ios_base::beg);
    input.read(pixels.get(), size);

    auto_array<char> pixels2(new char[width * height * 3]);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                pixels2[3 * (width * y + x) + c] =
                        pixels[bytesPerRow * y + 3 * x + (2 - c)];
            }
        }
    }

    input.close();
    return new Image(pixels2.release(), width, height);
}


class Planet
{
public:
	float radius, distance, orbit, orbitSpeed, axisTilt, axisAni;
	Planet(float _radius, float _distance, float _orbit, float _orbitSpeed, float _axisTilt, float _axisAni)
	{
		radius = _radius;
		distance = _distance;
		orbit = _orbit;
		orbitSpeed = _orbitSpeed;
		axisTilt = _axisTilt;
		axisAni = _axisAni;
	}

	void drawSmallOrbit(void)
	{
		glColor3ub(255, 255, 255);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; ++i)
        {
            float angle = i * 3.14159 / 180;
            glVertex3f(cos(angle) * distance, 0.0, sin(angle) * distance);
        }
        glEnd();
	}

	void drawMoon(void)
	{
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();
		glPushMatrix();
		glColor3ub(255, 255, 255);
		glRotatef(orbit, 0.0, 1.0, 0.0);
		glTranslatef(distance, 0.0, 0.0);
		gluSphere(quadric, radius, 20.0, 20.0);
		glPopMatrix();
	}
};

// Sun
Planet sun(5.0, 0, 0, 0, 0, 0);					// Sun
// Planets
Planet mercury(1.0, 7, 0, 4.74, 02.11, 0);		// Mercury
Planet venus(1.5, 11, 0, 3.50, 177.0, 0);		// Venus
Planet earth(2.0, 16, 0, 2.98, 23.44, 0);	    // Earth
Planet mars(1.2, 21, 0, 2.41, 25.00, 0);	    // Mars
Planet jupiter(3.5, 28, 0, 1.31, 03.13, 0);	    // Jupiter
Planet saturn(3.0, 37, 0, 0.97, 26.70, 0);	    // Saturn
Planet uranus(2.5, 45.5, 0, 0.68, 97.77, 0);    // Uranus
Planet neptune(2.3, 53.6, 0, 0.54, 28.32, 0);   // Neptune
Planet pluto(0.3, 59, 0, 0.47, 119.6, 0);	    // Pluto
// Moons
Planet luna(.40, 3, 0, 5.40, 0, 0);		        // Luna      (Earth)
Planet phobos(.20, 1.8, 0, 2.30, 0, 0);	        // Phobos    (Mars)
Planet deimos(.24, 2.4, 0, 3.60, 0, 0);	  		// Deimos    (Mars)
Planet europa(.24, 4, 0, 4.40, 0, 0);		    // Europa    (Jupiter)
Planet ganymede(.24, 4.7, 0, 5.00, 0, 0);	    // Ganymede  (Jupiter)
Planet callisto(.24, 5.3, 0, 2.30, 0, 0);	    // Callisto  (Jupiter)
Planet titan(.75, 3.7, 0, 2.40, 0, 0);	  		// Titan	 (Saturn)
Planet nix(.10, 1.5, 0, 5.00, 0, 0);	  		// Nix	     (Pluto)
Planet puck(.26, 2.9, 0, 7.00, 0, 0);	  		// Puck	     (Uranus)
Planet triton(.36, 3.2, 0, 3.40, 0, 0);	  		// Triton    (Neptune)

int isAnimate = 0;
int bigOrbitActive = 1;
int smallOrbitActive = 1;
int moonsActive = 1;
int changeCamera = 0;
int frameCount = 0;
int labelsActive = 0;
int zoom = 50;

float lightPos[] = {0.0, 0.0, 75.0, 1.0};      // Spotlight position.
static float spotAngle = 360;			       // Spotlight cone half-angle.
float spotDirection[] = {1.0, 0.0, 0.0};       // Spotlight direction.
static float spotExponent = 1.0;		       // Spotlight exponent = attenuation factor.

GLuint loadTexture(Image *image)
{
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	return textureId;
}

GLuint sunTexture, mercuryTexture, venusTexture, earthTexture, marsTexture, jupiterTexture, saturnTexture, uranusTexture, neptuneTexture, plutoTexture, starsTexture, logTexture;

void writeBitmapString(void *font, char *string)
{
	char *c;
	for (c = string; *c != '\0'; c++){
		glutBitmapCharacter(font, *c);
	}
}

void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// TEXTURING SETUP
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	Image *stars = loadBMP("assets/stars.bmp");
	starsTexture = loadTexture(stars);
	delete stars;

	Image *sun = loadBMP("assets/sun.bmp");
	sunTexture = loadTexture(sun);
	delete sun;

	Image *mercury = loadBMP("assets/mercury.bmp");
	mercuryTexture = loadTexture(mercury);
	delete mercury;

	Image *venus = loadBMP("assets/venus.bmp");
	venusTexture = loadTexture(venus);
	delete venus;

	Image *earth = loadBMP("assets/earth.bmp");
	earthTexture = loadTexture(earth);
	delete earth;

	Image *mars = loadBMP("assets/mars.bmp");
	marsTexture = loadTexture(mars);
	delete mars;

	Image *jupiter = loadBMP("assets/jupiter.bmp");
	jupiterTexture = loadTexture(jupiter);
	delete jupiter;

	Image *saturn = loadBMP("assets/saturn.bmp");
	saturnTexture = loadTexture(saturn);
	delete saturn;

	Image *uranus = loadBMP("assets/uranus.bmp");
	uranusTexture = loadTexture(uranus);
	delete uranus;

	Image *neptune = loadBMP("assets/neptune.bmp");
	neptuneTexture = loadTexture(neptune);
	delete neptune;

	Image *pluto = loadBMP("assets/pluto.bmp");
	plutoTexture = loadTexture(pluto);
	delete pluto;

	// LIGHTING SETUP
	glEnable(GL_LIGHTING);
	float lightAmb[] = {0.0, 0.0, 0.0, 1.0};
	float lightDifAndSpec[] = {1.0, 1.0, 1.0, 1.0};
	float globAmb[] = {0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
}

void orbitalTrails(void)
{
	glPushMatrix();
    glColor3ub(255, 255, 255);
	glTranslatef(0.0, 0.0, 0.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    Planet planets[] = {mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto};
    int numPlanets = sizeof(planets) / sizeof(planets[0]);
    for (int i = 0; i < numPlanets; ++i) 
	{
    	glutSolidTorus(0.1, planets[i].distance, 100.0, 100.0);
    }
    glPopMatrix();
}

void drawSun() {
    GLUquadric *quadric;
    quadric = gluNewQuadric();
    glPushMatrix();
    glRotatef(sun.orbit, 0.0, 1.0, 0.0);
    glTranslatef(sun.distance, 0.0, 0.0);
	
	
	if (labelsActive == 1)
	{
		glRasterPos3f(-1.2, 7.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Sun");
	}
	glPushMatrix();
	glRotatef(sun.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(sun.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, sun.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();
}

void drawMercury() {
	GLUquadric *quadric;
	quadric = gluNewQuadric();
    glPushMatrix();
    glRotatef(mercury.orbit, 0.0, 1.0, 0.0);
    glTranslatef(mercury.distance, 0.0, 0.0);
	
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Mercury");
	}
	glPushMatrix();
	glRotatef(mercury.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(mercury.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mercuryTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, mercury.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();
}

void drawVenus() {
	GLUquadric *quadric;
	quadric = gluNewQuadric();
    glPushMatrix();
    glRotatef(venus.orbit, 0.0, 1.0, 0.0);
    glTranslatef(venus.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Venus");
	}
	glPushMatrix();
	glRotatef(venus.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(venus.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, venusTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, venus.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopMatrix();
}

void drawEarth() {
	GLUquadric *quadric;
	quadric = gluNewQuadric();
    glPushMatrix();
	glRotatef(earth.orbit, 0.0, 1.0, 0.0);
	glTranslatef(earth.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Earth");
	}
	glPushMatrix();
	glRotatef(earth.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(earth.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, earthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, earth.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		luna.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		luna.drawMoon();
	}
	glPopMatrix();
}

void drawMars() {
	GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(mars.orbit, 0.0, 1.0, 0.0);
	glTranslatef(mars.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Mars");
	}
	glPushMatrix();
	glRotatef(mars.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(mars.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, marsTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, mars.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		phobos.drawSmallOrbit();
		deimos.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		phobos.drawMoon();
		deimos.drawMoon();
	}
	glPopMatrix();
}

void drawJupiter() {
    GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(jupiter.orbit, 0.0, 1.0, 0.0);
	glTranslatef(jupiter.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Jupiter");
	}
	glPushMatrix();
	glRotatef(jupiter.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(jupiter.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, jupiterTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, jupiter.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		europa.drawSmallOrbit();
		ganymede.drawSmallOrbit();
		callisto.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		europa.drawMoon();
		ganymede.drawMoon();
		callisto.drawMoon();
	}
	glPopMatrix();
}

void drawSaturn(){
	GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(saturn.orbit, 0.0, 1.0, 0.0);
	glTranslatef(saturn.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Saturn");
	}
	glPushMatrix();
	glRotatef(saturn.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(saturn.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, saturnTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, saturn.radius, 20.0, 20.0);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glColor3ub(158, 145, 137);
	glRotatef(-63.0, 1.0, 0.0, 0.0);
	glutWireTorus(0.2, 6.0, 30.0, 30.0);
	glutWireTorus(0.4, 5.0, 30.0, 30.0);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		titan.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		titan.drawMoon();
	}
	glPopMatrix();

	glColor3ub(255, 255, 255);
}

void drawUranus(){
	GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(uranus.orbit, 0.0, 1.0, 0.0);
	glTranslatef(uranus.distance, 0.0, 0.0);
	
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Uranus");
	}
	glPushMatrix();
	glRotatef(uranus.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(uranus.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uranusTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, uranus.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		puck.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		puck.drawMoon();
	}
	glPopMatrix();
}

void drawNeptune(){
	GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(neptune.orbit, 0.0, 1.0, 0.0);
	glTranslatef(neptune.distance, 0.0, 0.0);
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Neptune");
	}
	glPushMatrix();
	glRotatef(neptune.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(neptune.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, neptuneTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, neptune.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		triton.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		triton.drawMoon();
	}
	glPopMatrix();
}

void drawPluto(){
	GLUquadric *quadric;
	quadric = gluNewQuadric();
	glPushMatrix();
	glRotatef(pluto.orbit, 0.0, 1.0, 0.0);
	glTranslatef(pluto.distance, 0.0, 0.0);
	if (labelsActive == 1)
	{
		glRasterPos3f(0.0, 3.0, 0.0);
		glColor3ub(255, 255, 255);
		writeBitmapString(GLUT_BITMAP_HELVETICA_12, "Pluto");
	}
	glPushMatrix();
	glRotatef(pluto.axisTilt, 1.0, 0.0, 0.0);
	glRotatef(pluto.axisAni, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, plutoTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);
	gluSphere(quadric, pluto.radius, 20.0, 20.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	if (smallOrbitActive == 1)
	{
		nix.drawSmallOrbit();
	}
	if (moonsActive == 1)
	{
		nix.drawMoon();
	}
	glPopMatrix();
}

void drawScene(void)
{
	frameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (changeCamera == 0)
		gluLookAt(0.0, zoom, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	if (changeCamera == 1)
		gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	if (changeCamera == 2)
		gluLookAt(0.0, zoom, 0.00001, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	if (bigOrbitActive == 1)
		orbitalTrails();

	drawSun();
	drawMercury();
	drawVenus();
	drawEarth();
	drawMars();
	drawJupiter();
	drawSaturn();
	drawNeptune();
	drawPluto();
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, starsTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_POLYGON);
	glTexCoord2f(-1.0, 0.0);
	glVertex3f(-200, -200, -100);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(200, -200, -100);
	glTexCoord2f(2.0, 2.0);
	glVertex3f(200, 200, -100);
	glTexCoord2f(-1.0, 2.0);
	glVertex3f(-200, 200, -100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, starsTexture);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-200, -83, 200);
	glTexCoord2f(8.0, 0.0);

	glVertex3f(200, -83, 200);
	glTexCoord2f(8.0, 8.0);
	glVertex3f(200, -83, -200);
	glTexCoord2f(0.0, 8.0);
	glVertex3f(-200, -83, -200);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glutSwapBuffers();
}

void drawScenesInOrder(void)
{
	drawScene();
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 200.0);
	glMatrixMode(GL_MODELVIEW);
}

void animate(int n)
{
	if (isAnimate)
	{
		mercury.orbit += mercury.orbitSpeed;
		venus.orbit += venus.orbitSpeed;
		earth.orbit += earth.orbitSpeed;
		mars.orbit += mars.orbitSpeed;
		jupiter.orbit += jupiter.orbitSpeed;
		saturn.orbit += saturn.orbitSpeed;
		uranus.orbit += uranus.orbitSpeed;
		neptune.orbit += neptune.orbitSpeed;
		pluto.orbit += pluto.orbitSpeed;
		luna.orbit += luna.orbitSpeed;
		phobos.orbit += phobos.orbitSpeed;
		deimos.orbit += deimos.orbitSpeed;
		europa.orbit += europa.orbitSpeed;
		ganymede.orbit += ganymede.orbitSpeed;
		callisto.orbit += callisto.orbitSpeed;
		titan.orbit += titan.orbitSpeed;
		nix.orbit += nix.orbitSpeed;
		puck.orbit += puck.orbitSpeed;
		triton.orbit += triton.orbitSpeed;
		if (mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto, luna, phobos, deimos, europa, ganymede, callisto, titan, nix, puck, triton.orbit > 360.0)
		{
			mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto, luna, phobos, deimos, europa, ganymede, callisto, titan, nix, puck, triton.orbit -= 360.0;
		}
		mercury.axisAni += 10.0;
		venus.axisAni += 10.0;
		earth.axisAni += 10.0;
		mars.axisAni += 10.0;
		jupiter.axisAni += 10.0;
		saturn.axisAni += 10.0;
		uranus.axisAni += 10.0;
		neptune.axisAni += 10.0;
		pluto.axisAni += 10.0;
		if (mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto.axisAni > 360.0)
		{
			mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto.axisAni -= 360.0;
		}
		glutPostRedisplay();
		glutTimerFunc(30, animate, 1);
	}
}

void mouseControl(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		exit(0);
	glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y)
{
	if (direction > 0 && zoom < 100)
		zoom--;
	if (direction < 0 && zoom > -75)
		zoom++;
	glutPostRedisplay();
}

void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		if (isAnimate)
			isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
		break;
	case 'o':
		if (smallOrbitActive)
			smallOrbitActive = 0;
		else
			smallOrbitActive = 1;
		glutPostRedisplay();
		break;
	case 'O':
		if (bigOrbitActive)
			bigOrbitActive = 0;
		else
			bigOrbitActive = 1;
		glutPostRedisplay();
		break;
	case 'm':
		if (moonsActive)
			moonsActive = 0;
		else
			moonsActive = 1;
		glutPostRedisplay();
		break;
	case 'l':
		if (labelsActive)
			labelsActive = 0;
		else
			labelsActive = 1;
		glutPostRedisplay();
		break;
	case '0':
		changeCamera = 0;
		glutPostRedisplay();
		break;
	case '1':
		changeCamera = 1;
		glutPostRedisplay();
		break;
	case '2':
		changeCamera = 2;
		glutPostRedisplay();
		break;
	}
}

void intructions(void)
{
	cout << "SPACE to play/pause the simulation." << endl;
	cout << "ESC to exit the simulation." << endl;
	cout << "O to show/hide Big Orbital Trails." << endl;
	cout << "o to show/hide Small Orbital Trails." << endl;
	cout << "m to show/hide Moons." << endl;
	cout << "l to show/hide labels" << endl;
	cout << "0, 1, and 2 to change camera angles." << endl;
	cout << "Scroll mouse to change camera movement" << endl;
}

int main(int argc, char **argv)
{
	intructions();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1080,1080);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Solar Odyssey");
	glutDisplayFunc(drawScenesInOrder);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseControl);
	glutKeyboardFunc(keyInput);
	glutMouseWheelFunc(mouseWheel);
	setup();
	glutMainLoop();
}
