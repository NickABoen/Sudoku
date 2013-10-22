#ifndef __PuzzleSerializer_h__
#define __PuzzleSerializer_h__

#include<QString>

#include"Puzzle.h"

namespace Model{

    class PuzzleSerializer
    {
    public:
        PuzzleSerializer();
        ~PuzzleSerializer();
        void serialize(Puzzle &puzzle, QString filePath);
        void deserialize(Puzzle *puzzle, QString filePath);
    };

}

#endif
