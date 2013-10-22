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

        Model::Puzzle puzzle;
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
        void onMakeMove(QString text); //Replace with tuple or something later
        void onUndoMove();
        void onRedoMove();

    };

}

#endif
