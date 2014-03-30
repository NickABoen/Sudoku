#include "gametimer.h"
#include "MainController.h"
#include "mainwindow.h"
#include "Puzzle.h"
#include "qdebug.h"
#include <QDebug>
#include <QException>

using namespace Model;
GameTimer::GameTimer(View::MainWindow *mw)
{
    paused = true;
    GTimer = new QElapsedTimer();
    runningTime = 0;
    v = mw;
}

GameTimer::~GameTimer()
{
    delete(GTimer);
}

void GameTimer::setPuzzle(Puzzle *p)
{
    puzzle = p;
}

void GameTimer::run()
{
    GTimer->start();
    int milli = 0;
    int Hours;
    int Minutes;
    int Seconds;
    QString sec;
    QString min;
    QString Hr;
    runningTime += puzzle->currentTime;
    while(true)
    {
        if(!paused)
        {
            milli = GTimer->elapsed()+runningTime;
            puzzle->currentTime = milli;
            Seconds = (int)(milli/1000)%60;
            milli -= Seconds*1000;
            Minutes = (int)(milli/(1000*60))%60;
            milli -= Minutes*1000*60;
            Hours = (int)(milli/(1000*60*60))%24;
            sec = QString("%1").arg(Seconds).rightJustified(sec.indexOf('.')+3,'0');
            min = QString("%1").arg(Minutes).rightJustified(sec.indexOf('.')+3,'0');
            Hr =  QString("%1").arg(Hours).rightJustified(sec.indexOf('.')+3,'0');
            v->TIMER->setText(Hr+":"+min+":"+sec);
            this->msleep(100);
        }
        else runningTime = puzzle->currentTime;
    }

}
