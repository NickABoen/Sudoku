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
#include "difficultyselector.h"

namespace Controller{

    class MainController : public QObject
    {
        Q_OBJECT

    public:
        MainController();
        ~MainController();

    public slots:
        void onLoadProgress();
        void onSaveProgress();
        void onLoadPuzzle();
        void onSavePuzzle();
        void onMakeMove(int *moveArray);
        void onUndoMove();
        void onRedoMove();
        void onGenerateBoard();
        void onClear();
        void onHint();
        void onEnableNotes();
        void onClues();
        void giveClues();

    signals:
        void setDefaultMove(int *moveArray);

    private:
        void displayDefaultBoard();
        void displayCurrentBoard();
        bool clueTimer;
        QTimer *timer;
        Model::Puzzle* puzzle;
        Model::CurrentProgressSerializer currentProgressSerializer;
        Model::PuzzleSerializer puzzleSerializer;
        View::MainWindow view;
    };

}

#endif
