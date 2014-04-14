#ifndef __MainController_h__
#define __MainController_h__

#include <QObject>
#include <QStack>
#include <QString>
#include <QElapsedTimer>
#include <QThread>

#include "Move.h"
#include "Puzzle.h"
#include "PuzzleSerializer.h"
#include "CurrentProgressSerializer.h"
#include "mainwindow.h"
#include "difficultyselector.h"
#include "gametimer.h"

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
        void onEnableValidation();
        void onClues();
        void giveClues();
        void endThread();

    signals:
        void setDefaultMove(int *moveArray);

    private:
        void displayDefaultBoard();
        void displayCurrentBoard();
        bool clueTimer;
        bool validationEnabled;
        QTimer *ctimer;
        Model::Puzzle* puzzle;
        Model::CurrentProgressSerializer currentProgressSerializer;
        Model::PuzzleSerializer puzzleSerializer;
        View::MainWindow view;
        void storeFilePath();
        bool validateBoard(int value, int x, int y);
        Model::GameTimer *timerThread;
    };

}

#endif
