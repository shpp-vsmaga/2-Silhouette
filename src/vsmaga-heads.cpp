/** File: vsmaga-heads.cpp
 * ------------------------------------------------------------
 *
 * In this file are located functions required for the algorithm
 * of searching human silhouettes by the heads.
 */

#include <iostream>
#include "console.h"
#include "gwindow.h" // for GWindow
#include "simpio.h"  // for getLine
#include "gbufferedimage.h"
#include "point.h"
#include "filelib.h"

/* Own collections */
#include "vectorshpp.h"
#include "queueshpp.h"

const double CORRECTION_OF_SHARP_CHANGE = 0.026;
const double CORRECTION_OF_SMOOTH_CHANGE = 0.022;
const int DELTA_RANGE = 3;

/* Function prototypes*/
int findHeadsInAllObj (VectorSHPP<VectorSHPP<Point>> objects);
int findHeads (VectorSHPP<Point> &object);
int findMinY(int x, VectorSHPP<Point> &object);
int goThroughTopLine(VectorSHPP<int> &topLine, int objectHeigth);
int findHeigth(VectorSHPP<Point> &object);
int findMinX(VectorSHPP<Point> &object);
int findMaxX(VectorSHPP<Point> &object);


/**
 * Function:  findHeadsInAllObj
 * Usage: int heads = findHeadsInAllObj(objects);
 * -------------------------------------------------------------
 *
 * This function find heads in all objects
 *
 * @param objects Reference to Vector of objects
 * @return Number of heads
 */
int findHeadsInAllObj(VectorSHPP<VectorSHPP<Point> > objects){
    int heads = 0;
    for (int i = 0; i < objects.size(); i++){
        heads += findHeads(objects[i]);
    }
    return heads;
}



/**
 * Fucntion: findHeads
 * Usage: heads += findHeads(objects[i]);
 * -----------------------------------------------------------
 *
 * This function find heads in single object.
 * It builds the line of the top points of the object
 * and analyzes it for changes similar to heads.
 *
 * @param object Reference to Vector of single object
 * @return Number of heads in single object
 */
int findHeads (VectorSHPP<Point> &object){
    VectorSHPP<int> topLine;

    int leftX = findMinX(object);
    int rightX = findMaxX(object);

    for (int i = leftX; i <= rightX; i++){
            topLine.add(findMinY(i, object));
    }

    int objectHeigth = findHeigth(object);
    int heads = goThroughTopLine(topLine, objectHeigth);

    return heads;
}


/**
 * Function: findMinX
 * Usage: int leftX = findMinX(object);
 * --------------------------------------------------------------
 *
 * It finds is the minimum value of X in the points of received object
 *
 * @param object Reference to Vector of single object
 * @return Int value of the minimum X
 */
int findMinX(VectorSHPP<Point> &object){
    int left = object.get(0).getX();;
    for (int i = 0; i < object.size(); i++){
        Point point = object.get(i);
        if (point.getX() < left) left = point.getX();
    }
    return left;
}


/**
 * Function: findMaxX
 * Usage: int rightX = findMaxX(object);
 * -------------------------------------------------------------------
 *
 * It finds is the maximum value of X in the points of received object
 *
 * @param object Reference to Vector of single object
 * @return Int value of the maximum X
 */
int findMaxX(VectorSHPP<Point> &object){
    int right = object.get(0).getX();;
    for (int i = 0; i < object.size(); i++){
        Point point = object.get(i);
        if (point.getX() > right) right = point.getX();
    }
    return right;
}


/**
 * Function:  findHeigth
 * Usage:  int objectHeigth = findHeigth(object);
 * ------------------------------------------------------------------
 *
 * This function finds the height of the received object. It calculates
 * minimum and maximum values of Y and returns difference between them.
 *
 * @param object Reference to Vector of single object
 * @return Height of the received object
 */
int findHeigth(VectorSHPP<Point> &object){
    int top = object.get(0).getY();
    int down = object.get(0).getY();

    for (int i = 0; i < object.size(); i++){
        Point point = object.get(i);
        if (point.getY() < top) top = point.getY();
        if (point.getY() > down) down = point.getY();
    }

    return down - top;
}


/**
 * Function: findMinY
 * Usage: topLine.add(findMinY(i, object));
 * ---------------------------------------------------------------------
 *
 * This finds minimal (top) Y in received X-line
 *
 * @param x Coordinate of X-line to find minimum Y
 * @param object Reference to Vector of single object
 * @return Minimum value of Y in the received X-line
 */
int findMinY(int x, VectorSHPP<Point> &object){
    int y = object.get(0).getY();
    for (int i = 0; i < object.size(); i++){
        Point point = object.get(i);
        if(point.getX() == x && point.getY() <= y) y = point.getY();
    }
    return y;
}


/**
 * Function: goThroughTopLine
 * Usage: int heads = goThroughTopLine(topLine, objectHeigth);
 * -------------------------------------------------------------------
 *
 * This function analyzes the line of the top points of the object
 * and finds changes similar to heads. It use four condiotions of changes
 * in the line, and if all of the conditions are fulfilled in right order
 * that means head is found.
 *
 * @param line Vector of the top points of the object
 * @param objectHeigth Height of the object
 * @return Number of heads in the object
 */
int goThroughTopLine(VectorSHPP<int> &line, int objectHeigth){
    int result = 0;
    int dy = 0; // change of line between three points
    int coefSide = objectHeigth * CORRECTION_OF_SHARP_CHANGE; //coefficient of the sharp change of the line
    int coefTop = objectHeigth * CORRECTION_OF_SMOOTH_CHANGE; //coefficient of the smooth change of the line
    bool sharpRise = false; //Sharp rise trigger
    bool smoothRise = false; //Smooth rise trigger
    bool smoothDescent = false; //Smooth descent trigger

    for(int i = 0; i < line.size(); i++){

        /* Begin to read changes from the third point*/
        if (i >= DELTA_RANGE){
            dy = line.get(i - DELTA_RANGE) - line.get(i);
        }

        /* Turn on trigger if sharp rise found*/
        if (dy >= coefSide){
            sharpRise = true;
        }

        /* Turn on trigger if smooth rise found and it was sharp rise before it*/
        if (dy <= coefTop && dy > 0 && sharpRise ){
            smoothRise = true;
        }

        /* Turn on trigger if smooth descent found and it was sharp rise and smooth rise before it*/
        if (dy >= -coefTop && sharpRise && smoothRise && dy < 0){
           smoothDescent = true;
        }

        /* Adds one head to the result if sharp descent found and  all previous conditions are true*/
        if (dy <= -coefSide && smoothDescent && sharpRise && smoothRise ){
            result++;
            smoothDescent = sharpRise = smoothRise = false; //zero all triggers
        }
    }


    return result;
}
