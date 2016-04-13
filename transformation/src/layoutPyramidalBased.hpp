#pragma once
#include "layout.hpp"

namespace IMT {
class LayoutPyramidalBased : public Layout
{
    public:
        LayoutPyramidalBased(double baseEdge,double yaw, double pitch, double roll, unsigned int width, unsigned int height):
            Layout(width,height), m_yaw(yaw), m_pitch(pitch), m_roll(roll),
             m_baseEdge(baseEdge), m_alpha(baseEdge/2.0),
             m_canonicTopPlan((1-std::pow(m_alpha,2))/(2*m_alpha), 0, 1, -(1+std::pow(m_alpha,2))/(2*m_alpha)),
             m_top(-m_canonicTopPlan[3]/m_canonicTopPlan[0], 0, 0),
             m_interTop((1-std::pow(m_alpha,2))/(1+std::pow(m_alpha,2)),0,2*m_alpha/(1+std::pow(m_alpha,2))),
             m_pyramidHeight(norm(m_top-Coord3dCart(1,0,0))),
             m_topHeight(norm(m_top-Coord3dCart(1,0,m_alpha))),
             m_intersectionHeight(norm(m_interTop-Coord3dCart(1,0,m_alpha)))
             {}

        enum class Faces {Base, Top, Bottom, Right, Left, Black, Last, First = Base};

        virtual NormalizedFaceInfo From2dToNormalizedFaceInfo(const CoordI& pixel) const = 0;
        virtual CoordF FromNormalizedInfoTo2d(const NormalizedFaceInfo& ni) const = 0;
        //Return coordinate value between [0;1] for each face
        virtual NormalizedFaceInfo From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const final;
        //Expect coordinate value to be in the [0;1] range for each face
        virtual Coord3dCart FromNormalizedInfoTo3d(const NormalizedFaceInfo& ni) const final;

        Plan FaceToPlan(Faces f) const // return the (a, b, c, d) vector corresponding to the equation of the plan contening the face f: a.x+b.y+c.z+d=0 for the canonical pyramid
        {
            switch (f)
            {
            case Faces::Base:
                return Plan(1,0,0,-1);
            case Faces::Top:
                return Plan( m_canonicTopPlan[0], m_canonicTopPlan[1], m_canonicTopPlan[2], m_canonicTopPlan[3]);
            case Faces::Left:
                return Plan( m_canonicTopPlan[0], -m_canonicTopPlan[2], m_canonicTopPlan[1], m_canonicTopPlan[3]);
            case Faces::Bottom:
                return Plan( m_canonicTopPlan[0], -m_canonicTopPlan[1], -m_canonicTopPlan[2], m_canonicTopPlan[3]);
            case Faces::Right:
                return Plan( m_canonicTopPlan[0], m_canonicTopPlan[2], -m_canonicTopPlan[1], m_canonicTopPlan[3]);
            case Faces::Black:
                return Plan(0,0,0,0);
            case Faces::Last:
                throw std::invalid_argument("FaceToPlan: Last is not a valid face");
            }
        }
    protected:

    private:
      double m_yaw;
      double m_pitch;
      double m_roll;
      double m_baseEdge;
      double m_alpha;
      Plan m_canonicTopPlan;// Ton plan in the canonic pyramid
      Coord3dCart m_top; //coordinate of the top of the pyramid
      Coord3dCart m_interTop; //cartesian coordinate of the intersection between the top plan and the unit sphere
      double m_pyramidHeight; //Height of the pyramid (base to top)
      double m_topHeight; //Height of the top face (from (1, 0, m_alpha) to m_top
      double m_intersectionHeight; //Distance between (1, 0, m_alpha) and m_interTop

      double UsePlanEquation(double x) const; //compute the value of z knowing the value of x (for the top plan in the canonic pyramid)
};
}
