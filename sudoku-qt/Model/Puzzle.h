#ifndef __Puzzle_h__
#define __Puzzle_h__

#include <QObject>
#include <QStack>
#include <QList>
#include <QPair>

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
        int currentTime;

        QStack<Model::Move> undo;
        QStack<Model::Move> redo;
        QList<QPair<QString,int>> scoreboardList;

    };

}

#endif
