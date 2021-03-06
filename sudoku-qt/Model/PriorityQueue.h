#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H
#include <QMap>
#include <QPair>
#include <QPoint>
#include <QHash>
namespace Model{
    //Struct representing an x and y
    //point.
    /*struct point{
        int x;
        int y;
        point(int X,int Y){
            x = X;
            y = Y;
        }
    };*/
class PriorityQueue {
    private:
        //Elements stored in the pritority queue,
        //with their priorities as keys.
        //QPair<int, QPoint*> *elements;
        QPair<int, int> *elements;
        //maps each element in the priority queue
        //to its index in elements.
        //QHash<QPoint*, int> location;
        QHash<int, int> location;
        //Moves elements[i] upward in the priority
        //queue to an appropriate location.
        void SiftUp(int);
        //Moves elments[i] downward in the priority
        //queue to an appropriate location.
        void SiftDown(int);
        //swaps the contents of two locations in
        //elements.
        void Swap(int, int);
    public:
        //number of elements in the Priority queue
        int count;
        //gets the number of elements in the
        //Priority queue
        int GetCount(void);
        //Gets the maximum priority of any element
        //in the priority queue.
        int MaxPriority(void);
        //Constructor for the Priority Queue
        PriorityQueue(int);
        //adds the given point to the Priority queue,
        //with the given priority.
        //void Add(QPoint*,int);
        void Add(int, int);
        //removes an element with the maximum priority.
        //QPoint* RemoveMax(void);
        int RemoveMax(void);
        //Decreases the priority of the given elements
        //by 1.
        //void DecreasePriority(QPoint*);
        void DecreasePriority(int);
        //Increases the priority of the given elements
        //by 1.
        //void IncreasePriority(QPoint*);
        void IncreasePriority(int);
    };
}
#endif
