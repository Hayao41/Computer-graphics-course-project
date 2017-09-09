//
//  Camaro Control.cpp
//  Camaro Control
//
//  Created by 陈相男 on 2016/11/18.
//  Last Modified     on 2016/11/19.
//  Copyright © 2016年 JoelChen. All rights reserved.
//

#include <stdio.h>
#include <Windows.h>
#include <GL\glut.h>
#include <math.h>

#ifndef PI
#define PI  3.14159
#endif

int _near = 1.5;          //取景框的near
int _far = 10;            //取景框的far

int projectStyle = 1;     //投影类型

int winWidth = 800; 
int winHeight = 800;      //窗口的长宽

bool isMouseUp = false;   //鼠标左键是否弹起

int divided_num = 0;      //分型次数

GLfloat angle = 0.0;                             //旋转角度
GLfloat lastVector[3] = { 0.0, 0.0, 0.0 };       //最后点的球面上坐标
GLfloat axis[3] = { 0.0, 0.0, 0.0 };             //旋转轴

GLfloat lookAtVector[3] = { 0.0, 0.0, 0.0 };     //摄像机的观察点

GLfloat camaroPosition[4][4] = {                 //摄像机的位置
	{1.0,0.0,0.0,0.0},
	{0.0,1.0,0.0,0.0},
	{0.0,0.0,1.0,0.0},
	{0.0,0.0,4.0,1.0}
};
GLfloat lookUp[4][4] = {                         //摄像机的up向量
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 1.0, 0.0, 1.0 }
};

float *p_camaro = *camaroPosition;
float *p_up = *lookUp;

GLfloat gasket_vertices[4][3] = {               //顶点
	{ -1.732, -1.0, -1.0 },
	{ 0.0, -1.0, 2.0 },
	{ 1.732, -1.0, -1.0 },
	{ 0.0, 2.0, 0.0 }
};


GLfloat gasket_colors[4][3] = {                 //颜色
	{ 1.0, 0.0, 1.0 },
	{ 0.2, 0.35, 1.0 },
	{ 0.90, 0.95, 0.0 },
	{ 0.90, 0.35, 0.5 }
};

GLfloat  CompositeTransMatrix[4][4] = {          //用于记录上一次CTM的矩阵，防止重画时还原
	{ 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 }
};

float *p_CompositeTransMatrix = *CompositeTransMatrix;


void display();
void divideGasket(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth, int dvided_num);
void drawTriangles(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth);
void setTriangleponits(GLfloat* first, GLfloat* second, GLfloat* third);
void mouseClick(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void draw();
void menu(int index);
void Perspective(int w, int h);
void Orthographic(int w, int h);
void myReshape(int w, int h);
void mapping(int x, int y, int width, int height, GLfloat vector[3]);
void rotateTimer(int id);
void processSpecial(int key, int x, int y);
void getKeyboardInput(unsigned char key, int x, int y);
void camaroMotion();

int main(int argc, char** args){
	printf("You can input the divide number of this triangle or use the key UP/DOWN to control it\n But the max number is 8 :) \n");
	glutInit(&argc, args);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);   //启用zbuffer
	glutInitWindowSize(800, 800);
	glutCreateWindow("3D TrackGasket Gasket | Author 陈相男");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);   //启用深度测试
	glShadeModel(GL_FLAT);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutTimerFunc(20, rotateTimer, 1);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutSpecialFunc(processSpecial);
	glutKeyboardFunc(getKeyboardInput);
	glutCreateMenu(menu);
	glutAddMenuEntry("Orthographic", 1);
	glutAddMenuEntry("Perspective", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   //清空颜色缓存和zbuffer缓存
	draw();
	glFlush();
}

void draw(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camaroPosition[3][0], camaroPosition[3][1], camaroPosition[3][2], lookAtVector[0], lookAtVector[1], lookAtVector[2], lookUp[3][0], lookUp[3][1], lookUp[3][2]);
	glBegin(GL_TRIANGLES);
	divideGasket(gasket_vertices[0], gasket_vertices[1], gasket_vertices[2], gasket_vertices[3], divided_num);
	glEnd();
}

void divideGasket(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth, int dvided_num){

	/*
	*三维gasket建模
	*/

	GLfloat midpoints[6][3] = { 0 };
	int i = 0;
	if (dvided_num > 0){
		for (i = 0; i < 3; i++){
			midpoints[0][i] = (first[i] + second[i]) / 2;
		}
		for (i = 0; i < 3; i++){
			midpoints[1][i] = (first[i] + fourth[i]) / 2;
		}
		for (i = 0; i < 3; i++){
			midpoints[2][i] = (second[i] + third[i]) / 2;
		}
		for (i = 0; i < 3; i++){
			midpoints[3][i] = (second[i] + fourth[i]) / 2;
		}
		for (i = 0; i < 3; i++){
			midpoints[4][i] = (third[i] + fourth[i]) / 2;
		}
		for (i = 0; i < 3; i++){
			midpoints[5][i] = (third[i] + first[i]) / 2;
		}
		divideGasket(first, midpoints[0], midpoints[5], midpoints[1], dvided_num - 1);
		divideGasket(midpoints[0], second, midpoints[2], midpoints[3], dvided_num - 1);
		divideGasket(midpoints[5], midpoints[2], third, midpoints[4], dvided_num - 1);
		divideGasket(midpoints[1], midpoints[3], midpoints[4], fourth, dvided_num - 1);

	}
	else{
		drawTriangles(first, second, third, fourth);

	}
}

void drawTriangles(GLfloat* first, GLfloat* second, GLfloat* third, GLfloat* fourth){

	/*
	*绘制每一个分形的表面
	*/

	glColor3fv(gasket_colors[0]);
	setTriangleponits(first, second, third);
	glColor3fv(gasket_colors[1]);
	setTriangleponits(second, third, fourth);
	glColor3fv(gasket_colors[3]);
	setTriangleponits(first, second, fourth);
	glColor3fv(gasket_colors[2]);
	setTriangleponits(third, first, fourth);
}

void setTriangleponits(GLfloat* first, GLfloat* second, GLfloat* third){
	/*
	*设置三角形顶点
	*/
	glVertex3fv(first);
	glVertex3fv(second);
	glVertex3fv(third);
}

void mouseClick(int button, int state, int x, int y){

	/*
	*鼠标点击事件，然后将点击的屏幕坐标映射到球面
	*/

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		mapping(x, y, winWidth, winHeight, lastVector);
	}
	else if (state == GLUT_UP){
		isMouseUp = true;
	}
}

void mouseMotion(int x, int y){

	/*
	*追踪鼠标的最后位置，然后计算出旋转角度和旋转轴
	*/

	GLfloat curVector[3] = { 0 };
	GLfloat dx = 0;
	GLfloat dy = 0;
	GLfloat dz = 0;
	mapping(x, y, winWidth, winHeight, curVector);
	dx = lastVector[0] - curVector[0];
	dy = lastVector[1] - curVector[1];
	dz = lastVector[2] - curVector[2];
	if (dx || dy || dz){
		angle = 90.0F * sqrt((double)(pow(dx, 2) + pow(dy, 2) + pow(dz, 2))); //微分近似，(sin theta = theta，theta =  arc length ， arc length = distance) 然后求出角度

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
	camaroMotion();
	printf("camaro position : %.2f %.2f %.2f\n", camaroPosition[3][0], camaroPosition[3][1], camaroPosition[3][2]);
	printf("camaro up : %.2f %.2f %.2f\n", lookUp[3][0], lookUp[3][1], lookUp[3][2]);
}

void menu(int index){

	/* 菜单事件 */

	switch (index){
	case(1) : {
				  Orthographic(winWidth, winHeight);
				  projectStyle = 1;
				  glutPostRedisplay();
				  break;

	}
	case(2) : {

				  Perspective(winWidth, winHeight);
				  projectStyle = 2;
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
		glFrustum(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, _near, _far);
	}
	else{
		glFrustum(-4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, -4.0, 4.0, _near, _far);
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

	/*
	*坐标映射，将屏幕左边映射到球面
	*/

	float distance = 0.0;
	float alpha = 0.0;
	vector[0] = (4.0F * x - width) / width;                                     //屏幕x坐标映射到世界坐标
	vector[1] = (height - 4.0F * y) / height;                                   //屏幕y坐标映射到世界坐标
	distance = sqrt((double)(vector[0] * vector[0] + vector[1] * vector[1]));   //点到xoy面圆心的距离
	/*
	*z方向分量的映射，如果到圆心距离比半径大就做一次平滑操作，在超过的地方外接另一个双曲面
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

void rotateTimer(int id){

	/* 动画timer */

	if (isMouseUp){
		angle = 1;
		camaroMotion();
	}
	glutTimerFunc(20, rotateTimer, 1);
}

void processSpecial(int key, int x, int y){

	/* 键盘事件 */

	switch (key){
	case GLUT_KEY_UP:{
						 if (divided_num < 9){
							 divided_num++;
							 glutPostRedisplay();
						 }
						 break;
	}
	case GLUT_KEY_DOWN:{
						   if (divided_num > 0){
							   divided_num--;
							   glutPostRedisplay();
						   }
						   break;
	}
	default:
		break;
	}
}


void getKeyboardInput(unsigned char key, int x, int y){

		/* 键盘事件 */

		if (key == 'q'){
			exit(0);
		}
		else{
			divided_num = (key - 48) % 9;
		}
		glutPostRedisplay();
}

void camaroMotion(){

	/*
	*通过旋转改变摄像机的镜头视角
	*/

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();                                  //保存当前的modelview防止后面的错误修改
	glLoadIdentity();                                //加载一个单位矩阵
	/*
	*通过矩阵乘法来改变摄像机的position和lookup
	* LPi+1 = R * LPi
	* LUi+1 = R * LUi
	*/
	glRotated(angle, axis[0], axis[1], axis[2]);     
	glMultMatrixf(p_camaro);
	glGetFloatv(GL_MODELVIEW_MATRIX, p_camaro);      //保存新的摄像机的位置
	glLoadIdentity();
	glRotated(angle, axis[0], axis[1], axis[2]);
	glMultMatrixf(p_up);
	glGetFloatv(GL_MODELVIEW_MATRIX, p_up);          //保存新的摄像机的lookup
	glLoadIdentity();                                  
	glPopMatrix();                                   //还原modelview
	glutPostRedisplay();
}












