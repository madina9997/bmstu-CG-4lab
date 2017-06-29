//
// Created by madina on 21.05.17.
// заполнения многоугольника по ребрам с перегородкой;
// постфильтрация с взвешенным усреднением области 3х3 (с использованием аккумулирующего буфера);
//

#include <cstring>
#include <stdlib.h>
#include "pixels.hpp"
#include <stack>
#include <vector>
#include <iostream>
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

void pixels::filling(int boundary,line l) {
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
    //int previous_y=y;
    while(x != x2 || y != y2) {
        //colour(x,y,102,0,0,127);
        int t1,t2;
        if ((x-boundary)<0) {t1=x;t2=boundary-1;} else {t1=boundary;t2=x;}

        if (!prev)/*(previous_y!=y)*/ for (int i = t1; i <= t2 ; i++) {
            if (!(isRed(i, y))) colour(i,y,102,0,0,127);
            else colour(i, y, 0, 0, 0, 127);
        }
        //colour(x,y,102,0,0,127);
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
        //previous_y=y;
    }
    /*int t1,t2;
    if ((x-boundary)<0) {t1=l.x2+1;t2=boundary;} else {t1=boundary+1;t2=l.x2-1;}

    if (!prev)/*(previous_y!=y)/ for (int i = t1; i <= t2 ; i++) {
            if (!(isRed(i, y))) colour(i,y,102,0,0,127);
            //else colour(i, y, 0, 0, 0, 127);
        }*/
    //}
    /*int i,j=std::min(l.y,l.y2),b,b2=std::max(l.y2,l.y);
    bool a;
    if (l.x<boundary) {i=l.x;b=boundary;a=true;} else {i=boundary;b=l.x2;a=false;}
    for (j;j<=b2;j++) {
        if (a)
            for (i = intersection_with_oy(l, j); i <= b; i++) {
                if (isBlack(i, j)) putpoint(i, j);
                else colour(i, j, 0, 0, 0, 0);
            }
        else {
            for (i; i < intersection_with_oy(l, j); i++) {
                if (isBlack(i, j)) putpoint(i, j);
                else colour(i, j, 0, 0, 0, 0);
            }
        }
    }*/
}

void pixels::rasterize(vertex v) {
    std::stack<vertex> s;
    s.push(v);
    while(!s.empty()) {
        vertex k = s.top(), j = vertex::null(); s.pop();
        colour(k.x,k.y,102,0,0,127);
        for(j = k.next(); isBlack(j.x,j.y) && //right
                          j.in(); colour(j.x,j.y,102,0,0,127), j = j.next());

        vertex xr = j.prev();

        for(j = k.prev(); isBlack(j.x,j.y) && //left
                          j.in(); colour(j.x,j.y,102,0,0,127), j = j.prev());

        vertex xl = j.next().board();
        vertex p = vertex::null();
        //top
        for(j = xl.prev().up(); j.in() && j.x <= xr.x+1 ; j = j.next()) {
            if(isBlack(j.x,j.y)) p = j;
            else if(!p.isNull()) {s.push(p); p = vertex::null();}
        }
        if(!p.isNull()) s.push(p);
        //bot
        for(j = xl.prev().down(), p = vertex::null(); j.in() && j.x <= xr.x+1 ; j = j.next()) {
            if(isBlack(j.x,j.y)) p = j;
            else if(!p.isNull()) {s.push(p); p = vertex::null();}
        }
        if(!p.isNull()) s.push(p);
    }
}

void pixels::antialise(line l) {
    int x1 = l.x, y1 = l.y, x2 = l.x2, y2 = l.y2;
    int dx = abs(x2 - x1), dy = abs(y2 - y1), s1 = sign(x2 - x1), s2 = sign(y2 - y1);
    float e = 102.0f / 2.0f;
    bool change;
    if(dy>dx) {
        dx^=dy; dy^=dx; dx^=dy;
        change = true;
    } else change = false;
    float m = ((float) 102.0f * dy) / dx;
    float w = 102.0f - m;
    colour(x1, y1, m/2, 0, 0, 127);
    for (int i = 1; i <= dx; i++) {
        if(change) y1+=s2;
        else x1+=s1;
        if(e >= w) {
            if(change) x1+=s1;
            else y1+=s2;
            e-=w;
        } else {
            e+=m;
        }

        colour(x1, y1, 102.0f - e , 0 , 0, 127);
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
