#pragma once
#include "layout.hpp"
#include <stdexcept>

namespace IMT {

class LayoutRhombicdodeca: public Layout
{
    public:
        LayoutRhombicdodeca(unsigned int height): Layout(6*height, 2*height), m_faceHeight(height) {}
        ~LayoutRhombicdodeca(void) = default;

        enum class Face{ Face1, Face2, Face3, Face4, Face5, Face6, Face7, Face8, Face9, Face10, Face11, Face12, Last, First=Face1 };

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const override;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const override;
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const override;
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const override;

        Plan FaceToPlan(Face f) const
        {
            switch(f)
            {
                case Face::Face1:
                    return Plan(-1,0,0,1);
                case Face::Face2:
                    return Plan(-1,-std::sqrt(2),-1,2);
                case Face::Face3:
                    return Plan(-1,-std::sqrt(2),1,2);
                case Face::Face4:
                    return Plan(-1,std::sqrt(2),-1,2);
                case Face::Face5:
                    return Plan(-1,std::sqrt(2),1,2);
                case Face::Face6:
                    return Plan(0,0,1,1);
                case Face::Face7:
                    return Plan(1,std::sqrt(2),1,2);
                case Face::Face8:
                    return Plan(1,-std::sqrt(2),1,2);
                case Face::Face9:
                    return Plan(1,-std::sqrt(2),-1,2);
                case Face::Face10:
                    return Plan(1,std::sqrt(2),-1,2);
                case Face::Face11:
                    return Plan(0,0,-1,1);
                case Face::Face12:
                    return Plan(1,0,0,1);
                case Face::Last:
                    throw std::invalid_argument("FaceToPlan: Last is not a valid face");
            }
        }

        RotMat FaceToRotMat(Face f) const; //Rot matrix to transform Face1 into f

        Face LayoutToFace(unsigned int i, unsigned int j) const;

        CoordF CanonicLayoutCoordToLayoutCoord(double i, double j, Face f) const;

    private:
        unsigned int m_faceHeight;
};

}
