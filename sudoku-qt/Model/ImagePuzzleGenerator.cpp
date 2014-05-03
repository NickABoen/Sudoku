#include "ImagePuzzleGenerator.h"
#include "BoardGenerator.h"
#include "BoardSolver.h"

//Tesseract includes
#include "baseapi.h"

//OpenCV includes
#include "core.hpp"
#include "highgui.hpp"
#include "imgproc/imgproc.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for ImagePuzzleGenerator
//////////////////////////////////////////////////////////////////////////////////
ImagePuzzleGenerator::ImagePuzzleGenerator()
{
}

//////////////////////////////////////////////////////////////////////////////////
// This function tries to generates a puzzle from the given image. If there is
// a problem generating a puzzle, NULL is returned.
//////////////////////////////////////////////////////////////////////////////////
Puzzle *ImagePuzzleGenerator::generate(QString filePath)
{
    Puzzle *puzzle = new Puzzle();

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("tesseract-ocr", "eng") == -1)
    {
        //TODO error handling
        return NULL;
    }

    // Set to only recognize numbers
    api->SetVariable("tessedit_char_whitelist", "0123456789");

    // Open input image with OpenCV
    cv::Mat src = cv::imread(filePath.toStdString());
    if(src.empty())
    {
        //TODO error handling
        return NULL;
    }

    cv::cvtColor(src, src, CV_BGR2GRAY);

    cv::Mat blur;
    cv::Mat canny;
    cv::blur( src, blur, cv::Size(3,3) );

    cv::Canny(blur, canny, 120, 300, 3);
   // cv::imshow("1: Canny Edge Detection", canny);

    int maxArea, index = 0;
    cv::vector<cv::vector<cv::Point>> contours;
    cv::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> bestContour;

    cv::findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    if(contours.size() == 0)
        {
            return NULL;
        }

    for(int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);

        if(area > maxArea)
        {
            maxArea = area;
            bestContour = contours[i];
            index = i;
        }
    }

    cv::Point2f srcCorners[4];
    cv::Point2f corners[4];
    int size;

    if(maxArea > src.size().area() / 9)
    {

        cv::vector<cv::Point> approxContour;
        cv::approxPolyDP(contours[index], approxContour, 3, true);

        if(approxContour.size() < 4)
        {
            return NULL;
        }

        int topLeftIndex = 0;
        float distance = FLT_MAX;

        for(size_t i = 0; i < approxContour.size(); i++)
        {
            int deltaX = approxContour[i].x - 0;
            int deltaY = approxContour[i].y - 0;

            float tempDistance = std::sqrt((float)(deltaX * deltaX + deltaY * deltaY));

            if(tempDistance < distance)
            {
                topLeftIndex = i;
                distance = tempDistance;
            }
        }
        corners[0] = approxContour[topLeftIndex];

        int topRightIndex = 0;
        distance = FLT_MAX;

        for(size_t i = 0; i < approxContour.size(); i++)
        {
            int deltaX = approxContour[i].x - src.size().width;
            int deltaY = approxContour[i].y - 0;

            float tempDistance = std::sqrt((float)(deltaX * deltaX + deltaY * deltaY));

            if(tempDistance < distance)
            {
                topRightIndex = i;
                distance = tempDistance;
            }
        }
        corners[1] = approxContour[topRightIndex];

        int bottomLeftIndex = 0;
        distance = FLT_MAX;

        for(size_t i = 0; i < approxContour.size(); i++)
        {
            int deltaX = approxContour[i].x - 0;
            int deltaY = approxContour[i].y - src.size().height;

            float tempDistance = std::sqrt((float)(deltaX * deltaX + deltaY * deltaY));

            if(tempDistance < distance)
            {
                bottomLeftIndex = i;
                distance = tempDistance;
            }
        }
        corners[2] = approxContour[bottomLeftIndex];

        int bottomRightIndex = 0;
        distance = FLT_MAX;

        for(size_t i = 0; i < approxContour.size(); i++)
        {
            int deltaX = approxContour[i].x - src.size().width;
            int deltaY = approxContour[i].y - src.size().height;

            float tempDistance = std::sqrt((float)(deltaX * deltaX + deltaY * deltaY));

            if(tempDistance < distance)
            {
                bottomRightIndex = i;
                distance = tempDistance;
            }
        }
        corners[3] = approxContour[bottomRightIndex];

        int boardWidth = corners[1].x - corners[0].x;
        if(corners[3].x - corners[2].x < boardWidth) boardWidth = corners[3].x - corners[2].x;

        int boardHeight = corners[2].y - corners[0].y;
        if(corners[3].y - corners[1].y < boardHeight) boardHeight = corners[3].y - corners[1].y;

        size = boardWidth;
        if(boardHeight < size) size = boardHeight;

    }
    else
    {
        corners[0] = cv::Point(0,                0);
        corners[1] = cv::Point(src.size().width, 0);
        corners[2] = cv::Point(0,                src.size().height);
        corners[3] = cv::Point(src.size().width, src.size().height);

        size = src.size().width;
        if(src.size().height < size) size = src.size().height;
    }

    srcCorners[0] = cv::Point(0,    0);
    srcCorners[1] = cv::Point(size, 0);
    srcCorners[2] = cv::Point(0,    size);
    srcCorners[3] = cv::Point(size, size);

    cv::Mat newSrc(size, size, CV_8UC3);

    cv::Mat M( 2, 4, CV_32FC1 );
    M = cv::getPerspectiveTransform(corners, srcCorners);
    cv::warpPerspective(src, newSrc, M, newSrc.size());

    cv::imwrite("warped.png", newSrc);


    cv::Scalar avgPixelIntensity = cv::mean( newSrc );
    newSrc = newSrc > (avgPixelIntensity.val[0] - (255 - avgPixelIntensity.val[0])/3 );
    cv::imwrite("binaryThreshold.png", newSrc);

    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::erode(newSrc, newSrc, kernel2);

    cv::floodFill(newSrc, cv::Point(2, 2), 255);
    cv::imwrite("LinesRemoved.png", newSrc);

    api->SetImage
    (
        (uchar*)newSrc.data,
        newSrc.size().width,
        newSrc.size().height,
        newSrc.channels(),
        newSrc.step1()
    );

    char *boxtext = api->GetBoxText(0);

    // Destroy used object and release memory
    api->End();

    int cellWidth = newSrc.size().width/9;
    int cellHeight = newSrc.size().height/9;

    std::istringstream boxTextStream(boxtext);

    if(boxTextStream == NULL)
       {
           return NULL;
       }

    puzzle = new Puzzle();

    int detectedValueCount = 0;

    //file << "Parsed box text...\n";
    while (!boxTextStream.eof())
    {
       detectedValueCount++;

       int left, right, top, bottom, value, trash;

       boxTextStream >> value >> left >> bottom >> right >> top >> trash;

       //TODO for some reason it reads in the last line twice...

       float x1 = left / cellWidth;
       float x2 = right / cellWidth;
       float y1 = bottom / cellHeight;
       float y2 = top / cellHeight;

       if((1 - (x1 - (int)x1)) < (x2 - (int)x2)) x1 = x2;
       if((1 - (y1 - (int)y1)) < (y2 - (int)y2)) y1 = y2;
       puzzle->defaultBoard[(int)x1][8 - (int)y1] = value;
    }

    if(detectedValueCount == 0 || detectedValueCount > 81)
        {
            return NULL;
        }

    //TODO fix this
    int** ConvertedBoard = new int*[9];
    for(int i = 0; i < 9; i++){
        ConvertedBoard[i] = new int[9];
    }
    for(int i = 0; i<9;i++){
        for(int j = 0; j<9; j++){
            int row = 3*(i/3)+j/3;
            int column = 3*(i%3)+j%3;
            ConvertedBoard[i][j] = puzzle->defaultBoard[row][column];
        }
    }

    BoardGenerator*  boardGenerator = new BoardGenerator();

    BoardSolver boardSolver(ConvertedBoard);
    puzzle->solvedBoard = boardGenerator->ConvertBoard(boardSolver.Solve());

    return puzzle;
}




