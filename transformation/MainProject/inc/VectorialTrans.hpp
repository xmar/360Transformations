#pragma once

#include "Common.hpp"

namespace IMT {
class VectorialTrans
{
    public:
        VectorialTrans(void) {};
        virtual ~VectorialTrans(void) = default;

        //FromBeforeTrans3dToAfterTrans3d From 2D to 3D
        virtual Coord3dCart FromBeforeTrans3dToAfterTrans3d(Coord3dCart vectBefore)
        {//default transformation do nothing
            return std::move(vectBefore);
        }
        //FromAfterTrans3dToBeforeTrans3d From 3D to 2D
        virtual Coord3dCart FromAfterTrans3dToBeforeTrans3d(Coord3dCart vectAfter)
        {//default transformation do nothing
            return std::move(vectAfter);
        }

    private:
};
}
