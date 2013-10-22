#ifndef __CurrentProgressSerializer_h__
#define __CurrentProgressSerializer_h__

#include <QString>

#include "Puzzle.h"

namespace Model{

    class CurrentProgressSerializer
    {
    public:
        CurrentProgressSerializer();
        ~CurrentProgressSerializer();
        void serialize(Puzzle& puzzle, QString filePath);
        void deserialize(Puzzle* puzzle, QString filePath);
    };

}

#endif
