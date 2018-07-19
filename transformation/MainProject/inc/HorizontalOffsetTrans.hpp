#pragma once

#include "VectorialTrans.hpp"
#include "Quaternion.hpp"

namespace IMT {
class HorizontalOffsetTrans: public VectorialTrans
{
    public:
        HorizontalOffsetTrans(std::shared_ptr<LayoutView> baseLayout, SCALAR amplitude, Quaternion orientation): VectorialTrans(baseLayout), m_amplitude(amplitude), m_orientation(orientation),
            m_xP(orientation.Rotation(Coord3dCart(1,0,0))), m_yP(orientation.Rotation(Coord3dCart(0,1,0))), m_zP(orientation.Rotation(Coord3dCart(0,0,1))) {}

    private:
        SCALAR m_amplitude;
        Quaternion m_orientation;
        Coord3dCart m_xP;
        Coord3dCart m_yP;
        Coord3dCart m_zP;

        virtual Coord3dCart FromBeforeTrans3dToAfterTrans3d(Coord3dCart vectBefore) const override
        {//Apply offset on the horizontal plan
            Coord3dCart verticalComponent = vectBefore.DotProduct(m_zP)*m_zP;
            Coord3dCart v = vectBefore - verticalComponent;
            SCALAR horizontalNorm = v.Norm();
            if (!AlmostEqual(horizontalNorm, 0))
            {
                //Normalize the horizontal component then apply the offset projection on it.
                v /= v.Norm();
                v = v+m_amplitude*m_xP;
                v /= v.Norm();
                //Resize v to the original horizontal size (radius of the circle)
                v *= horizontalNorm;
            }
            //Return the resulting vector:
            return v + verticalComponent;
        }

        virtual Coord3dCart FromAfterTrans3dToBeforeTrans3d(Coord3dCart vectAfter) const override
        {
            Coord3dCart verticalComponent = vectAfter.DotProduct(m_zP)*m_zP;
            Coord3dCart v = vectAfter - verticalComponent;
            SCALAR horizontalNorm = v.Norm();
            if (!AlmostEqual(horizontalNorm, 0))
            {
                //Apply revers offset projection
                v /= v.Norm();
                SCALAR tmp = v.DotProduct(m_amplitude*m_xP);
                v = (tmp + std::sqrt(tmp*tmp-m_amplitude*m_amplitude+1))*v-m_amplitude*m_xP;
                //Resize v to the original radius and return the output vector
                v *= horizontalNorm;
            }
            return v + verticalComponent;
        } 

};
}
