#include "PriorityQueue.h"
#include <iostream>
#include <string>
#include <QMap>
#include <QPair>
#include <limits.h>
#include <QtDebug>
using namespace Model;

//////////////////////////////////////////////////////////////////////////////////
// Constructor for PriorityQueue
//////////////////////////////////////////////////////////////////////////////////
PriorityQueue::PriorityQueue(int cap){
    int Max = INT_MAX;
    this->count = 0;
    elements = new QPair<int, int>[cap+1];
    elements[0] = QPair<int, int>(Max, 999);
}

//////////////////////////////////////////////////////////////////////////////////
// This function moves elements[i] upward in the priority queue to an appropriate
// location.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::SiftUp(int child){
    while(elements[child].first >  elements[child/2].first) {
        Swap(child,child/2);
        child = child/2;
        //SiftUp(child);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function moves elements[i] downward in the priority queue to an appropriate
// location.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::SiftDown(int parent){
    while(2*parent <= count){
        int child = parent*2;
        if(child<count&& elements[child].first < elements[child+1].first) child++;
        if(elements[parent].first >= elements[child].first) break;
        Swap(parent,child);
        parent = child;

    }
}

//////////////////////////////////////////////////////////////////////////////////
// This function swaps the contents of two location in elements.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::Swap(int x, int y){
    QPair<int, int> temp = elements[x];
    elements[x] = elements[y];
    elements[y] = temp;
    location[elements[x].second] = x;
    location[elements[y].second] = y;
}

//////////////////////////////////////////////////////////////////////////////////
// This function gets the number of elements in the Priority queue.
//////////////////////////////////////////////////////////////////////////////////
int PriorityQueue::GetCount(){
        return count;
}

//////////////////////////////////////////////////////////////////////////////////
// This function gets the maximum priority of any element in the priority queue.
//////////////////////////////////////////////////////////////////////////////////
int PriorityQueue::MaxPriority(){
    return elements[1].first;
}

//////////////////////////////////////////////////////////////////////////////////
// This function adds the given point to the Priority queue, with the given
// priority.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::Add(int M, int i){
    QPair<int, int> nPair(i,M);
    elements[count+1] = nPair;
    location.insert(M,count+1);
    //location[M] = count+1;
    SiftUp(++count);
}

//////////////////////////////////////////////////////////////////////////////////
// This function removes an element with the maximum priority.
//////////////////////////////////////////////////////////////////////////////////
int PriorityQueue::RemoveMax(){
    int M = elements[1].second;
    Swap(1, count--);
    location.remove(M);
    SiftDown(1);
    return M;
}

//////////////////////////////////////////////////////////////////////////////////
// This function decreases the priority of the given elements by 1.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::DecreasePriority(int m){
    int loc = location[m];
    elements[loc] = *new QPair<int, int>(elements[loc].first-1, m);
    SiftDown(loc);
}

//////////////////////////////////////////////////////////////////////////////////
// This function increases the priority of the given elements by 1.
//////////////////////////////////////////////////////////////////////////////////
void PriorityQueue::IncreasePriority(int m){
    int loc = location[m];
    elements[loc] = *new QPair<int, int>(elements[loc].first+1, m);
    SiftUp(loc);
}
