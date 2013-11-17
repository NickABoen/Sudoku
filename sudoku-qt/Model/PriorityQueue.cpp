#include "PriorityQueue.h"
#include <iostream>
#include <string>
#include <map>
#include <limits.h>
using namespace std;
void PriorityQueue::SiftUp(int child){
    int parent = child/2;
    if(elements[child].first > elements[parent].first){
        Swap(child,parent);
        child = parent;
        SiftUp(child);
    }
}
void PriorityQueue::SiftDown(int parent){
    int child = parent*2;
    if(parent*2 <= count && elements[parent].first < elements[child].first){
        if(child < count && elements[child].first < elements[child+1].first){
            child++;
        }
        Swap(parent,child);
        parent = child;
        SiftDown(parent);
    }
}
void PriorityQueue::Swap(int x, int y){
    QPair<int, Move> temp = elements[x];
    elements[x] = elements[y];
    elements[y] = temp;
    location[elements[x].second] = x;
    location[elements[y].second] = y;
}

int PriorityQueue::GetCount(){
    return count;
}
int PriorityQueue::MaxPriority(){
    return elements[1].first;
}
PriorityQueue::PriorityQueue(int cap){
    int Max = UINT_MAX;
    elements = new QPair<int, Move>[cap+1];
    Move defMove(NULL,NULL,NULL);
    QPair<int, Move> defPair(Max, defMove);
    elements[0] = defPair;
}
void PriorityQueue::Add(Move M, int i){
    QPair<int,Move> nPair(i,M);
    elements[count+1] = nPair;
    loaction[M] = count+1;
    SiftDown(++count);
}
Move PriorityQueue::RemoveMax(){
    Move M = elments[1].second;
    Swap(1,count--);
    location.erase(M);
    SiftDown(1);
    return M;
}
void PriorityQueue::DecreasePriority(Moves m){
    int loc = location[m];
    elements[loc] = *new QPair<int, Move>(elements[loc].first+1, m);
    SiftUp(loc);
}
