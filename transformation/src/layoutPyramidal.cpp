#include "layoutPyramidal.hpp"
#include <limits>

using namespace IMT;

/*          3*h
 *   +-----+-----+-----+
 *   |    /|     |\    |
 *   |  /  |     |  \  |
 *   |/  L |  Ba |  R \|  h
 *   |\    |     |    /|
 *   |  \  |     |  /  |
 *   | T  \|     |/  Bo|
 *   +-----+-----+-----+
 */

double LayoutPyramidal::UsePlanEquation(double x) const //Use the plan equation to compute the second (non nul) variable knowing the value of x
{
    return -m_canonicTopPlan[3] - x * m_canonicTopPlan[0]; 
}

Coord3dCart LayoutPyramidal::from2dTo3d(unsigned int i, unsigned int j) const
{
    if (inInterval(i, m_outHeight+1, 2*m_outHeight))
    {//then we are on the base
        i -= m_outHeight;
        Coord3dCart inter(1, (double(i)/m_outHeight-0.5)*m_baseEdge, (double(j)/m_outHeight-0.5)*m_baseEdge);
        return Rotation(inter, m_yaw, m_pitch, m_roll);
    }
    if (inInterval(i, 0, m_outHeight))
    {//then top or left
        double normalizedI = double(i)/m_outHeight; // from 0 to 1
        double normalizedJ = double(j)/m_outHeight-0.5; //from -0.5 to 0.5
        if (inInterval(normalizedJ, -0.5*normalizedI, 0.5*normalizedI))
        {//Left face
            //normalizedJ /= 0.5*normalizedI;  
            normalizedI = 1.0-normalizedI;
            double z = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_topHeight);
            double y = -UsePlanEquation(x);
            //normalizedI = 1.0-normalizedI;
            //if(i == m_outHeight/4 && normalizedJ==0.5*normalizedI)//&& j == m_outHeight/2)
            //{
            //   std::cout << "Pyramid Height: "<<m_pyramidHeight << "; alpha= " <<m_alpha << " (a,b,c,d)= " << m_canonicTopPlan[0] <<", "<<m_canonicTopPlan[1]<<", "<<m_canonicTopPlan[2]<<", "<<m_canonicTopPlan[3] << "; Top.x = " << m_top.x << std::endl;
            //   std::cout << "Left: (i,j)=" <<i <<"; "<<j<<"; (normI,normJ)="<<normalizedI<<"; "<<normalizedJ <<";(x,y,z) = "<<x<<"; "<<y<<"; "<<z<<std::endl;
            //}
            //if(i == m_outHeight && j==0)//&& j == m_outHeight/2)
            //{
            //   std::cout << "Pyramid Height: "<<m_pyramidHeight << "; alpha= " <<m_alpha << " (a,b,c,d)= " << m_canonicTopPlan[0] <<", "<<m_canonicTopPlan[1]<<", "<<m_canonicTopPlan[2]<<", "<<m_canonicTopPlan[3] << "; Top.x = " << m_top.x << std::endl;
            //   std::cout << "Left: (i,j)=" <<i <<"; "<<j<<"; (normI,normJ)="<<normalizedI<<"; "<<normalizedJ <<";(x,y,z) = "<<x<<"; "<<y<<"; "<<z<<std::endl;
            //}

            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
        else
        {//top face
            normalizedJ = std::fmod(normalizedJ + 1,1.0)-0.5;

            double y = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double z = -UsePlanEquation(x);
            //return Coord3dCart(0,1,1);
            //if(i == m_outHeight/2 && j == m_outHeight-2)//m_outHeight/2)
            //{
            //    std::cout << "TOP" << std::endl;
            //   std::cout << "Pyramid Height: "<<m_pyramidHeight << "; alpha= " <<m_alpha << " (a,b,c,d)= " << m_canonicTopPlan[0] <<", "<<m_canonicTopPlan[1]<<", "<<m_canonicTopPlan[2]<<", "<<m_canonicTopPlan[3] << std::endl;
            //   std::cout << "Left: (i,j)=" <<i <<"; "<<j<<"; (normI,normJ)="<<normalizedI<<"; "<<normalizedJ <<";(x,y,z) = "<<x<<"; "<<y<<"; "<<z<<std::endl;
            //}
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    }
    else
    {//then right or bottom
        double normalizedI = double(i-2*m_outHeight)/m_outHeight; // from 0 to 1
        double normalizedJ = double(j)/m_outHeight-0.5; //from -0.5 to 0.5
        if (inInterval(normalizedJ, -0.5+0.5*normalizedI, 0.5-0.5*normalizedI))
        {//Right face
            //normalizedI = -normalizedI;
            //normalizedJ /= (1.0-normalizedI);  
            double z = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double y = UsePlanEquation(x);
            //return Coord3dCart(0,0,-1);
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
        else
        {//bottom face
            normalizedJ = (std::fmod(normalizedJ + 1,1.0)-0.5);
            normalizedI = 1.0-normalizedI;
            double y = normalizedJ * m_baseEdge;
            double x = (1.0-normalizedI*m_pyramidHeight);
            double z = UsePlanEquation(x);
            //return Coord3dCart(0,0,1);
            //if(i == 2*m_outHeight+m_outHeight/2 && j == m_outHeight-1)//m_outHeight/2)
            //{
            //    std::cout << "BOTTOM" << std::endl;
            //   std::cout << "Pyramid Height: "<<m_pyramidHeight << "; alpha= " <<m_alpha << " (a,b,c,d)= " << m_canonicTopPlan[0] <<", "<<m_canonicTopPlan[1]<<", "<<m_canonicTopPlan[2]<<", "<<m_canonicTopPlan[3] << std::endl;
            //   std::cout << "Left: (i,j)=" <<i <<"; "<<j<<"; (normI,normJ)="<<normalizedI<<"; "<<normalizedJ <<";(x,y,z) = "<<x<<"; "<<y<<"; "<<z<<std::endl;
            //}
            return Rotation(Coord3dCart(x,y,z), m_yaw, m_pitch, m_roll);
        }
    }
}

CoordF LayoutPyramidal::fromSphereTo2d(double theta, double phi) const
{
    auto f = Face::Base;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();


    //Transform to the canonic representation
    Coord3dSpherical p = Rotation(Coord3dSpherical(1, theta, phi), -m_yaw, -m_pitch, -m_roll);  

    for (auto testF: get_range<LayoutPyramidal::Face>())
    {
        try {
            auto plan = FaceToPlan(testF);
            auto interSphe = IntersectionPlanSpherical(plan, p); //raise exception if no intersection
            if (minRho > interSphe.x && interSphe.y == p.y) //check direction
            {
                minRho = interSphe.x;
                inter = SphericalToCart(interSphe);
                f = testF;
            }
        } catch ( std::logic_error& le )
        { //no intersection with this face
            continue;
        }
    }
    //TODO do the rotation first ...
    //Now inter is the right intersection and f is the right face
    switch (f)
    {
        case Face::Base:
            return CoordF((inter.y/m_baseEdge+0.5)*m_outHeight+m_outHeight, (inter.z/m_baseEdge+0.5)*m_outHeight);
        case Face::Left:
            {
                double normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight; 
                return CoordF( normalizedI+2*m_outHeight, (inter.z/m_baseEdge+0.5)*m_outHeight);
            }
        case Face::Right:
            {
                double normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight;
                return CoordF( m_outHeight-normalizedI, (inter.z/m_baseEdge+0.5)*m_outHeight);
            }
        case Face::Top:
            {
                double normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight;
                return CoordF( normalizedI, std::fmod((inter.y/m_baseEdge+0.5)*m_outHeight+m_outHeight,m_outHeight));
            }
        case Face::Bottom:
            {
                double normalizedI = m_outHeight*(1.0 - inter.x)/m_pyramidHeight;
                return CoordF( (m_outHeight-normalizedI)+2*m_outHeight, std::fmod((inter.y/m_baseEdge+0.5)*m_outHeight+m_outHeight,m_outHeight));
            }

    }
}

//LayoutPyramidal::Face LayoutPyramidal::AngleToFaceCart(const Coord3dCart& p) const
//{
//   //Rotate to canonical form
//   auto canonicP = CartToSherical(Rotation(p, -m_yaw, -m_pitch, -m_roll));
//
//   //Check where we are
//   auto theta = canonicP.y;
//   auto phi = canonicP.z;
//
//   double baseDemiAngle = std::atan2(m_baseEdge/2.0,1.0);
//
//   if (inInterval(theta, -baseDemiAngle, baseDemiAngle) && inInterval(phi, baseDemiAngle, PI()-baseDemiAngle))
//   {
//      return Face::Base;
//   }
//   //Else we test directly the intersection of each face:
//}

