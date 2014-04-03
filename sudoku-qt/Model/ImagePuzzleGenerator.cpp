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

ImagePuzzleGenerator::ImagePuzzleGenerator()
{
}

// comparison function object
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = cv::contourArea(contour1);
    double j = cv::contourArea(contour2);
    return ( i < j );
}

Puzzle *ImagePuzzleGenerator::generate(QString filePath)
{
    Puzzle *puzzle = new Puzzle();

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init("tesseract-ocr", "eng") == -1)
    {
        //TODO error handling

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

    cv::blur( src, blur, cv::Size(3,3) );

    cv::Mat canny;

    cv::Canny(blur, canny, 120, 300, 3);
    cv::imwrite("Canny1.png", canny);

    cv::vector<cv::vector<cv::Point>> contours;
    cv::vector<cv::Vec4i> hierarchy;

    cv::findContours(canny, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    int maxArea = 0;
    std::vector<cv::Point> bestContour;
    int index = 0;

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

    //cv::adaptiveThreshold(newSrc, newSrc, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 2);

    //cv::imwrite("adaptiveThresh.png", newSrc);

   // cv::floodFill(newSrc, newSrc, cv::Point(1, 1),cv::Scalar(0));


//    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1, 1));

    //cv::erode(newSrc, blur, kernel2);

   // cv::imwrite("erode.png", blur);
   // cv::blur( blur, blur, cv::Size(3,3) );

   // cv::Canny(blur, canny, 150, 250, 3);


    cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

//   // cv::bitwise_not(canny, canny);

    cv::erode(newSrc, newSrc, kernel2);

//    cv::Mat kernel3 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
//   // cv::dilate(canny, canny, kernel3);

//    cv::bitwise_not(canny, canny);
//    cv::erode(canny, canny, kernel3);

   // cv::imwrite("Canny2.png", canny);

   // cv::Mat mask(cv::Point(size + 2, size + 2), CV_8UC1, cv::Scalar(0));
    cv::Point p(0, 0);

    cv::floodFill(newSrc, p, 255);
   // cv::imwrite("mask.png", newSrc);

//    cv::findContours(canny, contours, hierarchy, CV_RETR_LIST , CV_CHAIN_APPROX_SIMPLE );

//    std::sort(contours.begin(), contours.end(), compareContourAreas);

//    int count = 0;
//    int i = 0;
//    double lastArea = -1;

//    while(count < 81 && i < contours.size() - 1)
//    {
//        i++;

//        double area = cv::contourArea(contours[i]);

//        if(area > (size/11 * size/11) && area < (size/9 * size/9))
//        {
//          //  if(lastArea == -1 || area - lastArea < lastArea/25)
//            {
//                lastArea = area;
//                count ++;
//                drawContours(mask, contours, i, cv::Scalar(255), CV_FILLED);
//            }
//        }
//    }

//    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));

//    cv::erode(mask, mask, kernel);

//    cv::imwrite("mask.png", mask);

//    cv::bitwise_not(newSrc, newSrc);

//    cv::bitwise_and(newSrc, mask, newSrc);

//    cv::bitwise_not(newSrc, newSrc);

//    cv::vector<cv::Vec4i> lines;
//    cv::HoughLinesP(canny, lines, 1, CV_PI/180, 5, size/2, 5 );

//    for( size_t i = 0; i < lines.size(); i++ )
//    {
//        cv::Vec4i l = lines[i];
//        cv::line( newSrc, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,255,255), 3, CV_AA);
//    }

//    int offset = (int)((float)size/9.0f);

//    for(int i = 0; i < 11; i++)
//    {
//        cv::line( newSrc, cv::Point(i * offset + 5, 0), cv::Point(i * offset + 5, size), cv::Scalar(255,255,255), 10, CV_AA);
//        cv::line( newSrc, cv::Point(0, i * offset + 5), cv::Point(size, i * offset + 5), cv::Scalar(255,255,255), 10, CV_AA);
//    }

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

        puzzle = new Puzzle();

        //file << "Parsed box text...\n";
        while (!boxTextStream.eof())
        {
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




