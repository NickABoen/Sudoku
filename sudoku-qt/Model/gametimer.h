#pragma once

#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <QElapsedTimer>
#include <QThread>
#include "Puzzle.h"
#include "mainwindow.h"
namespace Model
{
    class GameTimer: public QThread
    {
    public:
        GameTimer(View::MainWindow*);
        ~GameTimer();
        bool paused;
        View::MainWindow *v;
        Puzzle *puzzle;
        QElapsedTimer *GTimer;
        int runningTime;
        void setPuzzle(Puzzle *p);
        virtual void run();
    };
}
#endif // GAMETIMER_H
