#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>


#define WIDTH 580
#define HEIGHT 640
#define PI 3.14159265
#define PURPLE 0
#define ORANGE 1
#define RED 2
#define frand()			((float)rand()/RAND_MAX)
#define M_PI			3.14159265
#define MAX_PARTICULAS  1500

using std::ifstream;
using std::ios;

const GLfloat posXi = 14.5f, posYi = 8.0f;
const GLfloat posg1Xi = 27.0f, posg1Yi = 8.0f;
const GLfloat posg2Xi = 2.0f, posg2Yi = 30.0f;
const GLfloat posg3Xi = 27.0f, posg3Yi = 30.0f;
const GLfloat posX0 = 14.5f, posY0 = 17;

/////////////////////////////
//Casa das Vari·veis Globais
GLfloat posX = posXi, posY = posYi;
GLfloat posg1X = posg1Xi, posg1Y = posg1Yi;
GLfloat posg2X = posg2Xi, posg2Y = posg2Yi;
GLfloat posg3X = posg3Xi, posg3Y = posg3Yi;
GLfloat w1;
GLfloat h1;

GLuint texname;

typedef struct {
	float	life;		// vida
	float	fade;		// fade
	float	r, g, b;    // color
	GLfloat x, y, z;    // posição
	GLfloat vx, vy, vz; // velocidade 
    GLfloat ax, ay, az; // aceleração
} Particle;

Particle particula[1500];

int direction = 4, directiong1 = 2, directiong2 = 4, directiong3 = 4, direction0 = 1;
int teta = 0;
int cameraNo = 1;
int nextDirection;
int panic = 0, ghost1alive = 0, ghost2alive = 0, ghost3alive = 0;
int lives = 3, score = 0, killingspree = 0, ghostskilled = 0;
int _texWidth, _texHeight;

float final_time, initial_time = 0.0;
float dt;
float dtp = 0.2;

bool daynnight = false;
bool pause = false;
bool drawkillingspree = false;

GLfloat AmbientLight[] = {0.0, 0.0, 0.0, 0.0};
GLfloat DiffuseLight[] = {0.7, 0.7, 0.7, 1.0};
GLfloat SpecularLight[] = {1.0, 1.0, 1.0, 1.0};
GLfloat LightDirection0[] = {0.0, -5.0, 5.0, 0.0};
GLfloat directionlight[] = {1.0,0.0,-1.4};

GLfloat material_se_rest = 75;
GLfloat material_se_op = 5;
GLfloat material_se_met = 100;
GLfloat material_ke[] = {0.0f,0.0f,0.0f,0.0f};

// yellow
GLfloat material_ka_y[] = {1.0f,1.0f,0.0f,1.0f};
GLfloat material_kd_y[] = {1.0f,1.0f,0.0f,1.0f};
GLfloat material_ks_y[] = {0.0f,0.0f,0.0f,1.0f};

// blue (wall)
GLfloat material_ka_bw[] = {0.0f,0.0f,1.0f,1.0f};
GLfloat material_kd_bw[] = {0.0f,0.0f,1.0f,1.0f};
GLfloat material_ks_bw[] = {0.0f,0.0f,1.0f,1.0f};

// small spheres
GLfloat material_ka_lsphere[] = {0.0f,0.0f,0.0f,0.0f};
GLfloat material_kd_lsphere[] = {0.5f,0.8f,0.7f,1.0f};
GLfloat material_ks_lsphere[] = {1.0f,1.0f,1.0f,1.0f};

// big spheres
GLfloat material_ka_bsphere[] = {0.0f,0.0f,0.0f,0.0f};
GLfloat material_kd_bsphere[] = {1.0f,1.0f,0.5f,1.0f};
GLfloat material_ks_bsphere[] = {1.0f,1.0f,1.0f,1.0f};

// blue
GLfloat material_ka_b[] = {0.0,0.0,0.0,1.0};
GLfloat material_kd_b[] = {0.0,0.0,1.0,1.0};
GLfloat material_ks_b[] = {0.0,0.0,0.5,1.0};

// white
GLfloat material_ka_w[] = {1.0,1.0,1.0,1.0};
GLfloat material_kd_w[] = {1.0,1.0,1.0,1.0};
GLfloat material_ks_w[] = {1.0,1.0,1.0,1.0};

// orange
GLfloat material_ka_o[] = {1.0f,0.4f,0.0f,1.0f};
GLfloat material_kd_o[] = {1.0f,0.3f,0.0f,1.0f};
GLfloat material_ks_o[] = {1.0f,1.0f,1.0f,1.0f};


// purple
GLfloat material_ka_p[] = {0.5f,0.0f,0.5f,1.0f};
GLfloat material_kd_p[] = {0.5f,0.0f,0.7f,1.0f};
GLfloat material_ks_p[] = {1.0f,1.0f,1.0f,1.0f};

// red
GLfloat material_ka_r[] = {1.0f,0.0f,0.0f,1.0f};
GLfloat material_kd_r[] = {0.7f,0.0f,0.0f,1.0f};
GLfloat material_ks_r[] = {1.0f,0.0f,0.0f,1.0f};

// black
GLfloat material_ka_br[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat material_kd_br[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat material_ks_br[] = {0.0f,0.0f,0.0f,1.0f};

// green
GLfloat material_ka_g[] = {0.0f,0.0f,0.0f,1.0f};
GLfloat material_kd_g[] = {0.2f,1.0f,0.6f,1.0f};
GLfloat material_ks_g[] = {0.3f,0.3f,0.3f,1.0f};

float l[32][29] = {
		{ 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,2.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,2.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,3.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,1.0 },
		{ 1.0,0.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,0.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,0.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0 },
		{ 1.0,0.0,2.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,0.0,1.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,2.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,0.0,0.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,0.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0 },
		{ 1.0,0.0,0.0,0.0,3.0,0.0,0.0,3.0,1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.0,3.0,0.0,0.0,3.0,0.0,0.0,1.0 },
		{ 1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,2.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,2.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,0.0,3.0,1.0,1.0,1.0,1.0,0.0,3.0,1.0,1.0,1.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,3.0,1.0,0.0,3.0,0.0,0.0,0.0,0.0,0.0,3.0,0.0,0.0,0.0,0.0,3.0,1.0 },
		{ 1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0,0.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,1.0 },
		{ 1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0 },
	};

// VÈrtices da cabeÁa
GLfloat hv1[]={0.0f,0.0f,1.7f};
GLfloat hv2[]={1.0f, 0.0f, 1.1f};
GLfloat hv3[]={cos(22.5*PI/180), -(sin(22.5*PI/180)), 1.1f};

// VÈrtices do corpo
GLfloat bv2[] = {1.0f, 0.0f, 0.0f};
GLfloat bv3[] = {cos(22.5*PI/180), -(sin(22.5*PI/180)), 0.0f};

// VÈrtices perna 1
GLfloat f1v1[]={1.0f, 0.0f, -0.5f};
GLfloat f1v2[]={1.0f, 0.0f, 0.0f};
GLfloat f1v3[]={(cos(22.5*PI/180)), -(sin(22.5*PI/180)), 0.0f};
GLfloat f1v4[]={(cos(45*PI/180.0)), -(sin(45*PI/180.0)), 0.0f};
GLfloat f1v5[]={0.0f, 0.0f, 0.0f};
GLfloat f1v6[]={(cos(45*PI/180.0)), (sin(45*PI/180.0)), 0.0f};
GLfloat f1v7[]={(cos(22.5*PI/180)), (sin(22.5*PI/180)), 0.0f};
GLfloat f1v8[]={1.0f, 0.0f, 0.0f};
// VÈrtices perna 2
GLfloat f2v1[]={-1.0f, 0.0f, 0.5f};
GLfloat f2v2[]={-1.0f, 0.0f, 0.0f};
GLfloat f2v3[]={-(cos(22.5*PI/180)), (sin(22.5*PI/180)), 0.0f};
GLfloat f2v4[]={-(cos(45*PI/180.0)), (sin(45*PI/180.0)), 0.0f};
GLfloat f2v5[]={0.0f, 0.0f, 0.0f};
GLfloat f2v6[]={-(cos(45*PI/180.0)), -(sin(45*PI/180.0)), 0.0f};
GLfloat f2v7[]={-(cos(22.5*PI/180)), -(sin(22.5*PI/180)), 0.0f};
GLfloat f2v8[]={-1.0f, 0.0f, 0.0f};
// VÈrtices perna 3
GLfloat f3v1[]={0.0f, 1.0f, -0.5f};
GLfloat f3v2[]={0.0f, 1.0f, 0.0f};
GLfloat f3v3[]={(cos(67.5*PI/180)), (sin(67.5*PI/180)), 0.0f};
GLfloat f3v4[]={(cos(45*PI/180.0)), (sin(45*PI/180.0)), 0.0f};
GLfloat f3v5[]={0.0f, 0.0f, 0.0f};
GLfloat f3v6[]={-(cos(45*PI/180.0)), (sin(45*PI/180.0)), 0.0};
GLfloat f3v7[]={-(cos(67.5*PI/180)), (sin(22.5*PI/180)), 0.0f};
GLfloat f3v8[]={0.0f, 1.0f, 0.0f};
// VÈrtices perna 4
GLfloat f4v1[]={0.0f, -1.0f, -0.5f};
GLfloat f4v2[]={0.0f, -1.0f, 0.0f};
GLfloat f4v3[]={-(cos(67.5*PI/180)), -(sin(67.5*PI/180)), 0.0f};
GLfloat f4v4[]={-(cos(45*PI/180.0)), -(sin(45*PI/180.0)), 0.0f};
GLfloat f4v5[]={0.0f, 0.0f, 0.0f};
GLfloat f4v6[]={(cos(45*PI/180.0)), -(sin(45*PI/180.0)), 0.0f};
GLfloat f4v7[]={(cos(67.5*PI/180)), -(sin(22.5*PI/180)), 0.0f};
GLfloat f4v8[]={0.0f, 1.0f, 0.0f};


//////////////////////////////



void setAmbient(){
 if(daynnight){
  material_ka_y[0] = 0.0;
  material_ka_y[1] = 0.0;
  material_ka_bw[2] = 0.0;
  material_ka_w[0] = 0.0;
  material_ka_w[1] = 0.0;
  material_ka_w[2] = 0.0;
  material_ka_o[0] = 0.0;
  material_ka_o[1] = 0.0;
  material_ka_p[0] = 0.0;
  material_ka_p[2] = 0.0;
  material_ka_r[0] = 0.0;
 }else{
  material_ka_y[0] = 1.0;
  material_ka_y[1] = 1.0;
  material_ka_bw[2] = 1.0;
  material_ka_w[0] = 1.0;
  material_ka_w[1] = 1.0;
  material_ka_w[2] = 1.0;
  material_ka_o[0] = 1.0;
  material_ka_o[1] = 0.4;
  material_ka_p[0] = 0.5;
  material_ka_p[2] = 0.5;
  material_ka_r[0] = 1.0;
 }
}

void iterate(int value)
{
	float h; 

	/* MÈtodo de Euler de integraÁ„o de eq. diferenciais ordin·rias 
		h representa o step de tempo; dv/dt = a; dx/dt = v; e conhecem-se os valores iniciais de x e v */

	h=0.125f;
	for (int i=0; i<MAX_PARTICULAS; i++)
	{
		particula[i].x += (h*particula[i].vx);
		particula[i].y += (h*particula[i].vy);
		particula[i].z += (h*particula[i].vz);
		particula[i].vx += (h*particula[i].ax);
		particula[i].vy += (h*particula[i].ay);
		particula[i].vz += (h*particula[i].az);
		particula[i].life -= particula[i].fade;
	}
 glutPostRedisplay();
 glutTimerFunc(33,iterate,1);
}

void iniParticulas(void)
{
 GLfloat v, theta, phi;
 int i;

 for(i=0; i<MAX_PARTICULAS; i++)
    {
	v = 0.6*frand()+0.2;
    phi = frand()*M_PI;
    theta = 2*frand()*M_PI;
    particula[i].x = posXi;
    particula[i].y = posYi;
    particula[i].z = 1.0f;
    particula[i].vx = v * cos(theta) * sin(phi);
    particula[i].vy = v * cos(phi);
    particula[i].vz = v * sin(theta) * sin(phi);
	particula[i].ax =  0.0f; /* simular um pouco de vento */
    particula[i].ay = 0.0f; /* simular a aceleraÁ„o da gravidade */
    particula[i].az =  -1.0f;
	
	/* tom amarelado que vai ser multiplicado pela textura que varia entre branco e preto */
	particula[i].r = 0.882f;
	particula[i].g = 0.552f;	
	particula[i].b = 0.211f;

	particula[i].life = 1.0f;		/* vida inicial */                
	particula[i].fade = 0.07f;	    /* step de decrÈscimo da vida para cada iteraÁ„o */
	}
  glutTimerFunc(33,iterate,1);
}


int readlong(ifstream* file){
	char ch1, ch2, ch3, ch4;
	file->get(ch1);
	file->get(ch2);
	file->get(ch3);
	file->get(ch4);

	int mask = 0xFF;

	return (((ch4&mask)*256+(ch3&mask))*256+(ch2&mask))*256+(ch1&mask);




}

char* LoadBMP(const char * filename){
	char ch1, ch2, ch3;
	ifstream* file= new ifstream(filename,ios::binary|ios::in);
	ch1 = ch2 = '0';
	file->get(ch1); file->get(ch2);
	int total = readlong( file);
	if ( file == NULL )
	file->seekg(ios::beg+10);
	int offset = readlong(file);
	//file->seekg(ios::cur+4);
	int bsize = readlong( file);
	_texWidth = readlong(file);
	_texHeight = readlong(file);

	char* image = new char[3*_texWidth*_texHeight];


	file->seekg(ios::beg+offset);
	int idx = 0;

	for( int i = 0; i< (_texWidth*_texHeight); i++){
		file->get( ch3);file->get(ch2);file->get(ch1);
		image[idx] = ch1; idx++;
		image[idx] = ch2; idx++;
		image[idx] = ch3; idx++;
	}
	file->close();
	delete file;
	return image;
}	

void LoadTexture(const char * bitmap_file){

	char * _image = LoadBMP(bitmap_file);

	glGenTextures(1, &texname);
	glBindTexture(GL_TEXTURE_2D, texname);
	//copy data from bitmap into texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texWidth, _texHeight,
				 0, GL_RGB, GL_UNSIGNED_BYTE, _image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); /* n„o era necess·rio pois este comando È executado por omissão */
	//free the bit

}

void DrawCube(float scaleX, float scaleY, float scaleZ, float posX, float posY){
	glPushMatrix();
	glTranslatef(posX,posY,0);
	glScalef(scaleX,scaleY,scaleZ);
	glutSolidCube(1.0f);
	glPopMatrix();
}

void DrawBodyGhost(int color){
	glPushMatrix();
	if(panic > 0){
		glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_w);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_w);
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_w);
		glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
		glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	}else{
		if(color == ORANGE){
			glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_o);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_o);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_o);
			glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
			glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
		}
		if(color == RED){
			glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_r);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_r);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_r);
			glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
			glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
		}
		if(color == PURPLE){
			glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_p);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_p);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_p);
			glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
			glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
		}
	}
	for(float omega = 0; omega <= 360 ;omega+=22.5){
		glPushMatrix();
		glRotatef(omega,0,0,1);
		glBegin(GL_TRIANGLES);
		glNormal3f(0.511822f, -0.101808f, 0.853037f);
		glVertex3fv(hv1);
		glNormal3f(1.492607f, -0.296898f, 0.853037f);
		glVertex3fv(hv3);
		glNormal3f(1.492607f, -0.296898f, 0.853037f);
		glVertex3fv(hv2);
		glEnd();
		glBegin(GL_POLYGON);
		glNormal3f(1.492607f, -0.296898f, 0.853037f);
		glVertex3fv(hv3);
		glNormal3f(1.492607f, -0.296898f, 0.853037f);
		glVertex3fv(hv2);
		glNormal3f(0.980785f, -0.19509f, 0.0f);
		glVertex3fv(bv2);
		glNormal3f(0.980785f, -0.19509f, 0.0f);
		glVertex3fv(bv3);
		glEnd();
		glPopMatrix();
	}
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(1,0,0);
	glVertex3fv(f1v1);
	glNormal3f(1,0,0);
	glVertex3fv(f1v2);
	glNormal3f(1,0,0);
	glVertex3fv(f1v3);
	glNormal3f(1,0,0);
	glVertex3fv(f1v4);
	glNormal3f(1,0,0);
	glVertex3fv(f1v5);
	glNormal3f(1,0,0);
	glVertex3fv(f1v6);
	glNormal3f(1,0,0);
	glVertex3fv(f1v7);
	glNormal3f(1,0,0);
	glVertex3fv(f1v8);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v1);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v2);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v3);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v4);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v5);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v6);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v7);
	glNormal3f(-1,0,0);
	glVertex3fv(f2v8);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,1,0);   
	glVertex3fv(f3v1);
	glNormal3f(0,1,0);
	glVertex3fv(f3v2);
	glNormal3f(0,1,0);
	glVertex3fv(f3v3);
	glNormal3f(0,1,0);
	glVertex3fv(f3v4);
	glNormal3f(0,1,0);
	glVertex3fv(f3v5);
	glNormal3f(0,1,0);
	glVertex3fv(f3v6);
	glNormal3f(0,1,0);
	glVertex3fv(f3v7);
	glNormal3f(0,1,0);
	glVertex3fv(f3v8);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v1);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v2);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v3);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v4);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v5);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v6);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v7);
	glNormal3f(0,-1,0);
	glVertex3fv(f4v8);
	glEnd();
	glPopMatrix();
}

void DrawEyesGhost(){
	// White
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_w);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_w);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_w);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glPushMatrix();
	glTranslated(cos(60.0*PI/180.0),sin(-60.00*PI/180.0),0.5f);
	glutSolidSphere(0.2,8,8);
	glPopMatrix();
	glTranslated(cos(120.0*PI/180.0),sin(-120.00*PI/180.0),0.5f);
	glutSolidSphere(0.2,8,8);
	glPopMatrix();

	// Green
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_g);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_g);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_g);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glPushMatrix();
	glTranslated(cos(60.0*PI/180.0),1.2*sin(-60.00*PI/180.0),0.5f);
	glutSolidSphere(0.1,8,8);
	glPopMatrix();
	glTranslated(cos(120.0*PI/180.0),1.2*sin(-120.00*PI/180.0),0.5f);
	glutSolidSphere(0.1,8,8);
	glPopMatrix();
}

void DrawGhost(int color, float px, float py){
	glPushMatrix();
	glTranslatef(px,py, 0.0f);
	DrawBodyGhost(color);
	glPushMatrix();
	if (py>posY)
		glRotatef((atan((posX-px)/(py-posY))*180.0/PI),0.0,0.0,1.0);
	else
		glRotatef(180-(atan((posX-px)/(posY-py))*180.0/PI),0.0,0.0,1.0);
	DrawEyesGhost();
	glPopMatrix();
	glPopMatrix();
}

void DrawEyes(){

	// White
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_w);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_w);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_w);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glPushMatrix();
	glTranslated(0.5,- 0.5,0.7);
	glutSolidSphere(0.2,8,8);
	glPopMatrix();
	glTranslated(0.5,0.5,0.7);
	glutSolidSphere(0.2,8,8);
	glPopMatrix();

	// Blue
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_b);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_b);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_b);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glPushMatrix();
	glTranslated(0.65,- 0.5, 0.7);
	glutSolidSphere(0.1,8,8);
	glPopMatrix();	
	glTranslated(0.65,0.5,0.7);
	glutSolidSphere(0.1,8,8);
	glPopMatrix();

	//Brows
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_br);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_br);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_br);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glPushMatrix();
	glTranslated(0.4,-0.5,1.1);
	glScalef(0.1,0.7,0.1);
	glutSolidCube(1.0f);
	glPopMatrix();
	glTranslated(0.4,0.5,1.1);
	glScalef(0.1,0.7,0.1);
	glutSolidCube(1.0f);
	glPopMatrix();
}

void DrawBody(){
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_y);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_y);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_y);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_rest);
	glutSolidSphere(1.0, 50, 50);
	glPopMatrix();
}

void DrawPacman(){
	glPushMatrix();
	glTranslatef(posX, posY, 0);
	glRotatef(teta, 0, 0, 1);
	DrawBody();
	DrawEyes();
	glPopMatrix();
}

void DrawLabyrinth(){

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_w);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_w);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_w);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_se_op);

	//Portao da Casa dos Fantasmas
	glDisable(GL_TEXTURE_2D);
	DrawCube(3, 0.01, 1, 14.5, 18.5);
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();

	for(int y = 0; y<32; y++){
		for(int x = 0; x<29; x++){
			if (l[y][x] == 1.0){
				if(!((y == 18  && x == 13)   ||
					(y == 18  && x == 14)  ||
					 (y == 18  && x == 15))){
				glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_bw);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_bw);
				glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_bw);
				glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
				glMaterialf(GL_FRONT, GL_SHININESS, material_se_op);
				glTranslatef((x+0.5),(y+0.5),0.0f);
				glutSolidCube(1.0f);
				glPopMatrix();
				glEnable(GL_TEXTURE_2D);
				}
			}
			if (l[y][x] == 3.0){
				glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_lsphere);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_lsphere);
				glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_lsphere);
				glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
				glMaterialf(GL_FRONT, GL_SHININESS, material_se_met);
				glTranslatef(x,y,0.0f);
				glutSolidSphere(0.2, 8, 8);
				glPopMatrix();
				glEnable(GL_TEXTURE_2D);
			}
			if (l[y][x] == 2.0){
				glDisable(GL_TEXTURE_2D);
				glPushMatrix();
				glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_bsphere);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_bsphere);
				glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_bsphere);
				glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
				glMaterialf(GL_FRONT, GL_SHININESS, material_se_met);
				glTranslatef(x,y,0.0f);
				glutSolidSphere(0.4, 8, 8);
				glPopMatrix();
				glEnable(GL_TEXTURE_2D);
			}
			glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, texname);
			glMaterialfv(GL_FRONT, GL_AMBIENT, material_ka_w);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material_kd_w);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_ks_w);
			glMaterialfv(GL_FRONT, GL_EMISSION, material_ke);
			glMaterialf(GL_FRONT, GL_SHININESS, material_se_op);
			glTranslatef((x+0.5),(y+0.5),-0.6f);
			glBegin(GL_TRIANGLE_STRIP);	/* 4 vÈrtices - 2 triangulos: v0 v1 v2; v1 v2 v3 */			        
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2d(1,1); 
			glVertex3f(1.0f,1.0f,-0.5f);
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2d(0,1);
			glVertex3f(0.0f,1.0f,-0.5f);       
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2d(1,0); 
			glVertex3f(1.0f,0.0f,-0.5);        
			glNormal3f(0.0,0.0,1.0);
			glTexCoord2d(0,0); 
			glVertex3f(0.0f,0.0f,-0.5);      
			glEnd();
			glPopMatrix();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void myReshape(GLsizei w, GLsizei h) {
	glViewport((w-WIDTH)/2, (h-HEIGHT)/2, WIDTH, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	w1=w+WIDTH;
	h1=h+HEIGHT;
}

void DrawHud()
{
	// Temporary disable lighting
	glDisable( GL_LIGHTING );

	// Our HUD consists of a simple rectangle
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0f, w1, 0.0f, h1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glColor3f( 1.0f, 1.0f, 0.0f );
	glRasterPos2i(20,1350);

	char message[] = "Pontuacao: ";
	int index = 0;
	for(index=0; message[index]!='\0'; index++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, message[index]);


	char pont[6];
	_itoa_s(score,pont,10);
	for(int i=0; pont[i]!='\0'; i++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, pont[i]);


	glRasterPos2i(1000,1350);

	char message2[] = "Vidas: ";
	int iter = 0;
	for(iter=0;message2[iter]!='\0';iter++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, message2[iter]);

	char lvs[6];
	_itoa_s(lives,lvs,10);
	for(int i=0; lvs[i]!='\0'; i++)
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, lvs[i]);
	// Reenable lighting
	glEnable( GL_LIGHTING );
}

void updateDirection(int key, int x, int y){

	switch(key){
	case GLUT_KEY_UP:
		nextDirection = 1; 
		break;
	case GLUT_KEY_LEFT:
		nextDirection = 2;  
		break;
	case GLUT_KEY_RIGHT:
		nextDirection = 3;  
		break;
	case GLUT_KEY_DOWN:
		nextDirection = 4;
		break;
	}

}

void setCamera(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch( cameraNo )
	{
	default:
	case 1:
		gluPerspective(70.0, ((float)WIDTH)/HEIGHT, 0.1,50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(posX, posY - 10, 10, posX, posY, 0.1, 0, 1, 0);
		break;
	case 2:
		gluPerspective(70.0, ((float)WIDTH)/HEIGHT, 0.1,50);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		switch(direction){
		case 1:
			gluLookAt(posX, posY - 5, 5, posX, posY, 0.1, 0, 1, 0);
			break;
		case 2:
			gluLookAt(posX+5,posY,5,posX,posY,0.1,-1,0,0);
			break;
		case 3:
			gluLookAt(posX-5,posY,5,posX,posY,0.1,1,0,0);
			break;
		case 4:
			 gluPerspective(45.0f,(GLfloat)WIDTH/(GLfloat)HEIGHT,0.1f,400.0f);
			 glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		 gluLookAt (0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
		}
		break;
	case 3:
		glOrtho(-0.0f, 29.0f, -0.0f, 35.0f, -2.0f, 2.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		break;
	case 4:
		gluLookAt (0.0, 0.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	}
}

void lightdirection(){
	directionlight[2] = -1.4f;
	switch(direction){
	case 1:
		directionlight[0] = 0.0f;
		directionlight[1] = 1.0f;
		break;
	case 2:
		directionlight[0] = -1.0f;
		directionlight[1] = 0.0f;
		break;
	case 3:
		directionlight[0] = 1.0f;
		directionlight[1] = 0.0f;
		break;
	case 4:
		directionlight[0] = 0.0f;
		directionlight[1] = -1.0f;
		break;
	}
}

void setLight(){
	GLfloat LightPosition[] = {posX,posY,2.0,1.0};
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	setAmbient();
	if(daynnight){
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		lightdirection();
		glLightfv(GL_LIGHT1, GL_AMBIENT, AmbientLight);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, DiffuseLight);
		glLightfv(GL_LIGHT1, GL_SPECULAR, SpecularLight);
		// SpotLight
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45); // 0~180
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0); // 0~128
		glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, directionlight);


	}else{
		//lighting set up
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
		//set lighting
		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
		//set the light direction
		glLightfv(GL_LIGHT0, GL_POSITION, LightDirection0);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 50);
	}
}

void handleKeys(unsigned char key, int x, int y){
	switch(key){
	case 'd':
		daynnight = !daynnight;
		break;
	case 'p':
		pause = !pause;
		break;
	case 'q':
		exit(0);
		break;
	case '1':
	case '2':
	case '3':
	case '4':
		int camera = key - '0';
		printf( "%c %d\n", key, camera);
		if ( camera != cameraNo )
		{
			cameraNo = camera;
			glutPostRedisplay();
		}
		break;
	}
}

void BackToNormal(int value){
	panic--;
	if (panic > 0)
		glutTimerFunc(1000,BackToNormal,0);
}

int crossroads(float pX, float pY, int dir, GLfloat dtg){
	int x,y, directiong;
	float posgX, posgY;
	posgX = pX;
	posgY = pY;
	directiong = dir;
	x = (int)(posgX+0.5);
	y = (int)(posgY+0.5);

	bool twentyfive = false;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool plefttog, prighttog, puptog, pdowntog;
	plefttog = false;
	prighttog = false;
	puptog = false;
	pdowntog = false;



	switch(directiong){
	case 1:
		if ((posgY < y) && ((posgY+dtg)>=y) && (l[y][x-2]!=1.0) && (l[y-1][x-2]!=1.0))
			left = true;

		if ((posgY < y) && ((posgY+dtg)>=y) && (l[y][x+1]!=1.0) && (l[y-1][x+1]!=1.0))
			right = true;

		if (((posgY+dtg)>y) && (l[y+1][x] == 1.0))
			down = true;

		break;
	case 2:
		if ((posgX > x) && ((posgX-dtg)<=x) && (l[y+1][x]!=1.0) && (l[y+1][x-1]!=1.0))
			up = true;

		if ((posgX > x) && ((posgX-dtg)<=x) && (l[y-2][x]!=1.0) && (l[y-2][x-1]!=1.0))
			down = true;

		if (((posgX-dtg)<x) && (l[y][x-2] == 1.0))
			right = true;

		break;
	case 3:
		if ((posgX < x) && ((posgX+dtg)>=x) && (l[y+1][x]!=1.0) && (l[y+1][x-1]!=1.0))
			up = true;		

		if ((posgX < x) && ((posgX+dtg)>=x) && (l[y-2][x]!=1.0) && (l[y-2][x-1]!=1.0))
			down = true;

		if (((posgX+dtg)>x) && (l[y][x+1] == 1.0))
			left = true;

		break;
	case 4:
		if ((posgY > y) && ((posgY-dtg)<=y) && (l[y][x-2]!=1.0) && (l[y-1][x-2]!=1.0))
			left = true;

		if ((posgY > y) && ((posgY-dtg)<=y) && (l[y][x+1]!=1.0) && (l[y-1][x+1]!=1.0))
			right = true;

		if (((posgY-dtg)<y) && (l[y-2][x] == 1.0))
			up = true;
		break;
	}

	if ((rand() % 4 + 1) == 1)
		twentyfive = true;
	else
		twentyfive = false;
	

	plefttog = ((posgX - posX) >= 0);
	prighttog = ((posgX - posX) <= 0);
	puptog = ((posgY - posY) <= 0);
	pdowntog = ((posgY - posY) >= 0);

	if (panic){
		if ((left && plefttog && twentyfive) || (left && prighttog && !twentyfive)){
			directiong = 2;
		}else if ((right && prighttog && twentyfive) || (right && plefttog && !twentyfive)){
			directiong = 3;
		}else if ((up && puptog && twentyfive) || (up && pdowntog && !twentyfive)){
			directiong = 1;
		}else if ((down && pdowntog && twentyfive) || (down && puptog && !twentyfive)){
			directiong = 4;
		}
	}else{
		if ((left && plefttog && !twentyfive) || (left && prighttog && twentyfive)){
			directiong = 2;
		}else if ((right && prighttog && !twentyfive) || (right && plefttog && twentyfive)){
			directiong = 3;
		}else if ((up && puptog && !twentyfive) || (up && pdowntog && twentyfive)){
			directiong = 1;
		}else if ((down && pdowntog && !twentyfive) || (down && puptog && twentyfive)){
			directiong = 4;
		}
	}

	switch(directiong){
	case 1:
		if (((posgY+dtg)>y) && (l[y+1][x] == 1.0)){
			if ((rand() % 2 + 1) == 1){
				directiong = 4;
			}else if(left){
				directiong = 2;
			}else{
				directiong = 3;
			}
		}
		break;
	case 2:
		if (((posgX-dtg)<x) && (l[y][x-2] == 1.0)){
			if ((rand() % 2 + 1) == 1){
				directiong = 3;
			}else if(up){
				directiong = 1;
			}else{
				directiong = 4;
			}
		}			
		break;
	case 3:
		if (((posgX+dtg)>x) && (l[y][x+1] == 1.0)){
			if ((rand() % 2 + 1) == 1){
				directiong = 2;
			}else if(up){
				directiong = 1;
			}else{
				directiong = 4;
			}
		}
		break;
	case 4:
		if (((posgY-dtg)<y) && (l[y-2][x] == 1.0)){
			if ((rand() % 2 + 1) == 1){
				directiong = 1;
			}else if(left){
				directiong = 2;
			}else{
				directiong = 3;
			}
		}
		break;
	}

	if(pX == (14.5) && pY > 17.0 && pY < 18.0){
		directiong = 1;
	}

	return directiong;
}

void updatePosGhost(int ghost){
	int x,y, directiong;
	float posgX, posgY;
	switch(ghost){
	case 0:
		x = (int)(posg1X+0.5);
		y = (int)(posg1Y+0.5);
		posgX = posg1X;
		posgY = posg1Y;
		directiong = directiong1;
		break;
	case 1:
		x = (int)(posg2X+0.5);
		y = (int)(posg2Y+0.5);
		posgX = posg2X;
		posgY = posg2Y;
		directiong = directiong2;
		break;
	case 2:
		x = (int)(posg3X+0.5);
		y = (int)(posg3Y+0.5);
		posgX = posg3X;
		posgY = posg3Y;
		directiong = directiong3;
		break;
	}
	GLfloat dtg;
	if (panic)
		dtg = ((2*dtp)/3);
	else dtg = dtp;

	if(directiong == 1){
		posgY += dtg;
	}
	if(directiong == 2){
		posgX -= dtg;
	}
	if(directiong == 3){
		posgX += dtg;

	}
	if(directiong == 4){
		posgY -= dtg;
	}

	switch(crossroads(posgX, posgY, directiong, dtg)){
	case 1:
		if (directiong != 1){
			directiong = 1;
			posgY = y;
			posgX = x;
		}		
		break;
	case 2:
		if (directiong != 2){
			directiong = 2;
			posgY = y;
			posgX = x;
		}
		break;
	case 3:
		if (directiong != 3){
			directiong = 3;
			posgY = y;
			posgX = x;
		}
		break;
	case 4:
		if (directiong != 4){
			directiong = 4;
			posgY = y;
			posgX = x;
		}
		break;
	}

	switch(ghost){
	case 0:
		posg1X = posgX;
		posg1Y = posgY;
		directiong1 = directiong;
		break;
	case 1:
		posg2X = posgX;
		posg2Y = posgY;
		directiong2 = directiong;
		break;
	case 2:
		posg3X = posgX;
		posg3Y = posgY;
		directiong3 = directiong;
		break;
	}
}

void updatePos(){
	int x,y;
	x = (int)(posX+0.5);
	y = (int)(posY+0.5);

	switch(nextDirection){
	case 1:
		if ( ((direction == 2) && ((posX-dtp)<x) && (l[y+1][x]!=1.0) && (l[y+1][x-1]!=1.0))
			|| ((direction == 3) && ((posX+dtp)>x) && (l[y+1][x]!=1.0) && (l[y+1][x-1]!=1.0))
			|| (direction == 4 && !(x==15 && y ==8))){
				direction = 1;
				teta = 90;
				posX = x;
		}		
		break;
	case 2:
		if ( ((direction == 4) && ((posY-dtp)<y) && (l[y][x-2]!=1.0) && (l[y-1][x-2]!=1.0))
			|| ((direction == 1) && ((posY+dtp)>y) && (l[y][x-2]!=1.0) && (l[y-1][x-2]!=1.0))
			|| (direction == 3)){
				direction = 2;
				teta = 180;
				posY = y;
		}
		break;
	case 3:
		if ( ((direction == 4) && ((posY-dtp)<y) && (l[y][x+1]!=1.0) && (l[y-1][x+1]!=1.0))
			|| ((direction == 1) && ((posY+dtp)>y) && (l[y][x+1]!=1.0) && (l[y-1][x+1]!=1.0))
			|| (direction == 2)){
				direction = 3;
				teta = 0;
				posY = y;
		}
		break;
	case 4:
		if ( ((direction == 2) && ((posX-dtp)<x) && (l[y-2][x]!=1.0) && (l[y-2][x-1]!=1.0))
			|| ((direction == 3) && ((posX+dtp)>x) && (l[y-2][x]!=1.0) && (l[y-2][x-1]!=1.0))
			|| (direction == 1)){
				direction = 4;
				teta = 270;
				posX = x;
		}
		break;
	}

	if(direction == 1)
		if (! (((posY+dtp)>y) && (l[y+1][x] == 1.0))){
			posY += dtp;
		} else {
			posY = y;
		}
		if(direction == 2)
			if (! (((posX-dtp)<x) && (l[y][x-2] == 1.0))){
				posX -= dtp;
			} else {
				posX = x;
			}
			if(direction == 3)
				if (! (((posX+dtp)>x) && (l[y][x+1] == 1.0))){
					posX += dtp;
				} else {
					posX = x;
				}
				if(direction == 4)
					if (! (((posY-dtp)<y) && (l[y-2][x] == 1.0))){
						posY -= dtp;
					} else {
						posY = y;
					}

					if (l[y][x] == 3.0){
						l[y][x] = 0.0;
						score += 10;
					}
					if (l[y][x] == 2.0){
						l[y][x] = 0.0;
						score+= 10;
						if (panic == 0){
							panic = 10;
							glutTimerFunc(1000,BackToNormal,0);
						}else{panic = 10;}
					}
}

void BackToLife1(int value){
	ghost1alive--;
	if (ghost1alive > 0)
		glutTimerFunc(1000,BackToLife1,0);
	else {
		posg1X = posX0;
		posg1Y = posY0;
		directiong1 = direction0;
	}
}

void BackToLife2(int value){
	ghost2alive--;
	if (ghost2alive > 0)
		glutTimerFunc(1000,BackToLife2,0);
	else {
		posg2X = posX0;
		posg2Y = posY0;
		directiong2 = direction0;
	}
}

void BackToLife3(int value){
	ghost3alive--;
	if (ghost3alive > 0)
		glutTimerFunc(1000,BackToLife3,0);
	else {
		posg3X = posX0;
		posg3Y = posY0;
		directiong3 = direction0;
	}
}

void resetPositions(){
	posX = posXi;
	posY = posYi;
	posg1X = posg1Xi;
	posg1Y = posg1Yi;
	posg2X = posg2Xi;
	posg2Y = posg2Yi;
	posg3X = posg3Xi;
	posg3Y = posg3Yi;
	direction = 4;
	directiong1 = 2;
	directiong2 = 4;
	directiong3 = 4;
}

void detectCollisions(){

	if ((panic != 0) && (ghost1alive == 0) && ((((posX-1)<(posg1X+1)) && (posY == posg1Y) && (posX>posg1X))||
		(((posX+1)>(posg1X-1)) && (posY == posg1Y) && (posX<posg1X))||
		(((posY-1)<(posg1Y+1)) && (posX == posg1X) && (posY>posg1Y))||
		(((posY+1)>(posg1Y-1)) && (posX == posg1X) && (posY<posg1Y)))){
		score += 200;
		ghost1alive = 5;
		killingspree++;
		glutTimerFunc(1000,BackToLife1,0);
	}

	if ((panic != 0) && (ghost2alive == 0) && ((((posX-1)<(posg2X+1)) && (posY == posg2Y) && (posX>posg2X))||
		(((posX+1)>(posg2X-1)) && (posY == posg2Y) && (posX<posg2X))||
		(((posY-1)<(posg2Y+1)) && (posX == posg2X) && (posY>posg2Y))||
		(((posY+1)>(posg2Y-1)) && (posX == posg2X) && (posY<posg2Y)))){
		score += 200;
		ghost2alive = 5;
		killingspree++;
		glutTimerFunc(1000,BackToLife2,0);
	}

	if ((panic != 0) && (ghost3alive == 0) && ((((posX-1)<(posg3X+1)) && (posY == posg3Y) && (posX>posg3X))||
		(((posX+1)>(posg3X-1)) && (posY == posg3Y) && (posX<posg3X))||
		(((posY-1)<(posg3Y+1)) && (posX == posg3X) && (posY>posg3Y))||
		(((posY+1)>(posg3Y-1)) && (posX == posg3X) && (posY<posg3Y)))){
		score += 200;
		ghost3alive = 5;
		killingspree++;
		glutTimerFunc(1000,BackToLife3,0);
	}

	if ((panic == 0) && (((ghost1alive == 0) && 
						((((posX-1)<(posg1X+1)) && (posY == posg1Y) && (posX>posg1X))||
						(((posX+1)>(posg1X-1)) && (posY == posg1Y) && (posX<posg1X))||
						(((posY-1)<(posg1Y+1)) && (posX == posg1X) && (posY>posg1Y))||
						(((posY+1)>(posg1Y-1)) && (posX == posg1X) && (posY<posg1Y)))) ||
						((ghost2alive == 0) && ((((posX-1)<(posg2X+1)) && (posY == posg2Y) && (posX>posg2X))||
						(((posX+1)>(posg2X-1)) && (posY == posg2Y) && (posX<posg2X))||
						(((posY-1)<(posg2Y+1)) && (posX == posg2X) && (posY>posg2Y))||
						(((posY+1)>(posg2Y-1)) && (posX == posg2X) && (posY<posg2Y)))) ||
						((ghost3alive == 0) && ((((posX-1)<(posg3X+1)) && (posY == posg3Y) && (posX>posg3X))||
						(((posX+1)>(posg3X-1)) && (posY == posg3Y) && (posX<posg3X))||
						(((posY-1)<(posg3Y+1)) && (posX == posg3X) && (posY>posg3Y))||
						(((posY+1)>(posg3Y-1)) && (posX == posg3X) && (posY<posg3Y)))))){
		lives--;
		killingspree = 0;
		//explosion?
		resetPositions();
	}
}

void resetKillingSpree(int value){
	drawkillingspree = false;
}

void setParticulas(){
	glDisable(GL_DEPTH_TEST);
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, texname);
 for (int i=0; i<MAX_PARTICULAS; i++)
	{
		if(particula[i].life > 0.0f) /* sÛ desenha as que ainda est„o vivas */
		{

			/* A vida da partÌcula representa o canal alpha da cor. Como o blend est· activo a cor final È a soma da cor rgb do fragmento multiplicada pelo
			alpha com a cor do pixel destino */

			glColor4f(particula[i].r,particula[i].g,particula[i].b, particula[i].life);

			glBegin(GL_TRIANGLE_STRIP);	/* 4 vÈrtices - 2 triangulos: v0 v1 v2; v1 v2 v3 */			        
			glTexCoord2d(1,1); 
			glVertex3f(particula[i].x+1.0f,particula[i].y+1.0f,particula[i].z);            
			glTexCoord2d(0,1); 
			glVertex3f(particula[i].x-1.0f,particula[i].y+1.0f,particula[i].z);       
			glTexCoord2d(1,0); 
			glVertex3f(particula[i].x+1.0f,particula[i].y-1.0f,particula[i].z);        
			glTexCoord2d(0,0); 
			glVertex3f(particula[i].x-1.0f,particula[i].y-1.0f,particula[i].z);      
			glEnd();	
		}
	}
 glEnable(GL_DEPTH_TEST);
}

void myDisplay(void) {

	detectCollisions();
	updatePos();
	if(ghost1alive == 0){
	updatePosGhost(0);
	}
	if(ghost2alive == 0){
	updatePosGhost(1);
	}
	if(ghost3alive == 0){
	updatePosGhost(2);
	}
	setCamera();
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	setLight();
	glEnable(GL_TEXTURE_2D);
	DrawLabyrinth();
	DrawPacman();
	if(ghost3alive == 0){
	DrawGhost(RED,posg3X,posg3Y);
	}
	if(ghost1alive == 0){
	DrawGhost(PURPLE,posg1X,posg1Y);
	}
	if(ghost2alive == 0){
	DrawGhost(ORANGE,posg2X,posg2Y);
	}
	if (killingspree == 3){
		glutTimerFunc(3000,resetKillingSpree,0);
		killingspree = 0;
		lives++;
	}
	//DrawHud();
	/*if(perdeu uma vida){*/
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);	        		
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		LoadTexture("particula.bmp");
		setParticulas();
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
		LoadTexture("chao.bmp");
	//}
	glutSwapBuffers();
}

void Timer(int value){

	final_time = glutGet(GLUT_ELAPSED_TIME); /* in milliseconds */
	dt = 0.001*(final_time - initial_time); /* in seconds */
	dt = dt*5; /* velocidade */
	initial_time = final_time;
	if(!pause){
		glutPostRedisplay();
	}
	glutTimerFunc(15,Timer,0);
}



int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize (600, 710);
	glutInitWindowPosition (-1, -1);
	glutCreateWindow("Pacman");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	LoadTexture("chao.bmp");
	iniParticulas();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutSpecialFunc(updateDirection);
	glutKeyboardFunc(handleKeys);
	glutTimerFunc(30,Timer,0);
	glutMainLoop();
	return 0;
}

