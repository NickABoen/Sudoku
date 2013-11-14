#ifndef __Puzzle_h__
#define __Puzzle_h__

#include <QObject>

#include "Move.h"

namespace Model{

    class Puzzle: public QObject
    {
        Q_OBJECT

    public:
        Puzzle();
        ~Puzzle();
        bool checkCompleted();
        void makeMove(Move move);

        QString filePathRef;
        int** currentBoard;
        int** defaultBoard;
        int** solvedBoard;
    };

}

#endif
