#ifndef __MainController_h__
#define __MainController_h__

#include <QObject>
#include <QStack>
#include <QString>

#include "Move.h"
#include "Puzzle.h"
#include "PuzzleSerializer.h"
#include "CurrentProgressSerializer.h"
#include "mainwindow.h"

namespace Controller{

    class MainController : public QObject
    {
        Q_OBJECT

    public:
        MainController();
        ~MainController();

        void displayBoard();
        void displayCurrentBoard();
        void clearBoard();

        Model::Puzzle* puzzle;
        Model::CurrentProgressSerializer currentProgressSerializer;
        Model::PuzzleSerializer puzzleSerializer;
        View::MainWindow view;
        QStack<Model::Move> undo;
        QStack<Model::Move> redo;

    public slots:
        void onLoadProgress();
        void onSaveProgress();
        void onLoadPuzzle();
        void onSavePuzzle();
        void onMakeMove(int *moveArray);
        void onUndoMove();
        void onRedoMove();

    signals:
        void setDefaultMove(int *moveArray);

    };

}

#endif
