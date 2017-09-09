//
//  Lighting model demo.cpp
//  Lighting model demo
//
//  Created by 陈相男 on 2016/11/12.
//  Last Modified     on 2016/11/12.
//  Copyright © 2016年 JoelChen. All rights reserved.
//

#include <stdio.h>
#include <Windows.h>
#include <GL\glut.h>
#include <math.h>
#ifndef PI
#define PI  3.14159
#endif


int oldx = 0;
int oldy = 0;

int cx = 0;
int cy = 0;

int _near = 4;     //镜头前距离
int _far = 20;     //镜头后距离

int projectStyle = 1;

int winWidth = 800;
int winHeight = 800;

int mod = 1;  //渲染模式

GLfloat angle = 0.0;   //旋转角度
GLfloat lastVector[3] = { 0.0, 0.0, 0.0 };
GLfloat axis[3] = { 0.0, 0.0, 0.0 };

int divideNum = 0;     //细分次数

GLfloat gasket_vertices[4][3] = {
	{ 0.0, 0.0, 1.0 },
	{ 0.0, 0.942809, -0.33333 },
	{ -0.816497, -0.471405, -0.333333 },
	{ 0.816497, -0.471405, -0.333333 }
};


GLfloat gasket_colors[3][3] = {
	{ 1.0, 0.0, 1.0 },
	{ 0.2, 0.35, 1.0 },
	{ 0.90, 0.95, 0.0 }
};

float  CompositeTransMatrix[4][4] = {    //保存上次的model_view矩阵
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 }
};

float *p_CompositeTransMatrix = *CompositeTransMatrix;




void display();
void mouseClick(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void draw();
void menu(int index);
void Perspective(int w, int h);
void Orthographic(int w, int h);
void myReshape(int w, int h);
void mapping(int x, int y, int width, int height, GLfloat vector[3]);
void init();
void timer(int id);
void drawShpere(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth, int m);
void divide_triangle(GLfloat* first, GLfloat* second, GLfloat* third, int devide_num);
void Normalization(GLfloat* vec);
void drawTriangle(GLfloat* first, GLfloat* second, GLfloat* third);
void keyEvent(unsigned char key, int x, int y);
void specialEvent(int key, int x, int y);

int main(int argc, char** args){
	glutInit(&argc, args);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Lighting Rotate Sphere | Author 陈相男 Team");
	init();
	glutSpecialFunc(specialEvent);
	glutKeyboardFunc(keyEvent);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutCreateMenu(menu);
	glutAddMenuEntry("Polygon", 1);
	glutAddMenuEntry("Line_loop", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 8, 0, 0, 0, 0, 1, 0);
	GLfloat light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat light1_position[] = { 0.0f, 4.0f, 0.0f, 1.0f };
	GLfloat light2_position[] = { 0.0f, -4.0f, 0.0f, 1.0f };
	GLfloat light3_position[] = { 0.0f, 0.0f, 4.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //将光源看作一个模型，加入到
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	draw();
	glFlush();
}

void draw(){
	glPushMatrix();
	glLoadIdentity();
	glRotated(angle, axis[0], axis[1], axis[2]);    glMultMatrixf(p_CompositeTransMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, p_CompositeTransMatrix);
	glPopMatrix();
	glMultMatrixf(p_CompositeTransMatrix);
	glPointSize(20);
	glBegin(GL_POINTS);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();
	glTranslated(-2.0, 0.0, 0.0);
	drawShpere(gasket_vertices[0], gasket_vertices[1], gasket_vertices[2], gasket_vertices[3], divideNum);
	glTranslated(4.0, 0.0, 0.0);
	drawShpere(gasket_vertices[0], gasket_vertices[1], gasket_vertices[2], gasket_vertices[3], divideNum);
}

void mouseClick(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		angle = 0.0;
		oldx = x;
		oldy = y;
		mapping(x, y, winWidth, winHeight, lastVector);
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		glutTimerFunc(20, timer, 1);
	}
}

void mouseMotion(int x, int y){
	GLfloat curVector[3] = { 0 };
	GLfloat dx = 0;
	GLfloat dy = 0;
	GLfloat dz = 0;
	mapping(x, y, winWidth, winHeight, curVector);
	dx = lastVector[0] - curVector[0];
	dy = lastVector[1] - curVector[1];
	dz = lastVector[2] - curVector[2];
	if (dx || dy || dz){
		angle = 90.0F * sqrt((double)(pow(dx, 2) + pow(dy, 2) + pow(dz, 2)));

		/*
		*雅克比行列式求解，得到旋转轴
		* i    j   k
		* a1   a2  a3
		* b1   b2  b3
		*a2 * b3 - a3 * b2
		*-1(a1 * b3 - a3 * b1)
		*a1 * b2 - a2 * b1
		*/
		axis[0] = lastVector[1] * curVector[2] - lastVector[2] * curVector[1];
		axis[1] = lastVector[2] * curVector[0] - lastVector[0] * curVector[2];
		axis[2] = lastVector[0] * curVector[1] - lastVector[1] * curVector[0];

		lastVector[0] = curVector[0];
		lastVector[1] = curVector[1];
		lastVector[2] = curVector[2];
	}
	glutPostRedisplay();
}

void menu(int index){
	switch (index){
	case(1) : {
				  mod = 1;
				  glutPostRedisplay();
				  break;

	}
	case(2) : {

				  mod = 2;
				  glutPostRedisplay();
				  break;
	}
	default:
		break;
	}
}

void Perspective(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w < h){
		glFrustum(-8.0, 8.0, -8.0*(GLfloat)h / (GLfloat)w, 8.0*(GLfloat)h / (GLfloat)w, _near, _far);
	}
	else{
		glFrustum(-8.0*(GLfloat)h / (GLfloat)w, 8.0*(GLfloat)h / (GLfloat)w, -8.0, 8.0, _near, _far);
	}

	glutPostRedisplay();
	winWidth = w;
	winHeight = h;

}

void Orthographic(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w < h){
		glOrtho(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, _near, _far);
	}
	else{
		glOrtho(-4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, -4.0, 4.0, _near, _far);
	}
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
	winWidth = w;
	winHeight = h;
}

void myReshape(int w, int h){
	if (projectStyle == 1)
		Orthographic(w, h);
	else
		Perspective(w, h);
}

void mapping(int x, int y, int width, int height, GLfloat vector[3]){
	float distance = 0.0;
	float alpha = 0.0;
	vector[0] = (8.0F * x - width) / width;                                     //屏幕x坐标映射到世界坐标
	vector[1] = (height - 8.0F * y) / height;                                   //屏幕y坐标映射到世界坐标
	distance = sqrt((double)(vector[0] * vector[0] + vector[1] * vector[1]));   //点到xoy面圆心的距离
	/*
	*z方向分量的映射，如果到圆心距离比半径大就做一次平滑操作
	*z = r^2 / sqrt(x^2 + y^2)
	*/
	if (distance <= 2.0F){
		vector[2] = (float)sqrt((double)(4.0F - distance));
	}
	else{
		vector[2] = (float)(2.0F / sqrt((double)distance));
	}
	alpha = 1.0F / sqrt((double)(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2)));

	vector[0] *= alpha;
	vector[1] *= alpha;
	vector[2] *= alpha;
}

void init(){

	//设置材质的各项属性，吸收，漫反射，镜面反射率

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shinness = { 100.0 };

	//设置吸收光，漫反射光，反射光（高光）颜色，强度 phong lighting model

	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat light1_ambient[] = { 0.0, 0.0, 0.0, 0.25 };
	GLfloat light1_diffuse[] = { 0.2, 0.2, 0.2, 0.25 };
	GLfloat light1_specular[] = { 0.0, 0.0, 0.0, 0.0 };

	GLfloat light2_ambient[] = { 0.0, 0.0, 0.0, 0.25 };
	GLfloat light2_diffuse[] = { 0.2, 0.2, 0.2, 0.25 };
	GLfloat light2_specular[] = { 0.0, 0.0, 0.0, 0.0 };

	GLfloat light3_ambient[] = { 0.0, 0.0, 0.0, 0.25 };
	GLfloat light3_diffuse[] = { 0.3, 0.3, 0.3, 0.35 };
	GLfloat light3_specular[] = { 0.0, 0.0, 0.0, 0.0 };

	//应用材质和光照

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shinness);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);

	glLightfv(GL_LIGHT3, GL_AMBIENT, light3_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light3_specular);

	//开启光照

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);       //位于圆心的点光源
	//glEnable(GL_LIGHT1);     //位于正上方的平行光
	//glEnable(GL_LIGHT2);     //位于正下方的平行光
	glEnable(GL_LIGHT3);       //镜头朝向的平行光
	glEnable(GL_DEPTH_TEST);   //开启zbuffer

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);

}

void timer(int id){
	angle = 1;
	glutPostRedisplay();
	glutTimerFunc(20, timer, 1);
}

void drawShpere(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth, int devide_num){

	/*
	*绘制球面
	*/

	divide_triangle(first, second, third, devide_num);
	divide_triangle(first, third, fourth, devide_num);
	divide_triangle(first, fourth, second, devide_num);
	divide_triangle(fourth, third, second, devide_num);
}

void Normalization(GLfloat* vec){

	/*
	* 规范化矢量，将点定位到球面
	*/

	double distance = 0;
	int i;
	for (i = 0; i < 3; i++){
		distance += (vec[i] * vec[i]);   //计算矢量的模
	}
	distance = sqrt(distance);
	if (distance > 0){
		for (i = 0; i < 3; i++){
			vec[i] = vec[i] / distance;  //单位化
		}
	}
}

void divide_triangle(GLfloat* first, GLfloat* second, GLfloat* third, int devide_num){

	/*
	*分形分量安排球面上的点
	*/

	GLfloat iVector[3][3] = { 0 };
	int i;
	if (devide_num > 0){
		for (i = 0; i < 3; i++) iVector[0][i] = first[i] + second[i];
		Normalization(iVector[0]);
		for (i = 0; i < 3; i++) iVector[1][i] = first[i] + third[i];
		Normalization(iVector[1]);
		for (i = 0; i < 3; i++) iVector[2][i] = second[i] + third[i];
		Normalization(iVector[2]);
		divide_triangle(first, iVector[0], iVector[1], devide_num - 1);
		divide_triangle(third, iVector[1], iVector[2], devide_num - 1);
		divide_triangle(second, iVector[2], iVector[0], devide_num - 1);
		divide_triangle(iVector[0], iVector[2], iVector[1], devide_num - 1);
	}
	else{
		drawTriangle(first, second, third);
	}
}

void drawTriangle(GLfloat* first, GLfloat* second, GLfloat* third){

	/*
	*绘制三角形来围成球面
	*/

	if (mod == 1) glBegin(GL_POLYGON);
	else        glBegin(GL_LINE_LOOP);
	glNormal3fv(first);
	glVertex3fv(first);
	glNormal3fv(second);
	glVertex3fv(second);
	glNormal3fv(third);
	glVertex3fv(third);
	glEnd();
}

void keyEvent(unsigned char key, int x, int y){
	int flag = key - '0';
	if (flag < 8){
		divideNum = (key - '0') % 8;
		glutPostRedisplay();
	}
}

void specialEvent(int key, int x, int y){
	switch (key) {
	case GLUT_KEY_UP:
		if (divideNum < 7){
			divideNum++;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_DOWN:
		if (divideNum > 0){
			divideNum--;
			glutPostRedisplay();
		}
		break;

	default:
		break;
	}
}














