#pragma once

#include "layout.hpp"

namespace IMT {

class LayoutCubeMap: public Layout
{
    public:
        enum class Face {FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, Last, First=FRONT};
        LayoutCubeMap(unsigned int outWidth):
            Layout(outWidth, 2*outWidth/3), m_cubeEdge(outWidth/3) {}

        virtual Coord3dCart from2dTo3d(unsigned int i, unsigned int j) const override;       

        virtual CoordF fromSphereTo2d(double theta, double phi) const override;
        
        Face from2dToFace(unsigned int i, unsigned int j) const;
        Face AnglesToFace(double theta, double phi) const;

        Plan FaceToPlan(Face f) const
        {
           switch (f)
           {
              case Face::FRONT:
                 return Plan(1,0,0,-1);
              case Face::BACK:
                 return Plan(1,0,0,1);
              case Face::TOP:
                 return Plan(0,0,1,1);
              case Face::BOTTOM:
                 return Plan(0,0,1,-1);
              case Face::LEFT:
                 return Plan(0,1,0,-1);
              case Face::RIGHT:
                 return Plan(0,1,0,1);
           }
        }
    private:
        unsigned int m_cubeEdge;

};

}
