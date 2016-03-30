#pragma once

#include "layout.hpp"

namespace IMT {

class LayoutPyramidal: public Layout
{
   public:
      LayoutPyramidal(double baseEdge,double yaw, double pitch, double roll, unsigned width, unsigned int height): Layout(width,height), m_yaw(yaw), m_pitch(pitch), m_roll(roll), m_baseEdge(baseEdge), m_alpha(baseEdge/2.0) {}
      virtual ~LayoutPyramidal(void) = default;


      enum class Face {Base, Top, Bottom, Right, Left, Last, First = Base};

      virtual Coord3dCart from2dTo3d(unsigned int i, unsigned int j) const override;
      virtual CoordF fromSphereTo2d(double theta, double phi) const override;

      //Face AngleToFace(double theta, double phi) const { AngleToFace(Coord3dSpherical(1,theta,phi));}
      //Face AngleToFace(const Coord3dSpherical& p) const {return AngleToFaceCart(SphericalToCart(p));}
      //Face AngleToFaceCart(const Coord3dCart& p) const;
      cv::Vec4d FaceToPlan(Face f) const // return the (a, b, c, d) vector corresponding to the equation of the plan contening the face f: a.x+b.y+c.z+d=0
      {
         switch (f)
         {
            case Face::Base:
               return cv::Vec4d(1,0,0,-1);
            case Face::Top:
               return cv::Vec4d( (std::pow(m_alpha, 2) - 1)/(2*m_alpha), 0, 1, (-std::pow(m_alpha, 4)-4*std::pow(m_alpha, 2) + 1)/(2*m_alpha*(std::pow(m_alpha, 2) + 1)));
            case Face::Left:
               return cv::Vec4d( (std::pow(m_alpha, 2) - 1)/(2*m_alpha), 1, 0, (-std::pow(m_alpha, 4)-4*std::pow(m_alpha, 2) + 1)/(2*m_alpha*(std::pow(m_alpha, 2) + 1)));
            case Face::Bottom:
               return cv::Vec4d( (std::pow(m_alpha, 2) - 1)/(2*m_alpha), 0, -1, (-std::pow(m_alpha, 4)-4*std::pow(m_alpha, 2) + 1)/(2*m_alpha*(std::pow(m_alpha, 2) + 1)));
            case Face::Right:
               return cv::Vec4d( (std::pow(m_alpha, 2) - 1)/(2*m_alpha), -1, 0, (-std::pow(m_alpha, 4)-4*std::pow(m_alpha, 2) + 1)/(2*m_alpha*(std::pow(m_alpha, 2) + 1)));
         }
      }
   private:
      double m_yaw;
      double m_pitch;
      double m_roll;
      double m_baseEdge;
      double m_alpha;
};

}
