//
// Created by madina on 21.05.17.
//

#ifndef LAB4_VERTEX_HPP
#define LAB4_VERTEX_HPP


class vertex {
public:
    double x,y;
    int number;
    static vertex null() { return vertex(-1,-1); };
    vertex(double x, double y);
    vertex(int number);
    vertex next();
    vertex prev();
    vertex up();
    bool isNull();
    bool in();
    vertex down();
    vertex board();
    bool operator<(vertex& u){return this->x<u.x;}
    bool operator>(vertex& u){return this->x>u.x;}
};


#endif //LAB4_VERTEX_HPP
