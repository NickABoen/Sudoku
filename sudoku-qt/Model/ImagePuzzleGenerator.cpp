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

using namespace Model;

ImagePuzzleGenerator::ImagePuzzleGenerator()
{
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

    cv::GaussianBlur(src, src, cv::Size(5, 5), 1, 0);
    cv::cvtColor(src, src, CV_BGR2GRAY);
    cv::imwrite("greyWithGaussianBlur.png", src);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(30, 30));

    cv::Mat close, div;

    cv::morphologyEx(src, close, cv::MORPH_CLOSE, kernel);

    src.convertTo(src, CV_32F);
    cv::divide(src, close, src, 1, CV_32F);
    cv::normalize(src, src, 0, 255, cv::NORM_MINMAX);
    src.convertTo(src, CV_8UC1);

    cv::imwrite("divideByClose1.png", src);

    cv::Mat canny;

    cv::Canny(src, canny, 50, 200, 3);
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

    cv::vector<cv::Point> approxContour;
    cv::approxPolyDP(contours[index], approxContour, 3, true);

    cv::Point2f srcCorners[4];
    cv::Point2f corners[4];

    int topLeftIndex = 0;
    float distance = 1000000000000; //Horray for magic numbers!

    for(int i = 0; i < approxContour.size(); i++)
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
    distance = 1000000000000; //Horray for magic numbers!

    for(int i = 0; i < approxContour.size(); i++)
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
    distance = 1000000000000; //Horray for magic numbers!

    for(int i = 0; i < approxContour.size(); i++)
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
    distance = 1000000000000; //Horray for magic numbers!

    for(int i = 0; i < approxContour.size(); i++)
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

    int size = src.size().width;
    if(src.size().height < size) size = src.size().height;

    srcCorners[0] = cv::Point(0,    0);
    srcCorners[1] = cv::Point(size, 0);
    srcCorners[2] = cv::Point(0,    size);
    srcCorners[3] = cv::Point(size, size);

    cv::Mat newSrc(size, size, CV_8UC3);

    cv::Mat M( 2, 4, CV_32FC1 );
    M = cv::getPerspectiveTransform(corners, srcCorners);
    cv::warpPerspective(src, newSrc, M, newSrc.size());
    cv::imwrite("warped.png", newSrc);

    cv::adaptiveThreshold(newSrc, newSrc, 255.0, CV_THRESH_BINARY, CV_ADAPTIVE_THRESH_MEAN_C, 11, 3.0f);
    cv::imwrite("afterThreshold.png", newSrc);

//    cv::Canny(newSrc, canny, 100, 100, 3);
//    cv::imwrite("Canny2.png", canny);

//    cv::vector<cv::Vec4i> lines;
//    cv::HoughLinesP(canny, lines, 1, CV_PI/180, 30);

//    for( size_t i = 0; i < lines.size(); i++ )
//    {
//        cv::Vec4i l = lines[i];
//        cv::line( newSrc, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,255,255), 3, CV_AA);
//    }

    int offset = (int)((float)size/9.0f);

    for(int i = 0; i < 11; i++)
    {
        cv::line( newSrc, cv::Point(i * offset + 5, 0), cv::Point(i * offset + 5, size), cv::Scalar(255,255,255), 10, CV_AA);
        cv::line( newSrc, cv::Point(0, i * offset + 5), cv::Point(size, i * offset + 5), cv::Scalar(255,255,255), 10, CV_AA);
    }

    cv::imwrite("Lines.png", newSrc);

    //    api->SetImage
    //    (
    //        (uchar*)newSrc.data,
    //        newSrc.size().width,
    //        newSrc.size().height,
    //        newSrc.channels(),
    //        newSrc.step1()
    //    );

    //    char *boxtext = api->GetBoxText(0);

    //    // Destroy used object and release memory
    //    api->End();

    //    int cellWidth = src.size().width/9;
    //    int cellHeight = src.size().height/9;

    //    std::istringstream boxTextStream(boxtext);

    //    puzzle = new Puzzle();

    //    //file << "Parsed box text...\n";
    //    while (!boxTextStream.eof())
    //    {
    //       int left, right, top, bottom, value, trash;

    //       boxTextStream >> value >> left >> bottom >> right >> top >> trash;

    //       //TODO for some reason it reads in the last line twice...

    //       float x1 = left / cellWidth;
    //       float x2 = right / cellWidth;
    //       float y1 = bottom / cellHeight;
    //       float y2 = top / cellHeight;

    //       if((1 - (x1 - (int)x1)) < (x2 - (int)x2)) x1 = x2;
    //       if((1 - (y1 - (int)y1)) < (y2 - (int)y2)) y1 = y2;
    //       puzzle->defaultBoard[(int)x1][8 - (int)y1] = value;
    //    }

    //    //TODO fix this
    //    int** ConvertedBoard = new int*[9];
    //    for(int i = 0; i < 9; i++){
    //        ConvertedBoard[i] = new int[9];
    //    }
    //    for(int i = 0; i<9;i++){
    //        for(int j = 0; j<9; j++){
    //            int row = 3*(i/3)+j/3;
    //            int column = 3*(i%3)+j%3;
    //            ConvertedBoard[i][j] = puzzle->defaultBoard[row][column];
    //        }
    //    }

    //    BoardGenerator*  boardGenerator = new BoardGenerator();

    //    BoardSolver boardSolver(ConvertedBoard);
    //    puzzle->solvedBoard = boardGenerator->ConvertBoard(boardSolver.Solve());

    return puzzle;
}


