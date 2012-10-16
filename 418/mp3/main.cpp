//  
// Example code for OpenGL programming
//
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include "../../newmat/newmat.h"
#include "../../libsmf/smfobj.h"
#include "../../libsmf/easypng.h"
#include <math.h>
#include <iostream> 
using namespace std;


#define TEXMAP 0
#define ENVMAP 1
#define SHADER 2

#define TEX_WIDTH 256
#define TEX_HEIGHT 256

int nFPS = 50;
float fRotateAngle = 0.f;
//mesh file
SmfObj teapot;
PNG teapotTex;
PNG envTex;


int mode = TEXMAP;

float ligthSwitch=1.0;
float envSwitch=1.0;
float bumpSwitch=1.0;


float t=0;
float tvar;
float red=0;
float green=0;
float blue=0;

GLuint potTexId;
GLuint envTexId;
GLubyte * imageData;
GLubyte * envData;
	
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat lpos[] = {1.0,0.0,0.0,0.0};
	
	
	GLuint programObj = 0;
GLint vstime;

float a=1.0;
float b=0.0;
float c=.50;
//reflection matrix
float mat44[16]={  (float)(1.0-(2.0*a*a)),	(float)-2.0*a*b,	(float)-2.0*a*c,	(float)0.,
		 (float)(-2.0*a*b),		(float)(1.0-(2.0*b*b)),	(float)-2.0*b*c,	(float)0.,
		 (float)-2.0*a*c,		(float)-2.0*b*c,	(float)(1.0-(2.0*c*c)),	(float)0.,
		 (float)0.,			(float)0.,		(float)0.,		(float)1.0};
			

char *loadshader(char *filename)
{
	std::string strbuf;
	std::string line;
	std::ifstream in(filename);
	while(std::getline(in,line))
		strbuf += line + '\n';

	char *buf = (char *)malloc(strbuf.size()*sizeof(char));
	strcpy(buf,strbuf.c_str());

	return buf;
}
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		free(infoLog);
	}
}

void initShader()
{

	glDisable(GL_TEXTURE_2D);

	GLchar *vertexShaderCodeStr = loadshader("blend.vs");
	const GLchar **vertexShaderCode = (const GLchar **)&vertexShaderCodeStr;

	GLchar *fragmentShaderCodeStr = loadshader("blend.fs");
	const GLchar **fragmentShaderCode = (const GLchar **)&fragmentShaderCodeStr;
	
	
	int status;
	GLint infologLength = 0;
	GLint charsWritten = 0;
	GLchar infoLog[10000];




	GLuint vertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObj, 1, vertexShaderCode, 0);
	glCompileShader(vertexShaderObj); /* Converts to GPU code */
	cout<<"Vertex log:"<<endl;
	printShaderInfoLog(vertexShaderObj);
	cout<<endl;
	
	
	

	GLuint fragmentShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObj, 1, fragmentShaderCode, 0);
	glCompileShader(fragmentShaderObj); /* Converts to GPU code */
	cout<<"Frag log:"<<endl;
	printShaderInfoLog(fragmentShaderObj);
	cout<<endl;
//	GLuint


	programObj = glCreateProgram();
	glAttachObjectARB(programObj,vertexShaderObj);
	glAttachObjectARB(programObj,fragmentShaderObj);
	glLinkProgram(programObj);	/* Connects shaders & variables */
	glUseProgram(programObj); /* OpenGL now uses the shader */
//	cout<<"Program log:"<<endl;
	printShaderInfoLog(programObj);
	
	
	
	
	
	int the_locationPot = glGetUniformLocation(programObj, "TexPot");
 	int the_locationEnv = glGetUniformLocation(programObj, "TexEnv");
 	int the_locationLightSwitch = glGetUniformLocation(programObj, "lightswitch");
 	int the_locationEnvSwitch   = glGetUniformLocation(programObj, "envswitch");
 	int the_locationBumpSwitch   = glGetUniformLocation(programObj, "bumpswitch");
	 //Bind to tex unit 0
	glUniform1i(the_locationPot, 0);
	//Bind to tex unit 1
	glUniform1i(the_locationEnv, 1);
	//Bind to tex unit 1
	glUniform1f(the_locationLightSwitch, ligthSwitch);
	glUniform1f(the_locationEnvSwitch, envSwitch);
	glUniform1f(the_locationBumpSwitch, bumpSwitch);
	
	
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, potTexId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, envTexId);
	glEnable(GL_TEXTURE_2D);
	 
	
	
	glGetProgramivARB(programObj, GL_LINK_STATUS, &status);

	glGetObjectParameterivARB(programObj,GL_OBJECT_INFO_LOG_LENGTH_ARB,&infologLength);

	if(infologLength > 0) {
		glGetInfoLogARB(programObj,infologLength,&charsWritten,infoLog);
		if(charsWritten)
			printf("InfoLog:\n%s\n\n",infoLog);
	}
	
	
	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

		
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_TEXTURE_2D);
	

}

void init(char *vsfilename, char *fsfilename) 
{
	//load files
	teapot.ObjRead("teapot.obj"); //load mesh file of 'I'
	teapot.getFaceNormals();
	teapot.getVertexNormals();
	
	
	// init your data, setup OpenGL environment here
	glClearColor(0.1,0.1,0.1,1.0); // clear color is gray		
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // uncomment this function if you only want to draw wireframe model
	glEnable(GL_DEPTH_TEST);
	
	
	
	teapotTex.readFromFile("texture.png");
	envTex.readFromFile("aaa.png");
	
	
	//TEXTURE SET UP

	
	imageData = new GLubyte[3* TEX_WIDTH * TEX_HEIGHT];
	envData = new GLubyte[3* TEX_WIDTH * TEX_HEIGHT];
	
	for (int i=0; i<TEX_HEIGHT;i++)
	{
		for (int j=0; j<3*TEX_WIDTH;j+=3)
		{
			imageData[i*3*TEX_WIDTH+j]  =teapotTex(i,j/3)->red;
			imageData[i*3*TEX_WIDTH+j+1]=teapotTex(i,j/3)->green;
			imageData[i*3*TEX_WIDTH+j+2]=teapotTex(i,j/3)->blue;
			
			
			envData[i*3*TEX_WIDTH+j]  =envTex(j/3,TEX_HEIGHT-i-1)->red;
			envData[i*3*TEX_WIDTH+j+1]=envTex(j/3+1,TEX_HEIGHT-i-1)->green;
			envData[i*3*TEX_WIDTH+j+2]=envTex(j/3+2,TEX_HEIGHT-i-1)->blue;
		}	
	}
	
//	for (int i=0; i<h;i++)
//	{
//		for (int j=0; j<w;j++)
//		{
//			imageData[i*j+j]=teapotTex(i,j)->red;
//			imageData[ 2*(i*j+j)]=teapotTex(i,j)->green;
//			imageData[3*(i*j+j)]=teapotTex(i,j)->blue;
//			imageData[4*(i*j+j)]=teapotTex(i,j)->alpha;
//		}
//		
//	}


//GLfloat xplane[] = {1,0,0,0};
//GLfloat yplane[] = {0,1,0,0};
//glTexGenfv(GL_S,GL_OBJECT_PLANE, xplane);
//glTexGenfv(GL_T,GL_OBJECT_PLANE, yplane);
//glEnable(GL_TEXTURE_GEN_S);
//glEnable(GL_TEXTURE_GEN_T);

	glGenTextures( 0, &potTexId );
	glBindTexture (GL_TEXTURE_2D, potTexId);

	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB,    TEX_WIDTH ,        TEX_HEIGHT,      0,   GL_RGB, GL_UNSIGNED_BYTE, imageData);
	//			    level rgb=3  image width  height   border format    
	

	glGenTextures( 1, &envTexId );
	
	glBindTexture (GL_TEXTURE_2D, envTexId);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);            // Set The Texture Generation Mode For S To Sphere Mapping ( NEW )
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);            // Set The Texture Generation Mode For T To Sphere Mapping ( NEW )

 
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH ,        TEX_HEIGHT,   0, GL_RGB, GL_UNSIGNED_BYTE, envData);
//	
		
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

		
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_TEXTURE_2D);
	
}


//function that draws I
//no input or output
//side effect: an 'I' will be drawn on the screen
//other: SmfObj 'I' and 'Off' must be loaded at the init stage, keyframing is used.


void display(void)
{
	// put your OpenGL display commands here
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// reset OpenGL transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // reset transformation matrix to identity

	// setup look at transformation so that 
	// eye is at : (0,0,10)
	// look at center is at : (0,0,0)
	// up direction is +y axis
	gluLookAt(0.f,0.f,10.f,
		0.f,0.f,0.f,
		0.f,1.f,0.f);
	
	//set up the gradient background
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glBegin(GL_QUADS);
//		//green color
//		glColor3f(0.0,1.0,0.0);
//		glVertex2f(-10.0,-10.0);
//		glVertex2f(10.0,-10.0);
//		//blue color
//		glColor3f(0.0,0.0,1.0);
//		glVertex2f(10.0, 10.0);
//		glVertex2f(-10.0, 10.0);
//	glEnd();
	


	glPushMatrix();	
		glTranslatef(5,0,0);
		glRotatef(fRotateAngle,0,1,0);
		glRotatef(fRotateAngle/2,1,0,0);
//		glScalef(.5,.5,.5);
//		teapot.enableRGB=false; 
//		teapot.draw();

//	GLfloat tanamb[] = {0.2,0.15,0.1,1.0};
//	GLfloat tandiff[] = {0.4,0.3,0.2,1.0};

	GLfloat tanamb[] = {.5,.5,.5,1};
	GLfloat tandiff[] = {.3,.3,.3,1};


		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tandiff);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
		
		
//	
//	glGenTextures( 1, &potTexId );
switch(mode)
{
	case TEXMAP: 
			glBindTexture (GL_TEXTURE_2D, potTexId); 
			break;
	case ENVMAP: 
			glBindTexture (GL_TEXTURE_2D, envTexId);
			glEnable(GL_TEXTURE_GEN_S);                     // Enable Texture Coord Generation For S ( NEW )
		   	glEnable(GL_TEXTURE_GEN_T);                     // Enable Texture Coord Generation For T ( NEW )
			break;
	case SHADER:
			
			break;
}
//if (mode==TEXMAP)
//{
//	glBindTexture (GL_TEXTURE_2D, potTexId);
//}
//else
//{
//	glBindTexture (GL_TEXTURE_2D, envTexId);
//	glEnable(GL_TEXTURE_GEN_S);                     // Enable Texture Coord Generation For S ( NEW )
//   	glEnable(GL_TEXTURE_GEN_T);                     // Enable Texture Coord Generation For T ( NEW )
//	
//}
//	glBindTexture (GL_TEXTURE_2D, envTexId);
//	glEnable(GL_TEXTURE_GEN_S);                     // Enable Texture Coord Generation For S ( NEW )
//   	glEnable(GL_TEXTURE_GEN_T);                     // Enable Texture Coord Generation For T ( NEW )
	
		glBegin(GL_TRIANGLES);
		
		for (unsigned i=0; i<teapot.triangles.size();i++)
		{
			float divCons=1;
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(0)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(0)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(0)][2] );
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(0)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(0)]->co[1]/divCons);
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(0)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(0)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(0)]->co[2]);	
					
					
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(1)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(1)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(1)][2] );					
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(1)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(1)]->co[1]/divCons);					
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(1)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(1)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(1)]->co[2]);
					
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(2)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(2)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(2)][2] );				
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(2)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(2)]->co[1]/divCons);
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(2)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(2)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(2)]->co[2]);	
		}
		glEnd();
	glPopMatrix();
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	glDisable(GL_TEXTURE_GEN_S);                        // Disable Texture Coord Generation ( NEW )
	glDisable(GL_TEXTURE_GEN_T); 
	int width=15;
	int height=11;
	
	if (mode==SHADER)
	{
//		cout<<"here"<<endl;
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture (GL_TEXTURE_2D, envTexId); 
//		glBegin (GL_QUADS);
//			
//			glTexCoord2f (0.0, 0.0);
//			glVertex3f (-width,-height, -5);

//			glTexCoord2f (1.0,0.0);
//			glVertex3f (width, -height, -5);

//			glTexCoord2f (1.0, 1.0);
//			glVertex3f (width, height, -5);
//		
//			glTexCoord2f (0.0,1.0);
//			glVertex3f (-width, height, -5);
//		glEnd ();
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture (GL_TEXTURE_2D, potTexId); 

	
	}
	else
	{
		glBegin (GL_QUADS);
			glTexCoord2f (0.0, 0.0);
			glVertex3f (-width,-height, -5);

			glTexCoord2f (1.0,0.0);
			glVertex3f (width, -height, -5);

			glTexCoord2f (1.0, 1.0);
			glVertex3f (width, height, -5);

			glTexCoord2f (0.0,1.0);
			glVertex3f (-width, height, -5);
		glEnd ();
	}
//		
	
	glPushMatrix();
		
		
		
		glMultMatrixf(mat44); //reflection matrix;
		glTranslatef(5,0,0);
		glRotatef(fRotateAngle,0,1,0);
		glRotatef(fRotateAngle/2,1,0,0);
//		glScalef(.5,.5,.5);
		
		glBegin(GL_TRIANGLES);
		for (unsigned i=0; i<teapot.triangles.size();i++)
		{
			float divCons=1;
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(0)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(0)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(0)][2] );
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(0)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(0)]->co[1]/divCons);
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(0)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(0)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(0)]->co[2]);	
					
					
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(1)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(1)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(1)][2] );					
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(1)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(1)]->co[1]/divCons);					
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(1)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(1)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(1)]->co[2]);
					
			glNormal3f(	teapot.vertexNorms[teapot.triangles[i]->at(2)][0],
					teapot.vertexNorms[teapot.triangles[i]->at(2)][1],
					teapot.vertexNorms[teapot.triangles[i]->at(2)][2] );				
			glTexCoord2f (	teapot.vertices[teapot.triangles[i]->at(2)]->co[0]/divCons, 
					teapot.vertices[teapot.triangles[i]->at(2)]->co[1]/divCons);
			glVertex3f(	teapot.vertices[teapot.triangles[i]->at(2)]->co[0],
					teapot.vertices[teapot.triangles[i]->at(2)]->co[1],
					teapot.vertices[teapot.triangles[i]->at(2)]->co[2]);	
		}
		glEnd();
	glPopMatrix();

	
	
	
	
//	glFlush();	//for single buffer
	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
}

void reshape (int w, int h)
{
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h; 
	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.f,fAspect,0.001f,30.f); 
}



void keyboard(unsigned char key, int x, int y)
{
	// put your keyboard control here
	switch (key) 
	{
		case 27:// ESC hit, so quit
			printf("demonstration finished.\n");
			exit(0);
			break;
		case '1': mode = TEXMAP; break;
		case '2': mode = ENVMAP; break;
		case '3': mode = SHADER; ligthSwitch=1.0;envSwitch=1.0; bumpSwitch=1.0; initShader();  break;
		case '4': ligthSwitch=1.0-ligthSwitch; initShader(); break; 
		case '5': envSwitch=1.0-envSwitch; initShader();    break; 
		case '6': bumpSwitch=1.0-bumpSwitch; initShader();  break; 
		default:
			cout<<"nvm"<<endl;
	}
}

void mouse(int button, int state, int x, int y)
{
	// process your mouse control here
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		printf("push left mouse button.\n");
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		{
			exit(0);
		}
}


void timer(int v)
{
	fRotateAngle += 1.f; // change rotation angles
	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	glutTimerFunc(1000/nFPS,timer,v); // restart timer again
	tvar+=.03;
	if (tvar>2)
	{
		tvar=0;
	}
//	t= abs(1-tvar);
//	t=0;

	if (tvar<1)
	{
		t=tvar;
	}
	else
	{
		t=2-tvar;
	}
//	cout<<t << " " << tvar<<endl;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	// set up for double-buffering & RGB color buffer & depth test
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	
	//********************single buffer for testing
//	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); 	
	//****************************************
	
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ((const char*)"Hello Triangle");
 	glewInit();
	init(argv[1],argv[2]); // setting up user data & OpenGL environment

	// set up the call-back functions 
	glutDisplayFunc(display);  // called when drawing 
	glutReshapeFunc(reshape);  // called when change window size
	glutKeyboardFunc(keyboard); // called when received keyboard interaction
	glutMouseFunc(mouse);	    // called when received mouse interaction
	glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
	glutMainLoop(); // start the main message-callback loop
	cout<<"quit"<<endl;
	return 0;
}

