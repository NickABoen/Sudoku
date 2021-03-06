#ifndef __PuzzleSerializer_h__
#define __PuzzleSerializer_h__

#include<QString>
#include<iostream>
#include<fstream>

#include"Puzzle.h"

namespace Model{

    class PuzzleSerializer
    {
    public:
        PuzzleSerializer();
        ~PuzzleSerializer();

        void serialize(Puzzle *puzzle, QString filePath);
        Puzzle* deserialize(QString filePath);
    };

}

#endif
