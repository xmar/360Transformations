#pragma once

#include "layout.hpp"

namespace IMT {

class LayoutCubeMap: public Layout
{
    public:
        enum class Face {FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, Last, First=FRONT};
        LayoutCubeMap(unsigned int outWidth):
            Layout(outWidth, 2*outWidth/3), m_cubeEdge(outWidth/3) {}

        virtual cv::Point3f from2dTo3d(unsigned int i, unsigned int j) const override;       
        
        Face from2dToFace(unsigned int i, unsigned int j) const;
    private:
        unsigned int m_cubeEdge;

};

}
