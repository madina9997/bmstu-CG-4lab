//
// Created by madina on 21.05.17.
// заполнения многоугольника по ребрам с перегородкой;
// постфильтрация с взвешенным усреднением области 3х3(уменьшение разрешения в два раза) (с использованием аккумулирующего буфера);
//

#include <cstring>
#include <stdlib.h>
#include "pixels.hpp"
#include <stack>
#include <vector>
#include <iostream>
using namespace std;
#include <algorithm>
int WIDTH1 = 800, HEIGHT1 = 800;
bool pixels::isBlack(int x, int y) {
    return buffer[(HEIGHT1-y)*WIDTH1+x].R == 0 && buffer[(HEIGHT1-y)*WIDTH1+x].G == 0
           && buffer[(HEIGHT1-y)*WIDTH1+x].B  == 0;
}
bool pixels::isRed(int x, int y) {
    return buffer[(HEIGHT1-y)*WIDTH1+x].R == 102 && buffer[(HEIGHT1-y)*WIDTH1+x].G == 0
           && buffer[(HEIGHT1-y)*WIDTH1+x].B  == 0;
}


void pixels::colour(int x, int y, GLbyte R, GLbyte G, GLbyte B, GLbyte A) {
    buffer[(HEIGHT1-y)*WIDTH1+x].R = R;
    buffer[(HEIGHT1-y)*WIDTH1+x].G = G;
    buffer[(HEIGHT1-y)*WIDTH1+x].B = B;
    buffer[(HEIGHT1-y)*WIDTH1+x].A = A;
}

int pixels::sign(int x) {return (x > 0) ? 1 : ((x < 0) ? -1 : 0);}

void pixels::putpoint(int x, int y) {colour(x,y,102,0,0,127);}

void pixels::putline(line l) {
    // line from x,y to x2,y2
    int x = l.x, y = l.y, x2 = l.x2, y2 = l.y2;
    int dx = abs(x2-x), dy = abs(y2-y);
    int s1 = sign(x2-x), s2 = sign(y2-y);
    bool change;
    if(dy>dx) {
        dx^=dy; dy^=dx; dx^=dy;
        change = true;
    } else change = false;
    int t = 0;
    while(x != x2 || y != y2) {
        colour(x,y,102,0,0,127);
        if(t>=0) {
            if(change) x+=s1;else y+=s2;
            t-=dx;
        }
        else {
            t+=dy;
            if(change) y+=s2;else x+=s1;
        }
    }
}

void pixels::filling(int boundary,line l,bool extremum) {
    putline(l);
    int x = l.x, y = l.y, x2 = l.x2, y2 = l.y2;
    int dx = abs(x2-x), dy = abs(y2-y);
    int s1 = sign(x2-x), s2 = sign(y2-y);

    bool change;
    if(dy>dx) {
        dx^=dy; dy^=dx; dx^=dy;
        change = true;
    } else change = false;
    int t = 0;
    bool prev=false;
    while(/*x != x2 ||*/ y != y2) {
        colour(x,y,102,0,0,127);
        int t1,t2;
        if ((x-boundary)<0) {t1=x;t2=boundary-1;} else {t1=boundary;t2=x;}

        if (!prev) for (int i = t1; i <= t2 ; i++) {
            if (!(isRed(i, y))) colour(i,y,102,0,0,127);
            else colour(i, y, 0, 0, 0, 127);
        }
        if(t>=0) {
            if(change) {x+=s1;prev=true;}
            else {y+=s2;prev=false;}
            t-=dx;
        }
        else {
            t+=dy;
            if(change) {y+=s2;prev=false;}
            else {x+=s1;prev=true;}
        }
    }
    if (extremum) {
        int t1, t2;
        if ((x - boundary) < 0) {
            t1 = l.x2 + 1;
            t2 = boundary-1;
        } else {
            t1 = boundary;
            t2 = l.x2 - 1;
        }

        if (!prev)/*(previous_y!=y)*/ for (int i = t1; i <= t2; i++) {
                if (!(isRed(i, y))) colour(i, y, 102, 0, 0, 127);
                else colour(i, y, 0, 0, 0, 127);
            }
    }
}

pixels::~pixels() {if(buffer != NULL) delete[] buffer;}

void pixels::clear() {memset(buffer,0,8*(w+1)*(h+1));}

void pixels::make(int w, int h) {
    if(buffer != NULL) delete[] buffer;
    buffer = new RGBA[8*(w+1)*(h+1)];
    this->w = w;
    this->h = h;
    memset(buffer,0,8*(w+1)*(h+1));
}
