//
// Created by madina on 21.05.17.
//

#include "line.hpp"
line::line(double x, double y, double x2, double y2)  {
    //if (x2>x) {
        this->x = x;this->x2 = x2;
        this->y = y;this->y2 = y2;
    /*} else {
        this->x=x2; this->x2=x;
        this->y=y2; this->y2=y;
    }*/
}
