#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <cmath>
#include <climits>
#include "Five.h"

Game game;
Action act;
int ww, hh;
bool gameover = false;
Color winner;

int eyer = 600;
float eyet = 1.0;
int eyez = 600;

int gridSize = 40;

int scaleLeft = -180;
int scaleRight = 180;
int scaleBottom = -180;
int scaleTop = 180;
int scaleNear = 250;
int scaleFar = 1600;


typedef struct materialStruct {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} materialStruct;

GLUquadricObj* pawn;

GLfloat angle = 0;

void useMaterial(GLfloat R,GLfloat G,GLfloat B,GLfloat specular,GLfloat shrininess)
{
	materialStruct material = {
		{0.3*R, 0.3*G, 0.3*B, 1.0},
		{0.8*R, 0.8*G, 0.8*B, 1.0},
		{specular, specular, specular, 1.0},
		shrininess
	};
	glMaterialfv(GL_FRONT,GL_AMBIENT, material.ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE, material.diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material.specular);
	glMaterialf(GL_FRONT,GL_SHININESS,material.shininess);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearColor(0,0,0,1);
	pawn = gluNewQuadric();
	gluQuadricDrawStyle(pawn,GLU_FILL);
	
	GLfloat light0_pos[4] = {200*cos(angle), 200*sin(angle), 200, 1.0};
	GLfloat light0_dir[4] = {-200*cos(angle), -200*sin(angle), -200, 1.0};
	GLfloat light0_dif[4] = {1.0,1.0,1.0,1.0};
	GLfloat light0_amb[4] = {0.0,0.0,0.0,1.0};
	GLfloat light0_spec[4] = {1.0,1.0,1.0,1.0};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spec);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_pos);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_dif);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_dir);
}

void drawPawn(int x, int y, GLfloat R, GLfloat G, GLfloat B)
{
	GLfloat fx = x*gridSize - gridSize*7;
	GLfloat fy = y*gridSize - gridSize*7;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eyer*cos(eyet),eyer*sin(eyet),eyez,0,0,0,0,0,1);
	glScalef(1,1,0.6);
	glTranslatef(fx,fy,gridSize/2*0.8);
	useMaterial(R,G,B,0.2,32);
	gluSphere(pawn,gridSize/2*0.8,12,12);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void drawGame(const Game & game)
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	useMaterial(1,1,0,0.8,100);
	glNormal3f(0,0,1);
	glBegin(GL_POLYGON);
		glVertex3f(-gridSize*7.5,-gridSize*7.5,0);
		glVertex3f(gridSize*7.5,-gridSize*7.5,0);
		glVertex3f(gridSize*7.5,gridSize*7.5,0);
		glVertex3f(-gridSize*7.5,gridSize*7.5,0);
	glEnd();
	useMaterial(0,0,0,0,0);
	for(int i = 0; i<15; i++)
	{
		glBegin(GL_LINES);
			glVertex3f(i*gridSize-gridSize*7,-gridSize*7,0.1);
			glVertex3f(i*gridSize-gridSize*7,gridSize*7,0.1);
			glVertex3f(-gridSize*7,i*gridSize-gridSize*7,0.1);
			glVertex3f(gridSize*7,i*gridSize-gridSize*7,0.1);
		glEnd();
	}
	for(int i = 0; i<15; i++)
		for(int j = 0; j<15; j++)
		{
			if(game._State[i][j]==BLACK)
				drawPawn(j,i,0,0,0);
			else if(game._State[i][j]==WHITE)
				drawPawn(j,i,1,1,1);
		}
}

const int bufSize = 1024;

int selected = -1;
bool scenemoving = false;

void computer(int value)
{
	if(gameover) return;
	act = game.alphaBetaSearch((Color)value);
	if(!act.empty())
	{
		game._takeAction(act);
		glutPostRedisplay();
		
		int u = game._utility();
		if(u==100 || u==-100)
		{
			gameover = true;
			winner = u==100?BLACK:WHITE;
		}
	}
	//glutTimerFunc(1000,computer,1-value);
}

int mousex, mousey;

void mouse(int button, int state, int x, int y)
{
	mousex = x;
	mousey = y;
	if(gameover)
	{
		scenemoving = true;
		return;
	}
	if(state==GLUT_DOWN)
	{
		if(button==GLUT_LEFT_BUTTON)
		{
			GLint hits;
			GLuint selectBuf[bufSize];
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			glSelectBuffer(bufSize,selectBuf);
			glRenderMode(GL_SELECT);
			glInitNames();
			glPushName(0);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluPickMatrix((GLfloat)x,(GLfloat)(viewport[3]-y),0.1,0.1,viewport);
			GLfloat fw = ww;
			GLfloat fh = hh;
			if(ww>hh)
				glFrustum(scaleLeft*fw/fh,scaleRight*fw/fh,
						scaleBottom,scaleTop,scaleNear,scaleFar);
			else
				glFrustum(scaleLeft,scaleRight,
						scaleBottom*fh/fw,scaleTop*fh/fw,scaleNear,scaleFar);
			
			glPolygonMode(GL_FRONT,GL_FILL);
			for(int i = 0; i<15; i++)
				for(int j = 0; j<15; j++)
				{
					glLoadName(i*15+j);
					glBegin(GL_POLYGON);
						glVertex3f(i*gridSize-gridSize*7.5,j*gridSize-gridSize*7.5,0);
						glVertex3f(i*gridSize-gridSize*6.5,j*gridSize-gridSize*7.5,0);
						glVertex3f(i*gridSize-gridSize*6.5,j*gridSize-gridSize*6.5,0);
						glVertex3f(i*gridSize-gridSize*7.5,j*gridSize-gridSize*6.5,0);
					glEnd();
				}
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			
			hits=glRenderMode(GL_RENDER);
			if(hits>0)
			{
				GLint names, *ptr;
				ptr = (GLint *) selectBuf; 
				for (int i = 0; i < hits; i++)
				{
					names = *ptr;
					ptr+=3;
					for (int j = 0; j < names; j++)
					{
						selected = *ptr;
						ptr++;
					}
				}
			}
			else
			{
				scenemoving = true;
			}
			if(selected >= 0)
			{
				int tx = selected % 15;
				int ty = selected / 15;
				act = action(tx, ty, WHITE);
				
				if(game._takeAction(act))
				{
					glutPostRedisplay();
					int u = game._utility();
					if(u==100 || u==-100)
					{
						gameover = true;
						winner = u==100?BLACK:WHITE;
						return;
					}
					glutTimerFunc(1000,computer,0);
				}
			}
		}
	}
	else
		scenemoving = false;
	
}

void mousemove(int x, int y)
{
	if(scenemoving)
	{
		eyet -= (x - mousex)/500.0;
		if(eyet<0) eyet += 2 * M_PI;
		if(eyet>2*M_PI) eyet -= 2 * M_PI;
		
		eyez += (y - mousey);
		if(eyez > 1000) eyez = 1000;
		if(eyez < 300) eyez = 300;
		
		mousex = x;
		mousey = y;
		glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat fw = w;
	GLfloat fh = h;
	if(w>h)
		glFrustum(scaleLeft*fw/fh,scaleRight*fw/fh,
				scaleBottom,scaleTop,scaleNear,scaleFar);
	else
		glFrustum(scaleLeft,scaleRight,
				scaleBottom*fh/fw,scaleTop*fh/fw,scaleNear,scaleFar);
	
	glutPostRedisplay();
	ww = w;
	hh = h;
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyer*cos(eyet),eyer*sin(eyet),eyez,0,0,0,0,0,1);
	
	drawGame(game);
	
	if(gameover)
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(-0.4,0,0);
		glScalef(0.2,0.2,0);
		glColor3f(1,0,0);
		GLfloat fw = ww;
		GLfloat fh = hh;
		if(ww>hh)
			gluOrtho2D(scaleLeft*fw/fh,scaleRight*fw/fh,scaleBottom,scaleTop);
		else
			gluOrtho2D(scaleLeft,scaleRight,scaleBottom*fh/fw,scaleTop*fh/fw);
		//useMaterial(1,0,0,100);
		if(winner==BLACK)
		{
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'B');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'L');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'A');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'C');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'K');
		}
		else
		{
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'W');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'H');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'I');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'T');
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'E');
		}
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,' ');
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'W');
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'I');
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,'N');
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
	
	glutSwapBuffers();
}

void menu(int value)
{
	switch(value)
	{
	case 0:
		game = Game();
		gameover = false;
		act = action(7, 7 ,BLACK);
		game._takeAction(act);
		break;
	case 1:
		game._undo();
		game._undo();
		gameover = false;
		if(game._actlist.empty())
		{
			act = action(7,7,BLACK);
			game._takeAction(act);
		}
		else if(game._Turn==BLACK)
		{
			glutTimerFunc(1000,computer,0);
		}
	}
	glutPostRedisplay();
}

void idle()
{
	//glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(0));
	act = action(7, 7 ,BLACK);
	game._takeAction(act);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000,1000);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Five");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mousemove);
	//glutIdleFunc(idle);
	init();
	int id = glutCreateMenu(menu);
	glutSetMenu(id);
	glutAddMenuEntry("Restart",0);
	glutAddMenuEntry("Undo",1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
	return 0;
}
