#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include <stack>
#include <algorithm>
#include "vertex.hpp"
#include "RGBA.hpp"
#include "line.hpp"
#include "pixels.hpp"
#include <cstdlib>
using namespace std;

//-lglfw3 -lGL  -lGLU  -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread

//g++ -o lab4 vertex.cpp RGBA.cpp line.cpp pixels.cpp main.cpp -lGLU -lm -lGL -lglfw -lGLEW

static int WIDTH1 = 800, HEIGHT1 = 800;

bool f = false;
pixels pixels,buffer;
vector<vertex> vertexes;
vector<line> lines;
//int ii=0;

void error(int code, const char *desc) { fputs(desc, stderr); }

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	glOrtho(-w/h, w/h, -1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
	WIDTH1 = w; HEIGHT1 = h;
	pixels.make(w,h);
	buffer.make(w,h);
	lines.clear();
	f = false;
}

int tx(double x) {return x*(WIDTH1/2)+WIDTH1/2;}

/*float*/double rx(double x){return (x-WIDTH1/2)/(WIDTH1/2);}

int ty(double y) {return -y*(HEIGHT1/2)+HEIGHT1/2;}

double/*float*/ ry(double y){return (HEIGHT1/2-y)/(HEIGHT1/2);}

int boundary(){
	vertex max = *(max_element(vertexes.begin(),vertexes.end()));//([&vertexes]()->int{vertexes[i++].x;});
	vertex min = *(min_element(vertexes.begin(),vertexes.end()));//[vertexes](vertex v)->int{v.x;});

	int b=(abs(tx(max.x)-tx(min.x)))/2+tx(min.x);
	//cout<<"\nboundary "<< b<<endl;
	//for (int j=0;j<HEIGHT1;j++) pixels.putpoint(b,j);
	return b;
}

void makelines() {
	for(int i = 0; i < vertexes.size(); i++) {
		if (i==(vertexes.size()-1)) lines.push_back(line(tx(vertexes[i].x), ty(vertexes[i].y),tx(vertexes[0].x),ty(vertexes[0].y)));
		else lines.push_back(line(tx(vertexes[i].x), ty(vertexes[i].y),  tx(vertexes[i+1].x),ty(vertexes[i+1].y)));
	}
}

void filling(vector<line>& lines){
	bool extremum=false;
	for (int i=0;i<lines.size();i++) {
		if ((lines[(i + 1) % lines.size()].y2 - lines[(i + 1) % lines.size()].y) *
			(lines[i].y - lines[i].y2) > 0) {
			extremum = true;
			//cout<<"extremum on the end of line number   "<<ii<<endl;//cout<<"extremum = "<<extremum<<endl;
		}
		else {
			extremum = false;
			//cout<<"non extremum on the end of line number   "<<ii<<endl;//cout<<"extremum = "<<extremum<<endl;
		}
		pixels.filling(boundary(), lines[i], extremum);
		for (line l:lines) pixels.putline(l);
	}

}

void antialiasing() {
	float value=1.0f/6.0f;
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);

	glDrawPixels(WIDTH1,HEIGHT1,GL_RGBA,GL_FLOAT,pixels.buffer);
	glAccum(GL_LOAD,value);

	for (int i=-1;i<=1;i++)
		for (int j=-1;j<=1;j++){
			if (i==j==0) continue;
			for(int x=0;x<WIDTH1;x++)
				for (int y=0;y<HEIGHT1;y++){
					int curPos=(y+j)*WIDTH1+(x+i);
					cout<<"curPos = "<<curPos<<endl;
					if (curPos>=0 && curPos<WIDTH1*HEIGHT1){

						buffer.buffer[y*WIDTH1+x].R=pixels.buffer[curPos].R;
						buffer.buffer[y*WIDTH1+x].G=pixels.buffer[curPos].G;
						buffer.buffer[y*WIDTH1+x].B=pixels.buffer[curPos].B;
						buffer.buffer[y*WIDTH1+x].A=pixels.buffer[curPos].A;
					}
					else{
						buffer.buffer[y*WIDTH1+x].R=1;
						buffer.buffer[y*WIDTH1+x].G=1;
						buffer.buffer[y*WIDTH1+x].B=1;
						buffer.buffer[y*WIDTH1+x].A=1;
					}
				}
			glDrawPixels(WIDTH1,HEIGHT1,GL_RGBA,GL_FLOAT,buffer.buffer);
			glAccum(GL_ACCUM,1.0/9);//((i+j)%2!=0)? (0.7f/6.0f):(0.55f/5.0f));
		}
	glAccum(GL_RETURN,1.0f);
	//glAccum(GL_LOAD,1);
	glReadPixels(0,0,WIDTH1,HEIGHT1,GL_RGBA,GL_FLOAT,pixels.buffer);
	buffer.clear();
}

void putline(){
	glColor4f(102,0,0,127);
	glBegin(GL_LINE_LOOP);
	for (vertex vertex1 : vertexes){
		glVertex2f(vertex1.x,vertex1.y);
	}
	glVertex2f(vertexes[0].x,vertexes[0].y);
	glEnd();


}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window,GL_TRUE);
	else if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		f = true;
		makelines();
		if (vertexes.size() == 1) pixels.putpoint(tx(vertexes[0].x), ty(vertexes[0].y));
		else for(line l : lines) pixels.putline(l);
	} else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		f = false;
		//ii=0;
		pixels.clear();
		lines.clear();
	} else if (key==GLFW_KEY_Q && action==GLFW_PRESS) filling(lines);
	else if(key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		f = false;
		lines.clear();
		vertexes.clear();
		pixels.clear();
	} else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		antialiasing();
	}
}

void display(GLFWwindow *window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!f) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glColor3f(0.8,0,0);
		GLenum en = (vertexes.size() < 3 ? (vertexes.size() == 1 ? GL_POINTS : GL_LINES) : GL_POLYGON);
		glBegin(en);
			for(vertex v : vertexes)
				glVertex2f(v.x, v.y);
		glEnd();

	} else glDrawPixels(WIDTH1,HEIGHT1,GL_RGBA,GL_BYTE,pixels.buffer);
}

void mouseKey(GLFWwindow *window, int button, int action, int mode) {
	int i=0;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		if (!f) {
			vertexes.push_back(vertex((x - WIDTH1 / 2) / (WIDTH1 / 2), (HEIGHT1 / 2 - y) / (HEIGHT1 / 2)));
			//vertexes.push_back(vertex(y*WIDTH1+x));pixels.putpoint(x,y);}//vertex(rx(x), ry(y)));
			cout << "vertex.x= " << (x - WIDTH1 / 2) / (WIDTH1 / 2) << "  vertex,y= " <<(HEIGHT1 / 2 - y) / (HEIGHT1 / 2) << endl;
			cout << "tx(rx(x))= " << tx(rx(x)) << "  ty(ry(y))= " <<ty(ry(y))<< endl;
			cout << "x= " << x << "  y= " << y << endl;
		}
		else filling(lines);//antialiasing();
	}
}

int main() {
	glfwSetErrorCallback(error);
	if(!glfwInit()) {
		std::cout << "Failed to init glfw" << std::endl;
		return 0;
		//exit(1);
	}
	GLFWwindow *window = glfwCreateWindow(WIDTH1, HEIGHT1, "4Lab", NULL, NULL);
	if(!window) {
		std::cout << "Failed to create the window" << std::endl;
		glfwTerminate();
		return 0;
		//exit(1);
	}
	glfwMakeContextCurrent(window);
	glewInit();
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouseKey);
	glClearColor(0,0,0,0);
	resize(window,WIDTH1,HEIGHT1);
	while(!glfwWindowShouldClose(window)) {
		display(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

