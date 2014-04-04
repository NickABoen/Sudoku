#ifndef IMAGEPUZZLEGENERATOR_H
#define IMAGEPUZZLEGENERATOR_H

#include <QString>
//OpenCV includes
#include "core.hpp"

#include "Puzzle.h"

namespace Model{

    class ImagePuzzleGenerator
    {
    public:

        ImagePuzzleGenerator();

        Puzzle* generate(QString filePath);

    };

}

#endif // IMAGEPUZZLEGENERATOR_H
