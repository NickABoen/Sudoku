#ifndef __CurrentProgressSerializer_h__
#define __CurrentProgressSerializer_h__

#include <QString>
#include<iostream>
#include<fstream>
#include <QStack>

#include "Puzzle.h"
#include "PuzzleSerializer.h"

namespace Model{

    class CurrentProgressSerializer
    {
    public:
        CurrentProgressSerializer();
        ~CurrentProgressSerializer();

        void serialize(Puzzle* puzzle, QString filePath);
        Puzzle* deserialize(QString filePath, PuzzleSerializer puzzleSerializer);
    };

}

#endif
