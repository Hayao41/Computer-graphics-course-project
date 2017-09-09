/*
* Sierpinski  Demo
* @Author         陈相男
* date            10/15/2016
* last modified   10/16/2016
*/

#include<stdio.h>
#ifdef __APPLE
#include <GLUT/glut.h>
#else
#include <Windows.h>
#include <GL/glut.h>
#endif // __APPLE

#define SIZE 512 //Picking名字栈的大小

GLfloat xoffset = 1; //顶点的x方向偏移量
GLfloat yoffset = 1; //顶点的y方向偏移量

bool selected = false; //是否被选中

int cx = 0;  //鼠标点击的初始坐标
int cy = 0;

int flag = 1; //绘制的类型 1.面，2.点，3.线

int divide_num = 0; //细分的次数

GLfloat initialfixed[3][3] = {
	{-1.0,-1.0},
	{1.0,-1.0},
	{0.0,1.0}
};           //初始三角形的顶点

/* 函数原型 */

void initWindow();
void dispaly();
void draw(GLenum mode);
void divide_triangle(GLfloat* a, GLfloat* b, GLfloat* c, int divied_num,int GL_TYPE);
void triangle(GLfloat* a, GLfloat* b, GLfloat* c);
void triangle_line(GLfloat* a, GLfloat* b, GLfloat* c);
void Keyboardswitch(int key,int x,int y);
void getKeyboardInput(unsigned char key, int x, int y);
void changeWindowsSize(int w, int h);
void typemenu(int value);
void Mouse(int button, int state, int x, int y);
void onMouseMove(int x, int y);
void processHits(GLint hits, GLuint buffer[],int recorder[]);


int main(int argc, char** argv){
	int menu_id;
	printf("You can input the divide number of this triangle or use the key UP/DOWN to control it\n But the max number is 8 :) \n 画框大小设置得有点大，因为点图元太小了不好picking\n");
	glutInit(&argc, argv); //窗口初始化
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);               //初始化显示模式 这里是单帧，颜色采用RGB模式
	glutInitWindowSize(500, 500);                              //初始化窗口大小
	glutCreateWindow("@Author 陈相男 | Sierpinski Demo");      //创建窗口并赋值名字
	initWindow();
	menu_id = glutCreateMenu(typemenu);                        //创建一个菜单并且将菜单选项值赋值给menu_id作为index
	glutAddMenuEntry("Triangles", 1);                          //向菜单中插入选项并编号
	glutAddMenuEntry("Points", 2);                             //向菜单中插入选项并编号
	glutAddMenuEntry("Lines", 3);                              //向菜单中插入选项并编号
	glutAttachMenu(GLUT_RIGHT_BUTTON);                         //将菜单唤醒事件绑定在鼠标右键
	glutReshapeFunc(changeWindowsSize);                        //改变视口从而重绘图形防止图形变形
	glutDisplayFunc(dispaly);                                  //显示回调函数注册一个绘图函数这里我使用的是display
	glutKeyboardFunc(getKeyboardInput);                        //键盘事件回调函数注册一个键盘事件监听函数
	glutSpecialFunc(Keyboardswitch);                           //设置当前窗口的特定键的回调函数如F1,F2 up，down等键
	glutMotionFunc(onMouseMove);                               //鼠标移动回调函数注册一个处理动态鼠标坐标的事件函数
	glutMouseFunc(Mouse);                                      //鼠标点击事件监听回调函数注册一个处理鼠标事件的函数
	glutMainLoop();                                            //事件处理循环无限调用main中的回调函数
}

void initWindow(){
	glMatrixMode(GL_PROJECTION);         //指定当前操作的矩阵，GL_PROJECTION为投影矩阵
	glLoadIdentity();                    //将当前坐标系的原点移动到屏幕中心
	gluOrtho2D(-2.0, 2.0, -2.0, 2.0);    //定义一个二维的投影坐标，参数为四个角的值
	glMatrixMode(GL_MODELVIEW);          //指定当前操作的矩阵，GL_PROJECTION为模型视角矩阵
	glClearColor(1.0, 1.0, 1.0, 1.0);    //清屏然后填充指定的颜色
	glColor3f(0.0, 0.0, 0.0);            //渲染图形的颜色
}

void dispaly(){
	
	glClear(GL_COLOR_BUFFER_BIT);        //清除缓冲 这里是颜色缓冲
	glColor3f(0.5, 0.28, 0.7);
	draw(GL_RENDER);                     //绘图 模式是渲染
	glFlush();                           //强制刷新缓冲防止排队
}

void divide_triangle(GLfloat* a, GLfloat* b, GLfloat* c, int num,int GL_TYPE){

	/* 分形递归算法 */

	GLfloat vertex_0[2],  //每一次递归细分记录的三角形定点
		    vertex_1[2],
		    vertex_2[2];
	int i;
	if (num > 0){
		/* 计算中点坐标 */
		for (i = 0; i < 2; i++) vertex_0[i] = (a[i] + b[i]) / 2;
		for (i = 0; i < 2; i++) vertex_1[i] = (a[i] + c[i]) / 2;
		for (i = 0; i < 2; i++) vertex_2[i] = (b[i] + c[i]) / 2;
		divide_triangle(a, vertex_0, vertex_1, num - 1, GL_TYPE);
		divide_triangle(c, vertex_1, vertex_2, num - 1, GL_TYPE);
		divide_triangle(b, vertex_2, vertex_0, num - 1, GL_TYPE);
	}
	else{
	    if (GL_TYPE != (int)GL_LINES){
			triangle(a, b, c);
		}
		triangle_line(a, b, c);
	}
		
}
void triangle(GLfloat* a, GLfloat* b, GLfloat* c){

	/* 渲染三角形 */

	glVertex2fv(a);   //设置将要渲染的点
	glVertex2fv(b);
	glVertex2fv(c);
}
void triangle_line(GLfloat* a, GLfloat* b, GLfloat* c){

	/* 绘制线模式的三角形 */

	glVertex2fv(a);
	glVertex2fv(b);
	glVertex2fv(b);
	glVertex2fv(c);
	glVertex2fv(c);
	glVertex2fv(a);
}

void Keyboardswitch(int key,int x,int y){
	/* 从键盘获取细分增减信息 */
	switch (key){
	    case GLUT_KEY_UP :{   // up键
				 if (divide_num < 8)
			     divide_num++;
			     glutPostRedisplay();  // 标记当前窗口重新绘制
	   		     break;
		}
		case GLUT_KEY_DOWN:{  // down 键
			     if (divide_num > 0)
				 divide_num--;
				 glutPostRedisplay();   // 标记当前窗口重新绘制
			     break;
		}
		default:
		break;
	}
}

void changeWindowsSize(int w, int h){

	/* 剪裁视口防止图形变形 */

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);    //视口函数可以设置视口矩阵的大小从而改变视口在窗口中的大小和位置

	glMatrixMode(GL_PROJECTION);                 //指定当前操作的矩阵，GL_PROJECTION为投影矩阵
	glLoadIdentity();

	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h / (GLfloat)w, 1.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);  //设置或修改修剪空间的范围，分别为x方向，y方向，z方向

	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void getKeyboardInput(unsigned char key,int x, int y){

	/* 键盘事件 */

	if (key == 'q'){
		exit(0);
	}
	else{
		divide_num = (key - 48) % 10;
	}
	glutPostRedisplay();
}

void typemenu(int value){

	/* 菜单事件 */

	switch (value){
	case 1:
		flag = 1;
		break;
	case 2:
		flag = 2;
		break;
	case 3:
		flag = 3;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y){

	/* 鼠标事件picking */

	GLuint selectbuf[SIZE];                                      //定义一个名字栈
	GLint hits;                                                  //选择类型
	GLint viewport[4];                                           //视口参数数组
	int recorder[2];                                             //回调函数返回的参数接收数组

	if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN){
		
		recorder[0] = x;
		recorder[1] = y;
		
		glGetIntegerv(GL_VIEWPORT, viewport);                    //将目标的值赋值给接受数组    这里是VIEWPORT的值           

		glSelectBuffer(SIZE, selectbuf);                         //创建一个名字栈用于检索被选中对象
		glRenderMode(GL_SELECT);                                 //修改渲染模式这里是选择模式

		glInitNames();                                           //初始化名字栈
		glPushName(0);                                           //向名字栈压0

		glMatrixMode(GL_PROJECTION);                         
		glPushMatrix();                                          //将当前的投影矩阵压入栈用于保存防止被画框修改
		glLoadIdentity();                              
		
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y),100.0, 100.0, viewport);   //创建一个画框用于选择对象，这里画框大小为100 x 100
		gluOrtho2D(-2.0, 2.0, -2.0, 2.0);                                                
		draw(GL_SELECT);


		glMatrixMode(GL_PROJECTION);
		glPopMatrix();                 //弹出被保存的矩阵用于恢复
		glFlush();                 

		hits = glRenderMode(GL_RENDER);            //mode可以选取以下三种模式之一：绘制模式(GL_RENDER),选择模式(GL_SELECT),反馈模式(GL_FEEDBACK)。函数的返回值可以确定选择模式下的命中次数或反馈模式下的图元数量。
		processHits(hits, selectbuf,recorder);     //处理命中，响应事件

		glutPostRedisplay();
	}
}

void onMouseMove(int x, int y){

	/* 通过鼠标的偏移量来重新确定三角形的顶点 */

	if (selected){
		xoffset = ((x - cx) * 0.01);
		yoffset = -((y - cy) * 0.01);
	}
	else{
		xoffset = 0;
		yoffset = 0;
	}
	initialfixed[0][0] += xoffset;
	initialfixed[0][1] += yoffset;
	initialfixed[1][0] += xoffset;
	initialfixed[1][1] += yoffset;
	initialfixed[2][0] += xoffset;
	initialfixed[2][1] += yoffset;
	glutPostRedisplay();
	/*GLfloat transMatrix[3][3] = {
		{1,0,0},
		{0,1,0},
		{xoffset,yoffset,1}
	};*/
	cx = x;
	cy = y;
}

void processHits(GLint hits, GLuint buffer[],int recorder[]){

	/* 处理命中 */

	unsigned int j;
	GLuint names, *iterator;

	iterator = (GLuint*)buffer;

	if (hits != 0){
		names = *iterator;
		iterator += 3;
		for (j = 0; j < names; j++){
			if (*iterator == 1){
				printf("the triangle is selected !\n");
				cx = recorder[0];
				cy = recorder[1];
				selected = true;
			}
			iterator++;
		}
	}
	else
		selected = false;
}

void draw(GLenum mode){

	/* 绘画函数绘制具体的分形 */

	if (mode == GL_SELECT) glLoadName(1); //如果是选择模式就替换栈顶的名字
	glColor3f(0.5, 0.28, 0.7);
	switch (flag){
	case 1:
		glBegin(GL_TRIANGLES);           //开始绘图，采用绘制三角形的模式，本质其实就是将begin和end之间的点按照给定的模式进行绘制    
		divide_triangle(initialfixed[0], initialfixed[1], initialfixed[2], divide_num, 0);
		glEnd();                         //完成绘制停止绘制过程
		break;
	case 2:
		glPointSize(9.0);
		glBegin(GL_POINTS);
		divide_triangle(initialfixed[0], initialfixed[1], initialfixed[2], divide_num, 0);
		glEnd();
		break;
	case 3:
		glBegin(GL_LINES);
		divide_triangle(initialfixed[0], initialfixed[1], initialfixed[2], divide_num, GL_LINES);
		glEnd();
		break;
	default:
		break;
	}
}