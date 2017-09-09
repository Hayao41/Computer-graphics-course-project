//
//  Circle.c
//  CircleDraw
//
//  Created by 陈相男 on 2016/10/27.
//  Last Modified     on 2016/10/29.
//  Copyright © 2016年 JoelChen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <GL\glut.h>
#include <math.h>

int cx = 0;         //鼠标点击的初始x分量
int cy = 0;         //鼠标点击的初始y分量
int oldx = 0;       //鼠标追踪的初始x分量
int oldy = 0;       //鼠标追踪的初始y分量

BOOLEAN latest = false;  //是否为第一个圆


void initWindow();                                                        //窗口初始化
void display();                                                           //显示回调函数
void drawCircle(int pastx, int pasty, int current_x, int current_y);      //中点画圆算法具体实现
void mouse(int button, int state, int x, int y);                          //鼠标点击事件
void mouseoffset(int x, int y);                                           //鼠标跟踪事件
void changeWindowsSize(int w, int h);                                     //窗口剪裁

int main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Mid Circle | Author 陈相男");
	initWindow();
	glutReshapeFunc(changeWindowsSize);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseoffset);
	glutMainLoop();
}

void initWindow(){

	/* 窗口初始化 */
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_COLOR_LOGIC_OP);    //打开逻辑操作

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 500.0, 0.0, 500.0);

	glMatrixMode(GL_MODELVIEW);
}

void display(){
	glFlush();
}

void drawCircle(int pastx, int pasty, int current_x, int current_y){

	/* 
	 *
     *@method
	 *中点画圆算法 
	 *@参数 
	 *pastx          圆心x分量
	 *pasty          圆心y分量
	 *current_x      鼠标追踪的x分量
	 *current_y      鼠标追踪的y分量
	 *
    */

	int radius = 0;
	int x = 0;       //相对绝对圆心的x分量
	int y = 0;       //相对绝对圆心的y分量
	int p = 0;       //判别值
	radius = (int)sqrt((pow((double)(current_x - pastx), 2) + pow((double)(current_y - pasty), 2)));  //通过圆心和移动点求出半径
	y = radius;
	p = 1 - radius;  //初始化p0
	glPointSize(1.0);

	glBegin(GL_POINTS);
	while (x < y){

		/* 利用对称性的1/8画圆 */

		glVertex2f(pastx + x, pasty + y);
		glVertex2f(pastx + x, pasty - y);
		glVertex2f(pastx - x, pasty - y);
		glVertex2f(pastx - x, pasty + y);
		glVertex2f(pastx + y, pasty + x);
		glVertex2f(pastx + y, pasty - x);
		glVertex2f(pastx - y, pasty + x);
		glVertex2f(pastx - y, pasty - x);

		/* 判别式 */
		if (p < 0){
			/* 中点在圆内 */
			p += 2 * x + 1;
		}
		else{
			/* 中点不在圆内 */
			p += 2 * (x - y) + 1;
			y --;
		}
		x ++;
	}
	glEnd();
}

void mouse(int button, int state, int x, int y){

	/* 鼠标点击事件确定圆心 */

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		printf("圆心：%d %d \n",x,y);
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);     //获取视口参数
		oldx = x;
		oldy = viewport[3] - y;                   //屏幕坐标y分量映射到世界坐标 ： 视口y分量 - 屏幕坐标
		glColor3f(1.0, 1.0, 1.0);
		latest = false;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		                   
		if (latest){
			/* 如果通过拖动设定了半径就画圆防止点击后就画圆 */
			glLogicOp(GL_XOR);                    //逻辑异或清除之前motion画的圆
			drawCircle(oldx, oldy, cx, cy);
			glFlush();
		}
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		cx = (int)x;
		cy = (int)(viewport[3] - y);
		glColor3f(1.0, 1.0, 1.0);
		drawCircle(oldx, oldy, cx, cy);
		glFlush();
		glLogicOp(GL_COPY);                       //将最后一个圆拷贝到缓存中
	}
}

void mouseoffset(int x, int y){

	/* 鼠标跟踪事件 */

	if (latest){
		glLogicOp(GL_XOR);                        //逻辑异或清除之前motion画的圆不然会有很多圆
		drawCircle(oldx, oldy, cx, cy);
		glFlush();
	}
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	cx = (int)x;
	cy = (int)(viewport[3] - y);
	glColor3f(1.0, 1.0, 1.0);
	drawCircle(oldx, oldy, cx, cy);
	glFlush();
	latest = true;
}

void changeWindowsSize(int w, int h){

	/* 窗口剪裁 */

	GLfloat hrate = (GLfloat)h / (GLfloat)w;
	GLfloat wrate = (GLfloat)w / (GLfloat)h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h){
		gluOrtho2D(0.0, 500.0, 0.0, 500.0*hrate);
	}
	else{
		gluOrtho2D(0.0, 500.0*wrate, 0.0, 500.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}


