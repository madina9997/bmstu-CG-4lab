//
// Created by madina on 21.05.17.
//

#include "vertex.hpp"
static int WIDTH1 = 800, HEIGHT1 = 800;

vertex vertex::board() {
    return (x == 0 ? vertex(x+1,y) : *this);
}

bool vertex::in() {
    return x <= WIDTH1 && x >= 0 && y >=0 && y <= HEIGHT1;
}

bool vertex::isNull() {
    return x ==-1 && y == -1;
}

vertex vertex::next() {
    return vertex(x+1, y);
}

vertex vertex::prev() {
    return vertex(x-1, y);
}

vertex vertex::up() {
    return vertex(x,y+1);
}

vertex vertex::down() {
    return vertex(x, y-1);
}

vertex::vertex(double x, double y) {
    this->x = x;
    this->y = y;
}
