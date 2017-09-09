//
//  Planet.cpp
//  Planet
//
//  Created by 陈相男 on 2016/11/12.
//  Last Modified     on 2016/11/12.
//  Copyright © 2016年 JoelChen. All rights reserved.
//

#define GLUT_DISABLE_ATEXIT_HACK 
#include <stdio.h>
#include <Windows.h>
#include <GL\glut.h>

static int year = 0;
static int day = 0;

void initWindow();
void changeWindowsize(int w, int h);
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void revolution();
void rotation();
void planetIdle();
void planetTimer(int id);

int main(int argc, char** argv){
	printf("Planet trace.按Y键(鼠标左键)公转，按D键(鼠标右键)自转\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Planet | Author 陈相男");

	initWindow();

	//glutIdleFunc(planetIdle);
	glutTimerFunc(30, planetTimer, 1);    //定时器，在规定的时间内调用timer执行来刷新位置
	glutDisplayFunc(display);
	glutReshapeFunc(changeWindowsize);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}

void initWindow(){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
}

void changeWindowsize(int w, int h){

	int width = w;
	int height = h;
	GLfloat hrate = (GLfloat)h / (GLfloat)w;
	GLfloat wrate = (GLfloat)w / (GLfloat)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (GLfloat)width / (GLfloat)height, 1.0, 20.0); //设置取景框的大小，使用透视投影
	glViewport(0, 0, width, height);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	*gluLookAt
	*第一组：eyex eyey eyez相机的位置
	*第二组：centerx centery centerz,观察的物体位置
	*第三组：Look up vector LUV向量始终指向参考坐标系的up方向
	*/
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//绘制太阳
	glColor3f(1.0, 0.0, 1.0);
	glutSolidSphere(0.5, 20, 16);                   //半径为0.5的太阳

	//绘制地球
	glColor3f(0.17, 0.56, 1.0);
	glRotated((GLfloat)year, 0.0, 1.0, 0.0);        //相对于太阳公转
	glTranslated(2.0, 0.0, 0.0);                    //距离太阳2个单位长度
	glRotated((GLfloat)day, 0.0, 1.0, 0.0);         //自转
	glutSolidSphere(0.2, 20, 16);                   //半径为0.2的地球

	//绘制月球
	glColor3f(0.68, 0.69, 1.0);
	glRotated((GLfloat)year * 3, 0.0, 1.0, 0.0);    //相对于地球公转
	glTranslated(0.5, 0.0, 0.0);
	glRotated((GLfloat)day * 2, 0.0, 1.0, 0.0);
	glutSolidSphere(0.1, 20, 16);                   //半径为0.1的月亮

	glutSwapBuffers();

}

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'd':
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;
	case 'D':
		day = (day - 10) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'Y':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y){
	switch (button){
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(revolution);
		else
			glutIdleFunc(NULL);
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(rotation);
		else
			glutIdleFunc(NULL);
		break;

	default:
		break;
	}
}

void revolution(){
	year = (year + 1) % 360;
	glutPostRedisplay();
}
void rotation(){
	day = (day + 10) % 360;
	glutPostRedisplay();
}

void planetIdle(){
	year = (year + 1) % 360;
	day = (day + 10) % 360;
	glutPostRedisplay();
}

void planetTimer(int id){
	year = (year + 1) % 360;
	day = (day + 10) % 360;
	glutPostRedisplay();
	glutTimerFunc(20, planetTimer, 1);
}