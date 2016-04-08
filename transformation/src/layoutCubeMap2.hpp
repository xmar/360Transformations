#pragma once

#include "layout.hpp"

namespace IMT {

class LayoutCubeMap2: public Layout
{
	public:
		enum class Face {FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, BLACK, Last, First=FRONT};
		LayoutCubeMap2(unsigned int outWidth):
		            Layout(outWidth, 3*outWidth/4), m_cubeEdge(outWidth/4) {}

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
				 return Plan(0,1,0,1);
			  case Face::RIGHT:
				 return Plan(0,1,0,-1);
			  case Face::BLACK:
				  return Plan(0,0,0,0);
		   }
		}

		private:
			unsigned int m_cubeEdge;
};

}
