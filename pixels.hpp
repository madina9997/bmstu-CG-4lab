//
// Created by madina on 21.05.17.
//

#ifndef LAB4_PIXELS_HPP
#define LAB4_PIXELS_HPP


#include <GL/glew.h>
#include <vector>
#include "RGBA.hpp"
#include "line.hpp"
#include "vertex.hpp"

class pixels {
public:
    RGBA *buffer = NULL;
    int w, h;
    void make(int w, int h);
    void clear();
    void putline(line l);
    void filling(int boundary,line l);
    void putpoint(int x, int y);
    int sign(int x);
    void rasterize(vertex v);
    void antialise(line l);
    bool isBlack(int x, int y);
    bool isRed(int x, int y);
    void colour(int x, int y, GLbyte R, GLbyte G, GLbyte B, GLbyte A);
    virtual ~pixels();
};


#endif //LAB4_PIXELS_HPP
