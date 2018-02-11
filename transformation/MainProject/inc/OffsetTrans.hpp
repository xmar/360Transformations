#pragma once

#include "Common.hpp"
#include "VectorialTrans.hpp"

namespace IMT {
class OffsetTrans: public VectorialTrans
{
    public:
        OffsetTrans(float vectorOffsetRatio, Coord3dCart b): VectorialTrans(), m_vectorOffsetRatio(vectorOffsetRatio), m_b(std::move(b)) {}
        ~OffsetTrans(void) = default;

        virtual Coord3dCart FromBeforeTrans3dToAfterTrans3d(Coord3dCart vectBefore) override
        {
            Coord3dCart v = vectBefore+m_vectorOffsetRatio*m_b;
            return v/v.Norm();
        }

        virtual Coord3dCart FromAfterTrans3dToBeforeTrans3d(Coord3dCart vectAfter) override
        {
            SCALAR tmp = vectAfter.DotProduct(-m_vectorOffsetRatio*m_b);
            return (-tmp + std::sqrt(tmp*tmp-m_vectorOffsetRatio*m_vectorOffsetRatio+1))*vectAfter-m_vectorOffsetRatio*m_b;
        } 
    private:
        float m_vectorOffsetRatio;
        Coord3dCart m_b;
};
}
