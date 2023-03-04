#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "SOIL.h"

#define R 1
#define B 2
#define G 3
#define T1 4
#define T2 5
#define T3 6
#define E 7
#define W 8
#define PI 3.1415927

// STERNIOTHS ELEFTHERIOS -- A.M: 2363.
// ASHMAKOPOULOS GEORGIOS -- A.M: 2216.


//------------------------------------------------------//
//                                //
//  													//
//------------------------------------------------------//

int h=520, w=600;
int vPressed=0, rPressed=0;
int oldx=-1,oldy=-1;
int counter=0;
// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx=0.0f,ly=0.0f,lz=-1.0f;
// XZ position of the camera
float x=0.0f,y=0.0f, z=5.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
int xOrigin = -1;
int moves=0;
int initialK;
int L,N,K,finalscore,hammers;
int ***maze=NULL;
double rotate_y=0;
double rotate_x=0;
bool disable=true;  //gia na mhn paizw ape3w apo ton maze
bool gameOver=false;
GLuint textures[3];
int num_of_jumps=0;
//------------------------------------------------------//
// Prototypes.                                         	//
//  													//
//------------------------------------------------------//

void loadTextures();
void draw_cylinder(GLfloat radius,GLfloat height);
void renderBitmapString(float x,float y,float z,void *font,char *string);
void restorePerspectiveProjection();
void setOrthographicProjection();
void readMazeFile();
void changeSize(int w, int h);
void destroyCube();
void computePos(float deltaMove);
void drawCubeColor(float r,float g,float b);
void drawCubeTexture(float r,float g,float b,GLuint texture);
void renderScene(void);
void processNormalKeys(unsigned char key, int xx, int yy);
void releaseKey(unsigned char key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);

//------------------------------------------------------//
// Main Program.                                       	//
//  													//
//------------------------------------------------------//


void loadTextures()
{

	textures[0] = SOIL_load_OGL_texture("T1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	textures[1] = SOIL_load_OGL_texture("T2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	textures[2] = SOIL_load_OGL_texture("T3.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void draw_cylinder(GLfloat radius,GLfloat height)
{
	GLfloat x              = 0.0;
	GLfloat y              = 0.0;
	GLfloat angle          = 0.0;
	GLfloat angle_stepsize = 0.1;

	/** Draw the tube */
	glColor3f(1,1,0);
	glBegin(GL_QUAD_STRIP);
	angle = 0.0;
	while( angle < 2*PI ) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y , height);
		glVertex3f(x, y , 0.0);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glVertex3f(radius, 0.0, 0.0);
	glEnd();

	/** Draw the circle on top of cylinder */
	glColor3f(1,1,0);
	glBegin(GL_POLYGON);
	angle = 0.0;
	while( angle < 2*PI ) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y , height);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glEnd();
}

void renderBitmapString(float x,float y,float z,void *font,char *string){

	char *c;
	glRasterPos3f(x, y,z);
	for (c=string; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

void restorePerspectiveProjection() {

	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, w, h, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

void readMazeFile(char *name){

	char C,buf[100];
	int i,j,k;
	FILE *infile=fopen(name,"r");
	if(infile==NULL) exit(0);

	C=fgetc(infile);
	C=fgetc(infile);
	fscanf(infile,"%d",&L);

	C=fgetc(infile);
	if(C=='\r')
		C=fgetc(infile);

	C=fgetc(infile);
	C=fgetc(infile);

	fscanf(infile,"%d",&N);
	C=fgetc(infile);
	if(C=='\r')
		C=fgetc(infile);

	C=fgetc(infile);
	C=fgetc(infile);
	fscanf(infile,"%d",&K);
	initialK=K;

	C=fgetc(infile);
	if(C=='\r')
		C=fgetc(infile);

	maze=(int ***)malloc(N*sizeof(int **));
	if(maze==NULL){
		printf("Error\n");
		exit(0);
	}

	for(i=0;i<N;i++){
		maze[i]=(int **)malloc(N*sizeof(int *));
		if(maze[i]==NULL){
			printf("Error\n");
			exit(0);
		}
	}

	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			maze[i][j]=(int *)malloc(L*sizeof(int));
			if(maze[i][j]==NULL){
				printf("Error\n");
				exit(0);
			}
		}
	}
	for(k=0;k<L;k++){
		fgets(buf,100,infile);
		for(i=0;i<N;i++){
			j=0;
			C=fgetc(infile);
			while(C!='\n' && C!='\r'){
				if(C=='R'){
					maze[i][j][k]=R;
				}
				else if(C=='G'){
					maze[i][j][k]=G;
				}
				else if(C=='B'){
					maze[i][j][k]=B;
				}
				else if(C=='1'){
					maze[i][j][k]=T1;
				}
				else if(C=='2'){
					maze[i][j][k]=T2;
				}
				else if(C=='3'){
					maze[i][j][k]=T3;
				}
				else if(C=='E'){
					maze[i][j][k]=E;
				}
				else if(C=='W'){
					maze[i][j][k]=W;
				}
				else if(C==' '){
					j++;
				}
				C=fgetc(infile);
			}
			if(C=='\r')
				C=fgetc(infile);
		}
	}
}

void changeSize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void destroyCube() {

	float deltaMove=1;

	float xx =x+ deltaMove *lx;
	float zz =z+ deltaMove *lz;
	if(xx>=0 && zz>=0 && (int)(xx)<N && (int)(zz)<N && maze[(int)(xx)][(int)(zz)][(int)(y)]!=E && K>0){
		maze[(int)(xx)][(int)(zz)][(int)(y)]=E;
		K--;
	}
	renderScene();
}

void computePos(float deltaMove) {

		int i,j,found=0;

		if(fabs(lx)>0.01 && fabs(lz)>0.01)
			return;
		lx=round(lx);
		lz=round(lz);
		float xx =x+ deltaMove *lx;
		float zz =z+ deltaMove *lz;

		if(xx>=0 && zz>=0 && (xx)<=N-1 && (zz)<=N-1 && (maze[(int)(xx)][(int)(zz)][(int)(y)]==E || maze[(int)(xx)][(int)(zz)][(int)(y)]==W)){
			x=xx;
			z=zz;
			moves++;
			if(maze[(int)(xx)][(int)(zz)][(int)(y)]==W){
				for(i=0;i<N;i++){
					for(j=0;j<N;j++){
						if(maze[i][j][(int)(y)]==W){
							if((int)x!=i || (int)z!=j){
								x=i;
								z=j;
								found=1;
							}
						}
						if(found==1){
							break;
						}
					}
				}
			}
			while((int)y>0 && maze[(int)xx][(int)zz][(int)y-1]==E){
				y--;
			}
		}
}

void drawCubeColor(float r,float g,float b){

	// FRONT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f(  0.5, -0.5, -0.5 );
	glVertex3f(  0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// BACK side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f(  0.5, -0.5, 0.5 );
	glVertex3f(  0.5,  0.5, 0.5 );
	glVertex3f( -0.5,  0.5, 0.5 );
	glVertex3f( -0.5, -0.5, 0.5 );
	glEnd();

	// RIGHT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f( 0.5, -0.5, -0.5 );
	glVertex3f( 0.5,  0.5, -0.5 );
	glVertex3f( 0.5,  0.5,  0.5 );
	glVertex3f( 0.5, -0.5,  0.5 );
	glEnd();

	// LEFT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// TOP side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f(  0.5,  0.5,  0.5 );
	glVertex3f(  0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glEnd();

	// BOTTOM side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glVertex3f(  0.5, -0.5, -0.5 );
	glVertex3f(  0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

}

void drawCubeTexture(float r,float g,float b,GLuint texture){

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	// FRONT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f(  0.5, -0.5, -0.5 );
	glTexCoord2i(0,1);glVertex3f(  0.5,  0.5, -0.5 );
	glTexCoord2i(1,1);glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2i(1,0);glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// BACK side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f(  0.5, -0.5, 0.5 );
	glTexCoord2i(0,1);glVertex3f(  0.5,  0.5, 0.5 );
	glTexCoord2i(1,1);glVertex3f( -0.5,  0.5, 0.5 );
	glTexCoord2i(1,0);glVertex3f( -0.5, -0.5, 0.5 );
	glEnd();

	// RIGHT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f( 0.5, -0.5, -0.5 );
	glTexCoord2i(0,1);glVertex3f( 0.5,  0.5, -0.5 );
	glTexCoord2i(1,1);glVertex3f( 0.5,  0.5,  0.5 );
	glTexCoord2i(1,0);glVertex3f( 0.5, -0.5,  0.5 );
	glEnd();

	// LEFT side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f( -0.5, -0.5,  0.5 );
	glTexCoord2i(0,1);glVertex3f( -0.5,  0.5,  0.5 );
	glTexCoord2i(1,1);glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2i(1,0);glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();

	// TOP side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f(  0.5,  0.5,  0.5 );
	glTexCoord2i(0,1);glVertex3f(  0.5,  0.5, -0.5 );
	glTexCoord2i(1,1);glVertex3f( -0.5,  0.5, -0.5 );
	glTexCoord2i(1,0);glVertex3f( -0.5,  0.5,  0.5 );
	glEnd();

	// BOTTOM side.
	glBegin(GL_POLYGON);
	glColor3f( r, g, b );
	glTexCoord2i(0,0);glVertex3f(  0.5, -0.5, -0.5 );
	glTexCoord2i(0,1);glVertex3f(  0.5, -0.5,  0.5 );
	glTexCoord2i(1,1);glVertex3f( -0.5, -0.5,  0.5 );
	glTexCoord2i(1,0);glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void renderScene(void) {

	char s[100];
	int i,j,k,score;
	if (deltaMove )
		computePos(deltaMove);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if(rPressed%5+1==1){
		gluLookAt(	x, L+10, z,
				x+lx, y,  z+lz,
	0.0f, 1.0f,  0.0f);
	}
	else if(rPressed%5+1==2){
		gluLookAt(	x+N+10, y,  z,
				x, y, z,
	0.0f, 1.0f,  0.0f);
	}
	else if(rPressed%5+1==3){
		gluLookAt(	x, y,  z+N+10,
				x, y, z,
	0.0f, 1.0f,  0.0f);
	}
	else if(rPressed%5+1==4){
		gluLookAt(	x-N-10, y,  z,
				x, y, z,
	0.0f, 1.0f,  0.0f);
	}
	else if(rPressed%5+1==5){
		gluLookAt(	x, y,  z-N-10,
				x, y, z,
	0.0f, 1.0f,  0.0f);
	}
	else{
		if(vPressed%2==1){
			gluLookAt(	x, y, z,
					x+lx, y+ly,  z+lz,
					0.0f, 1.0f,  0.0f);
		}
		else{
			gluLookAt(	x, L+10, z,
						x+lx, y,  z+lz,
				0.0f, 1.0f,  0.0f);
		}
	}

	// Draw ground
	glColor3f(1,1,0);

	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-100.0f, -0.5f, -100.0f);
		glVertex3f(-100.0f, -0.5f,  100.0f);
		glVertex3f( 100.0f, -0.5f,  100.0f);
		glVertex3f( 100.0f, -0.5f, -100.0f);
	glEnd();

	for (i = 0 ; i < N ; i++){
		for(j = 0 ; j < N ; j++){
			for(k = 0 ; k < L ; k++){
				glTranslatef(i,k,j);
				if (maze[i][j][k] == R){
					drawCubeColor(1.0f,0.0f,0.0f);
				}else if(maze[i][j][k] == G){
					drawCubeColor(0.0f,1.0f,0.0f);
				}else if(maze[i][j][k] == B){
					drawCubeColor(0.0f,0.0f,1.0f);
				}
				else if(maze[i][j][k] == T1){
					drawCubeTexture(1.0f,1.0f,1.0f,textures[0]);
				}
				else if(maze[i][j][k] == T2){
					drawCubeTexture(1.0f,1.0f,1.0f,textures[1]);
				}
				else if(maze[i][j][k] == T3){
					drawCubeTexture(1.0f,1.0f,1.0f,textures[2]);
				}
				else if (maze[i][j][k] == W){
					drawCubeColor(0.0f,0.0f,0.0f);
				}
				glTranslatef(-i,-k,-j);
			}
		}
	}

	if(vPressed%2!=1){
		glTranslatef(x,y,z);
		draw_cylinder(0.3, 0.5);
		glTranslatef(-x,-y,-z);
	}
	score=N*N -moves*10 -(initialK - K)*50;

	sprintf(s,"Score= %d",score);
	glColor3f(1,0,0);
	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(5,30,0,GLUT_BITMAP_HELVETICA_12,s);
	sprintf(s,"Hammer= %d",K);
	renderBitmapString(5,50,0,GLUT_BITMAP_HELVETICA_12,s);

	glColor3f(0,0,1);
	if(gameOver==true){
		sprintf(s,"Game Over! Press a to restart or esc to exit");
		renderBitmapString(5,90,0,GLUT_BITMAP_HELVETICA_12,s);

	}
	glPopMatrix();
	restorePerspectiveProjection();
	glFlush();
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int xx, int yy) {

	if(key == 27){
		exit(0);
	}
        else if (key == 'X' || key == 'x'){
        		gameOver=true;
			rPressed=-1;
			printf("Game over!\n");
        }
        if(gameOver==false){
		if(key == 'R' || key == 'r'){
			// Rotate the camera.
				rPressed++;
		}else if(key == 'V' || key == 'v'){
				vPressed++;
				rPressed=-1;
			// Move the camera into the maze.
		}else if(key == 32){
			// Spacebar.
			if(num_of_jumps==0 && (int)y<L && maze[(int)x][(int)z][(int)y+1]==E){
				y++;
				num_of_jumps=1;
			}
			if((int)y==L){
				gameOver=true;
			}
		}
		else if(key == 'W' || key == 'w'){
			deltaMove = 1.0f;
			num_of_jumps=0;
		}
		else if(key == 'S' || key == 's'){
			deltaMove = -1.0f;
			num_of_jumps=0;
		}
		else if(key == 'H' || key == 'h'){
			destroyCube();
		}
		else if(key == 'E' || key == 'e'){

			if((int)y==L){
				gameOver=true;
			}
		}
	}
	else{
		if(key=='a' || key=='A'){
			K=initialK;
			do{
				x=rand()%N;
				z=rand()%N;
				y=0;
			}while(maze[(int)x][(int)z][(int)y]!=E);
			gameOver=false;
		}
	}
        renderScene();
}

void releaseKey(unsigned char key, int x, int y) {

        switch (key) {

	     case 'W':
		     deltaMove = 0;
		     break;
	     case 'w':
		     deltaMove = 0;
		     break;
	     case 'S':
		     deltaMove = 0;
		     break;
	     case 's':
		     deltaMove = 0;
		     break;

        }
}

void mouseMove(int x, int y) {
         // this will only be true when the left button is down
         if (xOrigin >= 0) {
			// update deltaAngle
			deltaAngle = (x - xOrigin) * 0.001f;
			// update camera's direction
			if(oldx!=-1 && oldy!=-1){
				if(abs(oldx-x)>abs(oldy-y)){
					lx = sin(angle + deltaAngle);
					lz = -cos(angle + deltaAngle);
					printf("lx=%f lz=%f\n",lx,lz);
				}
			}
	}
	renderScene();
}

void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
			oldx=-1;
			oldy=-1;
		}
		else  {// state = GLUT_DOWN
			xOrigin = x;
			oldx=x;
			oldy=y;
		}
	}
}

int main(int argc, char **argv) {

	if(argc<2){
		printf("Give the file\n");
		exit(0);
	}
	readMazeFile(argv[1]);
	do{
		x=rand()%N;
		z=rand()%N;
		y=0;
	}while(maze[(int)x][(int)z][(int)y]!=E);
	printf("|lx| or |lz| must be close to zero (<0.01) to move!!!\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(w,h);
	glutCreateWindow("Project 2 ~ 3D Maze!");
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(releaseKey);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	loadTextures();
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 1;
}
