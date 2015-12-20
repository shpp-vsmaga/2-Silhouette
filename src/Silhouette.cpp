/** File: Silhouette.cpp
 * ---------------------------------------------------------------------
 *
 * This program finds all human silhouettes on the black and white image
 * received from the user using two algorithms - searching by heads and
 * and searching by center of mass.
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


#include "valekseev-centermass.cpp"
#include "vsmaga-heads.cpp"

using namespace std;


/* Function prototypes*/
VectorSHPP<VectorSHPP<Point>> findObjects(GBufferedImage* image);
VectorSHPP<Point> findObject (GBufferedImage* image, Point &point);
void findPeopleInPicture(string filename);


/* Main function*/
int main() {
    Vector<string> pictures;
    pictures.add("6.jpg");
    pictures.add("8.jpg");
    pictures.add("9.jpg");
    pictures.add("11.jpg");
    pictures.add("bz.jpg");
    pictures.add("sil3.jpg");
    pictures.add("1-1.jpg");
    pictures.add("5.jpg");

    for(string picture: pictures){
        cout << "Press ENTER to open next picture" << endl;
        cin.get();
        findPeopleInPicture(picture);
    }
    return 0;
}

void findPeopleInPicture(string fileName){
    cout << "Please wait!!!" << endl;
    VectorSHPP<VectorSHPP<Point>> objects;
    GWindow window;
    GBufferedImage* image = new GBufferedImage();
    image->load(fileName);
    GBufferedImage* screenImage = new GBufferedImage();
    screenImage->load(fileName);
    window.setCanvasSize(image->getWidth(),image->getHeight());
    window.add(screenImage);

    objects = findObjects(image);

    //------valekseev-centermass.cpp-------------------------------
    GBufferedImage* imageInLine;
    imageInLine = drawObjectInLiline(objects, image->getWidth(), image->getHeight());
    delete image;
    GBufferedImage *newImage = new GBufferedImage(imageInLine->getWidth(), HEIGHT_IMAGE_GRAPH, 0xffffff);
    int masses = findPeople(imageInLine, newImage);
    delete imageInLine;
    delete newImage;

    //-------vsmaga-heads.cpp---------------------------------------
    int heads = findHeadsInAllObj(objects);
    int objects_count = objects.size();
    int people = (masses + heads) / 2;
    if ((masses + heads) % 2 >= 0.5) people++;

    cout << "Objects found: " << objects_count << endl;
    cout << "Heads found: " << heads  << endl;
    cout << "People found by masses: " << masses  << endl;
    cout << "People found: " << people  << endl << endl;
}


/**
 * Function: findObjects
 * Usage: objects = findObjects(image);
 * -----------------------------------------------------------------------------
 *
 * This function finds all objects of black color in whole image.
 * It finds the nearest black point and sends it to findObject function,
 * that find all black points attached to this point, adds them to
 * Vector of one object and redraws to white color.
 *
 * @param image Pointer to GBufferedImage
 *
 * @return Vector of the all objects. Every single object - Vector of Points
 */
VectorSHPP<VectorSHPP<Point>> findObjects(GBufferedImage* image){
    VectorSHPP<VectorSHPP<Point>> objects;
    VectorSHPP<Point> currentObject;
    int width = image->getWidth();
    int height = image->getHeight();
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            Point currentPoint(x,y);

            if(image->getRGB(x,y) == 0x000000){
                currentObject = findObject(image, currentPoint);
                if(currentObject.size() > 50)
                    objects.add(currentObject);
            }
        }
    }
    return objects;
}

/**
 * Function: findObject
 * Usage: currentObject = findObject(image, currentPoint);
 * --------------------------------------------------------------------------
 *
 * This function finds single object of the black color and redraw it to white.
 * It receive one black point and finds all black points attached to this point,
 * adds them to the result Vector of single object and redraws them to white color.
 *
 * @param image Pointer to GBufferedImage
 * @param point Reference to point - start of the new object
 *
 * @return VectorSHPP<Point> with single object
 */
VectorSHPP<Point> findObject(GBufferedImage* image, Point &point){
    VectorSHPP<Point> result;
    QueueSHPP<Point> points;
    points.enqueue(point);

    while (!points.isEmpty()) {
        Point startPoint = points.dequeue();

        result.add(startPoint);

        for(int x = startPoint.getX() - 1; x <= startPoint.getX() + 1; x ++)
            for(int y = startPoint.getY() - 1; y <= startPoint.getY() + 1; y++){
                if (x > 0 && y > 0 && x < image->getWidth() - 1 && y < image->getHeight() - 1){
                    if (image->getRGB(x ,y) < 0xefefef){
                        Point currentPoint(x, y);
                        points.enqueue(currentPoint);
                        image->setRGB(currentPoint.getX(), currentPoint.getY(), 0xffffff);
                    }
                }
            }
    }
    return result;
}


