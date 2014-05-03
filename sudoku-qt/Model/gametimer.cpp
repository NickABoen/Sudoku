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
    currentTime=0;
}

GameTimer::~GameTimer()
{
    delete(GTimer);
}
void GameTimer::resetTimer()
{
    runningTime = 0;
    GTimer->restart();
}

void GameTimer::setPuzzle(Puzzle *p)
{
    puzzle = p;
}

void GameTimer::run()
{
    GTimer->start();
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
            currentTime = GTimer->elapsed()+runningTime;
            puzzle->currentTime = currentTime;
            Seconds = (int)(currentTime/1000)%60;
            currentTime -= Seconds*1000;
            Minutes = (int)(currentTime/(1000*60))%60;
            currentTime -= Minutes*1000*60;
            Hours = (int)(currentTime/(1000*60*60))%24;
            sec = QString("%1").arg(Seconds).rightJustified(sec.indexOf('.')+3,'0');
            min = QString("%1").arg(Minutes).rightJustified(sec.indexOf('.')+3,'0');
            Hr =  QString("%1").arg(Hours).rightJustified(sec.indexOf('.')+3,'0');
            v->TIMER->setText(Hr+":"+min+":"+sec);
            this->msleep(100);
        }
        else runningTime = puzzle->currentTime;
    }

}
