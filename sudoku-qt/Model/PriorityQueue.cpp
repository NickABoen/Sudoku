#include "PriorityQueue.h"
#include <iostream>
#include <string>
#include <QMap>
#include <QPair>
#include <limits.h>

namespace Model{
    //Moves elements[i] upward in the priority
    //queue to an appropriate location.
    void PriorityQueue::SiftUp(int child){
        int parent = child/2;
        if(elements[child].first >  elements[parent].first) {
            Swap(child,parent);
            child = parent;
            SiftUp(child);
        }
    }
    //Moves elments[i] downward in the priority
    //queue to an appropriate location.
    void PriorityQueue::SiftDown(int parent){
        int child = parent*2;
        if(parent*2 <= count && elements[parent].first < elements[child].first){
            if(child < count && elements[child].first < elements[child+1].first){
                child++;
            }
            Swap(parent, child);
            parent = child;
            SiftDown(parent);
        }
    }
    //swaps the contents of two locations in
    //elements.
    void PriorityQueue::Swap(int x, int y){
        QPair<int, QPoint*> temp = elements[x];
        elements[x] = elements[y];
        elements[y] = temp;
        location[elements[x].second] = x;
        location[elements[y].second] = y;
    }
    //gets the number of elements in the
    //Priority queue
    int PriorityQueue::GetCount(){
            return count;
    }
    //Gets the maximum priority of any element
    //in the priority queue.
    int PriorityQueue::MaxPriority(){
        return elements[1].first;
    }
    //Constructor for the Priority Queue
    PriorityQueue::PriorityQueue(int cap){
        int Max = UINT_MAX;
        elements = new QPair<int, QPoint*>[cap+1];
        //Moves defMove(NULL,NULL,NULL);
        QPoint* pt = new QPoint(-1,-1);
        //pair<int, Moves> defPair(Max, defMove);
        QPair<int, QPoint*> defPair(Max, pt);
        elements[0] = defPair;
    }
    //adds the given point to the Priority queue,
    //with the given priority.
    void PriorityQueue::Add(QPoint* M, int i){
        QPair<int, QPoint*> nPair(i,M);
        elements[count+1] = nPair;
        location[M] = count+1;
        SiftDown(++count);
    }
    //removes an element with the maximum priority.
    QPoint* PriorityQueue::RemoveMax(){
        QPoint* M = elements[1].second;
        Swap(1, count--);
        location.remove(M);
        SiftDown(1);
        return M;
    }
    //Decreases the priority of the given elements
    //by 1.
    void PriorityQueue::DecreasePriority(QPoint* m){
        int loc = location[m];
        elements[loc] = *new QPair<int, QPoint*>(elements[loc].first-1,m);
        SiftDown(loc);
    }
    //Increases the priority of the given elements
    //by 1.
    void PriorityQueue::IncreasePriority(QPoint* m){
        int loc = location[m];
        elements[loc] = *new QPair<int, QPoint*>(elements[loc].first+1,m);
        SiftUp(loc);
    }
}
