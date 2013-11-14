#ifndef __Puzzle_h__
#define __Puzzle_h__

#include <QObject>
#include <QStack>

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


        QStack<Model::Move> undo;
        QStack<Model::Move> redo;
    };

}

#endif
