//
//  Computer Graphics Project.cpp
//  Computer Graphics Project
//
//  Created by 陈相男 on 2016/11/12.
//  Last Modified     on 2016/11/12.
//  Copyright © 2016年 JoelChen. All rights reserved.
//

#include <stdio.h>
#include <Windows.h>
#include <GL\glut.h>
#include <math.h>
#include <GL\GLAUX.H>
#ifndef PI
#define PI  3.14159
#endif


int oldx = 0;      
int oldy = 0;

int cx = 0;
int cy = 0;

int _near = 4;     //镜头前距离
int _far = 20;     //镜头后距离

int projectStyle = 1;    //投影类型


int winWidth = 800;
int winHeight = 800;      //窗口的长宽

int mod = 1;             //渲染模式

GLuint texture[1];     //纹理缓存

GLfloat angle = 0.0;   //旋转角度
GLfloat Alpha = 1.0;   //alpha值
GLfloat Diffuse = 1.0; //需要透明模型的漫反射
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

GLfloat cubeVertices[8][3] = {
	{ -1.0, -1.0, -1.0 }, { 1.0, -1.0, -1.0 }, { 1.0, 1.0, -1.0 }, { -1.0, 1.0, -1.0 },
	{ -1.0, -1.0, 1.0 }, { 1.0, -1.0, 1.0 }, { 1.0, 1.0, 1.0 }, { -1.0, 1.0, 1.0 }
};

GLfloat colors[8][3] = {
	{ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 1.0 }, { 1.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0 }, { 0.0, 1.0, 1.0 }
};

GLfloat white_color[3] = { 1.0, 1.0, 1.0};
GLfloat red_color[3] = {1.0, 0.0, 0.0};
GLfloat green_color[3] = { 0.0, 1.0, 0.0};
GLfloat fog_color[4] = { 0.5, 0.5, 0.5, 0.8 };


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
void drawCube();
void drawPolygon(int a , int b , int c , int d , int face);
void setMaterial(const GLfloat* matdiffuse, GLfloat shinness, GLfloat alpha);
int loadTexture();
AUX_RGBImageRec *LoadBMP(char* Filename);


int main(int argc, char** args){
	glutInit(&argc, args);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Computer Graphics Project | Author 陈相男 Team");
	init();
	glutTimerFunc(30, timer, 1);
	glutSpecialFunc(specialEvent);
	glutKeyboardFunc(keyEvent);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutCreateMenu(menu);
	glutAddMenuEntry("Orthographic", 1);
	glutAddMenuEntry("Perspective", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
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
	GLfloat light3_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
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


	//启用混合
	glEnable(GL_BLEND);
	/*
	*混合公式：rgba = 源rgba * alpha + 源rgba * （1 - alpha）
	*/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//开启雾化效果
	glFogi(GL_FOG_MODE, GL_LINEAR);        //线性过滤
	glFogfv(GL_FOG_COLOR, fog_color);      //设置雾化颜色
	glFogf(GL_FOG_DENSITY, 0.20f);         //设置雾的浓度
	glHint(GL_FOG_HINT, GL_NICEST);        //设置雾化效果（最佳）
	glFogf(GL_FOG_START, 4.0f);            //设置雾离镜头的开始距离
	glFogf(GL_FOG_END, 10.0f);             //设置雾离镜头的结束距离
	glEnable(GL_FOG);                      //启用雾化效果

	//平滑顶点，抗锯齿
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	//开启光照
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);       //位于圆心的点光源
	//glEnable(GL_LIGHT1);     //位于正上方的平行光
	//glEnable(GL_LIGHT2);     //位于正下方的平行光
	glEnable(GL_LIGHT3);       //镜头朝向的点光源

	glShadeModel(GL_SMOOTH);   //平滑着色
	glEnable(GL_DEPTH_TEST);   //开启zbuffer
	glEnable(GL_TEXTURE_2D);   //启用纹理
	loadTexture();             //从图片加载纹理
	

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glColor3f(0.0, 0.0, 0.0);

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
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);  //将光源看作一个模型，加入到模型空间
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	draw();
	glFlush();
}

void draw(){
	glPushMatrix();                                           // 将观察矩阵压栈，防止后面的操作改变观察矩阵
	glLoadIdentity();                                         // I 加载一个单位矩阵，替换当前矩阵

	glRotated(angle, axis[0], axis[1], axis[2]);              // * R

	glMultMatrixf(p_CompositeTransMatrix);                    // * Mi

	glGetFloatv(GL_MODELVIEW_MATRIX, p_CompositeTransMatrix); //将新的CTM保存在p_CompositeTransMatrix中
	glPopMatrix();                                            //弹出观察矩阵
	glMultMatrixf(p_CompositeTransMatrix);                    //单位矩阵右乘p_CompositeTransMatrix

	//绘制中心参考点
	glPointSize(20);
	glPushMatrix();
	setMaterial(red_color, 60 , 1.0);
	glBegin(GL_POINTS);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	//绘制球体
	glPushMatrix();
	setMaterial(green_color, 100 , 1.0);
	glTranslated(-2.0, 0.0, 0.0);
	drawShpere(gasket_vertices[0], gasket_vertices[1], gasket_vertices[2], gasket_vertices[3], divideNum);
	glPopMatrix();

	//绘制可透明立方体
	glPushMatrix();
	glDepthMask(GL_FALSE);
	if (Alpha > 0.8){
		/* 如果透明度大于0.8就开启背面消隐 */
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}
	else{
		glDisable(GL_CULL_FACE);
	}
	setMaterial(white_color, 100, Alpha);
	glTranslated(2.0, 0.0, 0.0);
	drawCube();
	glDepthMask(GL_TRUE);
	glPopMatrix();
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


void mapping(int x, int y, int width, int height, GLfloat vector[3]){

	/**
	*屏幕坐标到球面坐标的映射
	*/

	float distance = 0.0;
	float alpha = 0.0;
	vector[0] = (4.0F * x - width) / width;                                     //屏幕x坐标映射到世界坐标
	vector[1] = (height - 4.0F * y) / height;                                   //屏幕y坐标映射到世界坐标s
	distance =(double)(vector[0] * vector[0] + vector[1] * vector[1]);          // x^2 + y^2
	/*
	*z方向分量的映射，如果到圆心距离比半径大就做一次平滑操作
	*z = sqrt(r^2 - x^2 - y^2)
	*z = (r^2/2) / sqrt(x^2 + y^2)
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



void drawCube(){

	/**
	*绘制立方体 
	*/

	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	drawPolygon(1, 0, 3, 2, 0);
	drawPolygon(3, 7, 6, 2, 1);
	drawPolygon(7, 3, 0, 4, 2);
	drawPolygon(2, 6, 5, 1, 3);
	drawPolygon(4, 5, 6, 7, 4);
	drawPolygon(5, 4, 0, 1, 5);
	//glDisable(GL_CULL_FACE);
}
void drawPolygon(int a, int b, int c, int d, int face){

	/**
	*绘制立方体每一个表面并映射到纹理空间
	*/

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3fv(cubeVertices[a]);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3fv(cubeVertices[b]);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3fv(cubeVertices[c]);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3fv(cubeVertices[d]);
	glEnd();
}

void setMaterial(const GLfloat* matdiffuse, GLfloat shinness,GLfloat alpha){

	/**
	*设置模型的材质
	*/

	GLfloat diffuse[4] = { 0 };
	GLfloat mat_diffuse[] = { Diffuse, Diffuse, Diffuse, alpha};
	for (int i = 0; i < 3; i++){
		diffuse[i] = matdiffuse[i];
	}
	diffuse[3] = alpha;
	glMaterialfv(GL_FRONT, GL_AMBIENT, diffuse);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	if (alpha > 0.8){
		GLfloat mat_diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}
	glMaterialf(GL_FRONT, GL_SHININESS, shinness);
}

int loadTexture(){

	/**
	*从位图文件加载纹理
	*/

	int status = false;
	AUX_RGBImageRec* TextureImage[1];
	memset(TextureImage, 0, sizeof(void *)* 1);
	if (TextureImage[0] = LoadBMP("texture.bmp")){
		status = true;
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D,
			0, 3,
			TextureImage[0]->sizeX,
			TextureImage[0]->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE,
			TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	if (TextureImage[0]){
		if (TextureImage[0]->data){
			free(TextureImage[0]->data);
		}
		free(TextureImage[0]);
	}
	return status;
}

AUX_RGBImageRec *LoadBMP(char* Filename){

	/**
	*打开文件并包装为纹理数组
	*/

	FILE* texturePic = NULL;
	if (!Filename){
		return NULL;
	}
	texturePic = fopen(Filename, "r");
	if (texturePic){
		fclose(texturePic);
		return auxDIBImageLoadA(Filename);
	}
	return NULL;
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
		if (Alpha < 1){
			Alpha += 0.1;
			Diffuse += 0.1;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_DOWN:
		if (Alpha > 0){
			Alpha -= 0.1;
			Diffuse -= 0.1;
			glutPostRedisplay();
		}
		break;

	default:
		break;
	}
}

void timer(int id){
	angle = 0.5;
	glutPostRedisplay();
	glutTimerFunc(30, timer, 1);
}


void Perspective(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w < h){
		glFrustum(-4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, -4.0, 4.0, _near, _far);
	}
	else{
		glFrustum(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, _near, _far);
	}
	glutPostRedisplay();
	winWidth = w;
	winHeight = h;

}

void Orthographic(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h){
		glOrtho(-4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, -4.0, 4.0, _near, _far);
	}
	else{
		glOrtho(-4.0, 4.0, -4.0*(GLfloat)h / (GLfloat)w, 4.0*(GLfloat)h / (GLfloat)w, _near, _far);
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

void menu(int index){
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












