#ifndef __CurrentProgressSerializer_h__
#define __CurrentProgressSerializer_h__

#include <QString>
#include<iostream>
#include<fstream>
#include <QStack>

#include "Puzzle.h"

namespace Model{

    class CurrentProgressSerializer
    {
    public:
        CurrentProgressSerializer();
        ~CurrentProgressSerializer();
        void serialize(QStack<Model::Move> undo, Puzzle* puzzle, QString filePath);
        void deserialize(QStack<Model::Move> undo, Puzzle* puzzle, QString filePath);
    };

}

#endif
