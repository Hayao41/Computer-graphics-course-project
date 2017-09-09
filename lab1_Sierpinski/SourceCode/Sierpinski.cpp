/*
* Sierpinski  Demo
* @Author         ������
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

#define SIZE 512 //Picking����ջ�Ĵ�С

GLfloat xoffset = 1; //�����x����ƫ����
GLfloat yoffset = 1; //�����y����ƫ����

bool selected = false; //�Ƿ�ѡ��

int cx = 0;  //������ĳ�ʼ����
int cy = 0;

int flag = 1; //���Ƶ����� 1.�棬2.�㣬3.��

int divide_num = 0; //ϸ�ֵĴ���

GLfloat initialfixed[3][3] = {
	{-1.0,-1.0},
	{1.0,-1.0},
	{0.0,1.0}
};           //��ʼ�����εĶ���

/* ����ԭ�� */

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
	printf("You can input the divide number of this triangle or use the key UP/DOWN to control it\n But the max number is 8 :) \n �����С���õ��е����Ϊ��ͼԪ̫С�˲���picking\n");
	glutInit(&argc, argv); //���ڳ�ʼ��
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);               //��ʼ����ʾģʽ �����ǵ�֡����ɫ����RGBģʽ
	glutInitWindowSize(500, 500);                              //��ʼ�����ڴ�С
	glutCreateWindow("@Author ������ | Sierpinski Demo");      //�������ڲ���ֵ����
	initWindow();
	menu_id = glutCreateMenu(typemenu);                        //����һ���˵����ҽ��˵�ѡ��ֵ��ֵ��menu_id��Ϊindex
	glutAddMenuEntry("Triangles", 1);                          //��˵��в���ѡ����
	glutAddMenuEntry("Points", 2);                             //��˵��в���ѡ����
	glutAddMenuEntry("Lines", 3);                              //��˵��в���ѡ����
	glutAttachMenu(GLUT_RIGHT_BUTTON);                         //���˵������¼���������Ҽ�
	glutReshapeFunc(changeWindowsSize);                        //�ı��ӿڴӶ��ػ�ͼ�η�ֹͼ�α���
	glutDisplayFunc(dispaly);                                  //��ʾ�ص�����ע��һ����ͼ����������ʹ�õ���display
	glutKeyboardFunc(getKeyboardInput);                        //�����¼��ص�����ע��һ�������¼���������
	glutSpecialFunc(Keyboardswitch);                           //���õ�ǰ���ڵ��ض����Ļص�������F1,F2 up��down�ȼ�
	glutMotionFunc(onMouseMove);                               //����ƶ��ص�����ע��һ������̬���������¼�����
	glutMouseFunc(Mouse);                                      //������¼������ص�����ע��һ����������¼��ĺ���
	glutMainLoop();                                            //�¼�����ѭ�����޵���main�еĻص�����
}

void initWindow(){
	glMatrixMode(GL_PROJECTION);         //ָ����ǰ�����ľ���GL_PROJECTIONΪͶӰ����
	glLoadIdentity();                    //����ǰ����ϵ��ԭ���ƶ�����Ļ����
	gluOrtho2D(-2.0, 2.0, -2.0, 2.0);    //����һ����ά��ͶӰ���꣬����Ϊ�ĸ��ǵ�ֵ
	glMatrixMode(GL_MODELVIEW);          //ָ����ǰ�����ľ���GL_PROJECTIONΪģ���ӽǾ���
	glClearColor(1.0, 1.0, 1.0, 1.0);    //����Ȼ�����ָ������ɫ
	glColor3f(0.0, 0.0, 0.0);            //��Ⱦͼ�ε���ɫ
}

void dispaly(){
	
	glClear(GL_COLOR_BUFFER_BIT);        //������� ��������ɫ����
	glColor3f(0.5, 0.28, 0.7);
	draw(GL_RENDER);                     //��ͼ ģʽ����Ⱦ
	glFlush();                           //ǿ��ˢ�»����ֹ�Ŷ�
}

void divide_triangle(GLfloat* a, GLfloat* b, GLfloat* c, int num,int GL_TYPE){

	/* ���εݹ��㷨 */

	GLfloat vertex_0[2],  //ÿһ�εݹ�ϸ�ּ�¼�������ζ���
		    vertex_1[2],
		    vertex_2[2];
	int i;
	if (num > 0){
		/* �����е����� */
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

	/* ��Ⱦ������ */

	glVertex2fv(a);   //���ý�Ҫ��Ⱦ�ĵ�
	glVertex2fv(b);
	glVertex2fv(c);
}
void triangle_line(GLfloat* a, GLfloat* b, GLfloat* c){

	/* ������ģʽ�������� */

	glVertex2fv(a);
	glVertex2fv(b);
	glVertex2fv(b);
	glVertex2fv(c);
	glVertex2fv(c);
	glVertex2fv(a);
}

void Keyboardswitch(int key,int x,int y){
	/* �Ӽ��̻�ȡϸ��������Ϣ */
	switch (key){
	    case GLUT_KEY_UP :{   // up��
				 if (divide_num < 8)
			     divide_num++;
			     glutPostRedisplay();  // ��ǵ�ǰ�������»���
	   		     break;
		}
		case GLUT_KEY_DOWN:{  // down ��
			     if (divide_num > 0)
				 divide_num--;
				 glutPostRedisplay();   // ��ǵ�ǰ�������»���
			     break;
		}
		default:
		break;
	}
}

void changeWindowsSize(int w, int h){

	/* �����ӿڷ�ֹͼ�α��� */

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);    //�ӿں������������ӿھ���Ĵ�С�Ӷ��ı��ӿ��ڴ����еĴ�С��λ��

	glMatrixMode(GL_PROJECTION);                 //ָ����ǰ�����ľ���GL_PROJECTIONΪͶӰ����
	glLoadIdentity();

	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h / (GLfloat)w, 1.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);  //���û��޸��޼��ռ�ķ�Χ���ֱ�Ϊx����y����z����

	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void getKeyboardInput(unsigned char key,int x, int y){

	/* �����¼� */

	if (key == 'q'){
		exit(0);
	}
	else{
		divide_num = (key - 48) % 10;
	}
	glutPostRedisplay();
}

void typemenu(int value){

	/* �˵��¼� */

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

	/* ����¼�picking */

	GLuint selectbuf[SIZE];                                      //����һ������ջ
	GLint hits;                                                  //ѡ������
	GLint viewport[4];                                           //�ӿڲ�������
	int recorder[2];                                             //�ص��������صĲ�����������

	if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN){
		
		recorder[0] = x;
		recorder[1] = y;
		
		glGetIntegerv(GL_VIEWPORT, viewport);                    //��Ŀ���ֵ��ֵ����������    ������VIEWPORT��ֵ           

		glSelectBuffer(SIZE, selectbuf);                         //����һ������ջ���ڼ�����ѡ�ж���
		glRenderMode(GL_SELECT);                                 //�޸���Ⱦģʽ������ѡ��ģʽ

		glInitNames();                                           //��ʼ������ջ
		glPushName(0);                                           //������ջѹ0

		glMatrixMode(GL_PROJECTION);                         
		glPushMatrix();                                          //����ǰ��ͶӰ����ѹ��ջ���ڱ����ֹ�������޸�
		glLoadIdentity();                              
		
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y),100.0, 100.0, viewport);   //����һ����������ѡ��������ﻭ���СΪ100 x 100
		gluOrtho2D(-2.0, 2.0, -2.0, 2.0);                                                
		draw(GL_SELECT);


		glMatrixMode(GL_PROJECTION);
		glPopMatrix();                 //����������ľ������ڻָ�
		glFlush();                 

		hits = glRenderMode(GL_RENDER);            //mode����ѡȡ��������ģʽ֮һ������ģʽ(GL_RENDER),ѡ��ģʽ(GL_SELECT),����ģʽ(GL_FEEDBACK)�������ķ���ֵ����ȷ��ѡ��ģʽ�µ����д�������ģʽ�µ�ͼԪ������
		processHits(hits, selectbuf,recorder);     //�������У���Ӧ�¼�

		glutPostRedisplay();
	}
}

void onMouseMove(int x, int y){

	/* ͨ������ƫ����������ȷ�������εĶ��� */

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

	/* �������� */

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

	/* �滭�������ƾ���ķ��� */

	if (mode == GL_SELECT) glLoadName(1); //�����ѡ��ģʽ���滻ջ��������
	glColor3f(0.5, 0.28, 0.7);
	switch (flag){
	case 1:
		glBegin(GL_TRIANGLES);           //��ʼ��ͼ�����û��������ε�ģʽ��������ʵ���ǽ�begin��end֮��ĵ㰴�ո�����ģʽ���л���    
		divide_triangle(initialfixed[0], initialfixed[1], initialfixed[2], divide_num, 0);
		glEnd();                         //��ɻ���ֹͣ���ƹ���
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