GBufferedImage* drawObjectInLiline(VectorSHPP <VectorSHPP<Point>>setObjects, int maxWidth, int maxHeight);
void setSizeWindow(VectorSHPP <VectorSHPP<Point>> setObjects, int maxHeight, int maxWidth, GBufferedImage* imageInLine);

int findPeople(GBufferedImage *image, GBufferedImage *newImage);
float sumPixelsInOneYCoordinate(int x, GBufferedImage *image);
float findMaxPixelsCol(VectorSHPP <float> &sumPixelsInWidth);
VectorSHPP<int> stabiliationPixels(VectorSHPP <float> &sumPixelsInWidth, float maxNumber);
VectorSHPP<int> flipArrayValues(VectorSHPP<int> stabSum);
void drawGraph(VectorSHPP<int> &stabSum, GBufferedImage *newImage);
int goThroughLine(VectorSHPP<int> &line);



// contains a value in pixels, the minimum size of an object
const int MIN_OBJECT_SIZE = 50;

// height of the image which will draw graphics
const int HEIGHT_IMAGE_GRAPH = 200;

// the distance between objects when they will draw in a line
const int SIZE_BETWEEN_OBJECTS = 5;


/**
 * Method: drawObjectInLiline
 * Usage: GBufferedImage* objectsInLine = drawObjectInLiline(VectorSHPP <VectorSHPP<Point>>setObjects,
 *                                               int maxWidth, int maxHeight)
 * _____________________________________________________________________________________________________
 *
 * This method specifies size of new picture, the height same the highest object multiplied by 1.3
 * (this is the number of attached to the free space at the top of image), and the width equal to the
 * width of all objects. After that draw objects on a new image in a line.
 *
 * @param setObjects - two dimensional vector of the points of the objects
 * @param maxWidth - width of the original image, it need for calculate the min and max size of the object
 * @param maxHeight - height of the original image, it need for calculate the min and max size of the object
 * @return - image stream in which all objects are located in one line
 */
GBufferedImage* drawObjectInLiline(VectorSHPP <VectorSHPP<Point>>setObjects, int maxWidth, int maxHeight){
    GBufferedImage* imageInLine = new GBufferedImage();
    int heightTmp;
    int startXPos = 0;

    // sets the size of the new window and images
    setSizeWindow(setObjects, maxHeight, maxWidth, imageInLine);
    int heightWindow = imageInLine->getHeight();

    // takes every object from the array
    for(int j = 0; j < setObjects.size(); j++){
        VectorSHPP<Point> object = setObjects.get(j);
        int minX = maxWidth;
        int maxX = 0;
        int minY = maxHeight;
        int maxY = 0;

        // calculates the minimum and maximum coordinate values of X and Y, for each object
        for (int i = 0; i < object.size(); i++){
            Point pointTmp = object[i];
            int pointX = pointTmp.getX();
            int pointY = pointTmp.getY();

            if(pointX < minX) minX = pointX;
            else if (pointX > maxX) maxX = pointX;

            if(pointY < minY) minY = pointY;
            else if (pointY > maxY) maxY = pointY;
        }

        // the height of each object
        heightTmp =  maxY - minY;

        // draws all objects in one line
        for (int i = 0; i < object.size(); i++){
            Point point = object.get(i);

            int tmpX = (point.getX() - minX) + startXPos;
            int tmpY = (heightWindow - heightTmp) + (point.getY() - minY);
            imageInLine->setRGB(tmpX, tmpY - 1, 0x000000);
        }
        startXPos = startXPos + (maxX - minX) + SIZE_BETWEEN_OBJECTS; // change start position of the X coordinate for each object

    }
    return imageInLine;
}

/**
 * Method: setSizeWindow
 * Usage: setSizeWindow(VectorSHPP <VectorSHPP<Point>> setObjects, int maxHeight, int maxWidth,
 *                                          GBufferedImage* imageInLine)
 * __________________________________________________________________________________________________
 *
 * Set size for new image, building on size of the objects
 *
 * @param setObjects - two dimensional vector of the points of the objects
 * @param maxHeight - height of the original image, it need for calculate the min and max size of the object
 * @param maxWidth - width of the original image, it need for calculate the min and max size of the object
 * @param imageInLine - image where will be placed all objects
 */
void setSizeWindow(VectorSHPP <VectorSHPP<Point>> setObjects, int maxHeight, int maxWidth, GBufferedImage* imageInLine){
    int height;
    int width;
    int maxHeightObject = 0;
    int widthWindow = 0;

    // takes every object from the array
    for(int i = 0; i < setObjects.size(); i++){
        VectorSHPP<Point> object = setObjects[i];
        int minY = maxHeight;
        int maxY = 0;
        int minX = maxWidth;
        int maxX = 0;

        // check each point of the object, and calculates
        // the width of all objects, and the height of the highest object
        for(int j = 0; j < object.size(); j++){
            Point tmpPoint = object[j];
            int pointY = tmpPoint.getY();
            int pointX = tmpPoint.getX();

            if(pointY < minY) minY = pointY;
            else if (pointY > maxY) maxY = pointY;

            if(pointX < minX) minX = pointX;
            else if (pointX > maxX) maxX = pointX;
        }
        width = maxX - minX;
        height =  maxY - minY;
        widthWindow += (width + SIZE_BETWEEN_OBJECTS);
        if(height > maxHeightObject){
            maxHeightObject = height;
        }
    }

    // sets the size of window and image
    imageInLine->resize(widthWindow, maxHeightObject * 1.3);
    imageInLine->fill(0xffffff);
}

/**
 * Method: findPeople
 * Using: findPeople(GBufferedImage *image, GBufferedImage *newImage)
 * __________________________________________________________________
 *
 * Method processes images with objects, calculates the "weight" of the axes Y,
 * after which visualizes it as a graph, and counts the number of people on graph.
 *
 * @param image - image stream, which shows all objects
 * @param newImage - image stream to visualize graphics
 */
int findPeople(GBufferedImage *image, GBufferedImage *newImage){
    VectorSHPP <float> sumPixelsInWidth;
    for (int x = 1; x < image->getWidth()-1; x++){
        sumPixelsInWidth.add(sumPixelsInOneYCoordinate(x, image));
    }
    float maxNumber = findMaxPixelsCol(sumPixelsInWidth);
    VectorSHPP<int> stabSum = stabiliationPixels(sumPixelsInWidth, maxNumber);
    stabSum = flipArrayValues(stabSum);
    drawGraph(stabSum, newImage);
    return goThroughLine(stabSum);

//    if (res > 1){
//        cout << "On this image, we found approx " << res << " peoples" << endl;
//    } else {
//        cout << "On this image, we found approx " << res << " people" << endl;
//    }
}

/**
 * Method: sumPixelsInOneYCoordinate
 * Usage: float sumPixels = sumPixelsInOneYCoordinate(int x, GBufferedImage *image)
 * ________________________________________________________________________________
 *
 * Method calculates the number of each black pixels in axis Y, as well as all the
 * neighboring pixels. For each pixel calculated a certain number, the number becomes
 * smaller when the neighboring pixels being away, as well as the number becomes
 * smaller towards the bottom of the image.
 *
 * @param x - X coordinate on which needs to process all pixels on the Y axis
 * @param image - stream image
 * @return "weight" Y axis
 */
float sumPixelsInOneYCoordinate(int x, GBufferedImage *image){
    float col = 0;

    float yv = 10; // reduces the "cost" of each pixel is closer to the legs
    for (int y = 1; y < image->getHeight()-1; y++){
        yv = yv/1.01;
        if (image->getRGB(x,y) < 0xefefef){
            int tmpX = x;
            float i = yv;
            // considers all the black pixels to the left
            if(tmpX > 1){
                while(image->getRGB(tmpX-1,y) < 0xefefef && tmpX-1 > 1){
                    col = col + i;
                    i = i/1.1;
                    tmpX--;
                }
            }
            tmpX = x;
            i = yv;
            if(tmpX < image->getWidth()-1){
                // considers all the black pixels to the right
                while(image->getRGB(tmpX+1,y) < 0xefefef && tmpX+1 < image->getWidth()-1){
                    col = col + i;
                    i = i/1.1;
                    tmpX++;
                }
            }
        }
    }
    return col;
}

/**
 * Method: findMaxPixelsCol
 * Usage: float max = findMaxPixelsCol(VectorSHPP <float> &sumPixelsInWidth)
 * _________________________________________________________________________
 *
 * This method finds the greatest value of the number of vector
 *
 * @param sumPixelsInWidth - Vector with numbers
 * @return the maximum number
 */
float findMaxPixelsCol(VectorSHPP <float> &sumPixelsInWidth){
    int maxNumber = 0;
    for(int i = 0; i < sumPixelsInWidth.size(); i++){
        int tmp = sumPixelsInWidth[i];
        if(tmp > maxNumber) maxNumber = tmp;
    }
    return maxNumber;
}

/**
 * Method: stabiliationPixels
 * Using: VectorSHPP<int> stableNumber = stabiliationPixels(VectorSHPP <float> &sumPixelsInWidth, float maxNumber)
 * _______________________________________________________________________________________________________________
 *
 * This method stabilizes all numbers in a percentage range from 0 to 100
 *
 * @param sumPixelsInWidth - vector numbers type float
 * @param maxNumber - The maximum number of the input vector
 * @return - vector numbers type int
 */
VectorSHPP<int> stabiliationPixels(VectorSHPP <float> &sumPixelsInWidth, float maxNumber){
    VectorSHPP<int> res;
    for(int i = 0; i < sumPixelsInWidth.size(); i++){
        int number = sumPixelsInWidth.get(i);
        if(number == 0){
            res.add(0);
        } else {
            float tmp = (number/maxNumber);
            res.add((int)(tmp*100));
        }
    }
    return res;
}

/**
 * Method: flipArrayValues
 * Usage: VectorSHPP<int> flipValues = flipArrayValues(VectorSHPP<int> stabSum)
 * ___________________________________________________________________________
 *
 * Receives vector Int and makes negative values
 *
 * @param stabSum - vector values type int
 * @return - returns the input vector with negative values
 */
VectorSHPP<int> flipArrayValues(VectorSHPP<int> stabSum){
    VectorSHPP<int> res;
    for(int i = 0; i < stabSum.size(); i++){
        int y = stabSum[i];
        res.add(y*(-1));
    }
    return res;
}

/**
 * Method: drawGraph
 * Usage: drawGraph(VectorSHPP<int> &stabSum, GBufferedImage *newImage)
 * ___________________________________________________________________
 *
 * Receives vector of int where each value - a visualization of the
 * "weight" of Y coordinate and draws these values on the graph.
 *
 * @param stabSum - vector of numbers from 0 to 100
 * @param newImage - stream image
 */
void drawGraph(VectorSHPP<int> &stabSum, GBufferedImage *newImage){
    int x = 1;
    for(int i = 0; i < stabSum.size(); i++){
        int y = stabSum[i];
        newImage->setRGB(x, y + (newImage->getHeight()-1), 0x000000);
        x++;
    }
}

/**
 * Method: goThroughLine
 * Usage: int colPeople = goThroughLine(VectorSHPP<int> &line)
 * ___________________________________________________________
 *
 * Method considers delta from the chart. If in the last five
 * pixels has been the growth of delta for more than 10 pixels,
 * and shortly thereafter follow descent of delta for more than
 * 10 pixels, then method counts one person.
 *
 * @param line - vector of numbers from 0 to 100
 * @return - number of people silhouettes
 */
int goThroughLine(VectorSHPP<int> &line){
    int result = 0;
    int dy = 0;
    bool trigUp = false;

    for(int i = 0; i < line.size(); i++){
        if (i >= 5) dy = line[i - 5] - line[i];

        if (dy >= 10) trigUp = true;

        if (dy <= -10 && trigUp){
            result++;
            trigUp = false;
        }
    }
    return result;
}
