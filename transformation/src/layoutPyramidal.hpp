#pragma once

#include "layout.hpp"

namespace IMT {

class LayoutPyramidal: public Layout
{
   public:
      LayoutPyramidal(double baseEdge,double yaw, double pitch, double roll, unsigned int height):
         Layout(3*height,height), m_yaw(yaw), m_pitch(pitch), m_roll(roll),
         m_baseEdge(baseEdge), m_alpha(baseEdge/2.0),
         m_canonicTopPlan((1-std::pow(m_alpha,2))/(2*m_alpha), 0, 1, -(1+std::pow(m_alpha,2))/(2*m_alpha)),
         m_top(m_canonicTopPlan[3]/m_canonicTopPlan[0], 0, 0), m_pyramidHeight(cv::norm(m_top-Coord3dCart(1,0,0))) {}
      virtual ~LayoutPyramidal(void) = default;


      enum class Face {Base, Top, Bottom, Right, Left, Last, First = Base};

      virtual Coord3dCart from2dTo3d(unsigned int i, unsigned int j) const override;
      virtual CoordF fromSphereTo2d(double theta, double phi) const override;

      //Face AngleToFace(double theta, double phi) const { AngleToFace(Coord3dSpherical(1,theta,phi));}
      //Face AngleToFace(const Coord3dSpherical& p) const {return AngleToFaceCart(SphericalToCart(p));}
      //Face AngleToFaceCart(const Coord3dCart& p) const;
      Plan FaceToPlan(Face f) const // return the (a, b, c, d) vector corresponding to the equation of the plan contening the face f: a.x+b.y+c.z+d=0 for the canonical pyramid
      {
         switch (f)
         {
            case Face::Base:
               return Plan(1,0,0,-1);
            case Face::Top:
               return Plan( m_canonicTopPlan[0], m_canonicTopPlan[1], m_canonicTopPlan[2], m_canonicTopPlan[3]);
            case Face::Left:
               return Plan( m_canonicTopPlan[0], m_canonicTopPlan[2], -m_canonicTopPlan[1], m_canonicTopPlan[3]);
            case Face::Bottom:
               return Plan( m_canonicTopPlan[0], -m_canonicTopPlan[1], -m_canonicTopPlan[2], m_canonicTopPlan[3]);
            case Face::Right:
               return Plan( m_canonicTopPlan[0], -m_canonicTopPlan[2], m_canonicTopPlan[1], m_canonicTopPlan[3]);
         }
      }
   private:
      double m_yaw;
      double m_pitch;
      double m_roll;
      double m_baseEdge;
      double m_alpha;
      Plan m_canonicTopPlan;// Ton plan in the canonic pyramid
      Coord3dCart m_top; //coordinate of the top of the pyramid
      double m_pyramidHeight; //Height of the pyramid (base to top)

      double UsePlanEquation(double x) const; //compute the value of z knowing the value of x (for the top plan in the canonic pyramid)
};

}
