#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
#include "Move.h"
#include <QMap>
#include <QPair>
using namespace std;
class PriorityQueue{
private:
    QPair<qint, Move> *elements;
    int count;
    QMap<Move, qint> location;
    void SiftUp(qint);
    void SiftDown(qint);
    void Swap(qint,qint);
public:
    int GetCount(void);
    int MaxPriority(void);
    PriorityQueue(qint);
    void Add(Move,qint);
    Move RemoveMax(void);
    void DecreasePriority(Move);
    void IncreasePriority(Move);
};

#endif // PRIORITYQUEUE_H
