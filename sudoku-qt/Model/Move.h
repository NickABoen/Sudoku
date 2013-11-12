#ifndef __Move_h__
#define __Move_h__

namespace Model{

    class Move
    {
    public:
        Move();
        Move(int x, int y, int value);
        ~Move();

        int x;
        int y;
        int value;
    };

}

#endif
