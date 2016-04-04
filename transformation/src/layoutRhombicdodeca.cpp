#include "layoutRhombicdodeca.hpp"

using namespace IMT;

constexpr double rH = 1;
constexpr double rotYaw = 0;
constexpr double rotPitch = 0;//0;//0;//0;//0;//0;//0;//0;//0;//PI()/4;

Coord3dCart LayoutRhombicdodeca::from2dTo3d(unsigned int i, unsigned int j) const
{
    double normalizedI = double(i%m_faceHeight) / m_faceHeight - 0.5;//[-0.5, 0.5]
    double normalizedJ = double(j%m_faceHeight) / m_faceHeight - 0.5;//[-0.5, 0.5]
    if (inInterval(i, 0, m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face1
        return Rotation(Coord3dCart(2*normalizedJ/rH, 2 + 2*normalizedJ/rH, 2*normalizedI/1 -1), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 0, m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face2
        return Rotation(Coord3dCart(2*normalizedI/1 -1, -2*normalizedJ/rH, 2*normalizedJ/rH + 2), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, m_faceHeight, 2*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face3
        return Rotation(Coord3dCart(2*normalizedJ/rH, 2 - 2*normalizedJ/rH, -2*normalizedI/1 +1), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, m_faceHeight, 2*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face4
        return Rotation(Coord3dCart(2*normalizedJ/rH, 1- 2*normalizedI/1, 2*normalizedJ/rH + 2), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 2*m_faceHeight, 3*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face5
        return Rotation(Coord3dCart(2*normalizedI/1-1, -2*normalizedJ/rH, 2-2*normalizedJ/rH), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 2*m_faceHeight, 3*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face6
        return Rotation(Coord3dCart(2*normalizedJ/rH, 1-2*normalizedI/1, 2-2*normalizedJ/rH), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 3*m_faceHeight, 4*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face7
        return Rotation(Coord3dCart(2*normalizedI/1 - 1, 2 - 2*normalizedJ/rH, -2*normalizedJ/rH), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 3*m_faceHeight, 4*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face8
        return Rotation(Coord3dCart(2*normalizedJ/rH, 1- 2*normalizedI/1, -2-2*normalizedJ/rH ), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 4*m_faceHeight, 5*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face9
        return Rotation(Coord3dCart(2*normalizedJ/rH, 1 - 2*normalizedI/1, 2*normalizedJ/rH-2), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 4*m_faceHeight, 5*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face10
        return Rotation(Coord3dCart(2*normalizedI/1-1, - 2*normalizedJ/rH, 2*normalizedJ/rH-2 ), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 5*m_faceHeight, 6*m_faceHeight) && inInterval(j, 0, m_faceHeight))
    {//Face11
        return Rotation(Coord3dCart(2*normalizedJ/rH, 2*normalizedJ/rH-2, 2*normalizedI/1-1), rotYaw, rotPitch, 0);
    }
    else if (inInterval(i, 5*m_faceHeight, 6*m_faceHeight) && inInterval(j, m_faceHeight, 2*m_faceHeight))
    {//Face12
        return Rotation(Coord3dCart(2*normalizedJ/rH-2, - 2*normalizedJ/rH, 2*normalizedI/1+1 ), rotYaw, rotPitch, 0);
    }
}



CoordF LayoutRhombicdodeca::fromSphereTo2d(double theta, double phi) const
{
    auto f = Face::Last;
    Coord3dCart inter;
    double minRho = std::numeric_limits<double>::max();
    Coord3dSpherical p(1, theta, phi);


    for (auto testF: get_range<LayoutRhombicdodeca::Face>())
    {
        try {
            auto plan = FaceToPlan(testF);
            auto interSphe = IntersectionPlanSpherical(plan, p); //raise exception if no intersection
            if (minRho > interSphe.x && AlmostEqual(interSphe.y, p.y)) //check direction
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
    switch (f)
    {
        case Face::Face1:
            {
                double i = inter.z/2.0-0.5; // [-0.5,0.5]
                double j = inter.x/2.0+0.5; // [-0.5,0.5]
                return CoordF((i+0.5)*m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face2:
            {
                double i = inter.x/2.0+0.5;
                double j = 0.5-inter.y/2.0;
                return CoordF((i+0.5)*m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
        case Face::Face3:
            {
                double i = (1-inter.z)/2.0;
                double j = inter.x/2.0 - 0.5;
                return CoordF((i+0.5)*m_faceHeight+m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face4:
            {
                double i = (1-inter.y)/2.0;
                double j = 0.5+inter.x/2.0;
                return CoordF((i+0.5)*m_faceHeight+m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
        case Face::Face5:
            {
                double i = (inter.x+1)/2.0;
                double j = (1-inter.y)/2.0;
                return CoordF((i+0.5)*m_faceHeight+2*m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face6:
            {
                double i = (1-inter.y)/2.0;
                double j = (1+inter.x)/2.0;
                return CoordF((i+0.5)*m_faceHeight+2*m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
        case Face::Face7:
            {
                double i = (inter.x+1)/2.0;
                double j = (1-inter.z)/2.0;
                return CoordF((i+0.5)*m_faceHeight+3*m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face8:
            {
                double i = (1-inter.y)/2.0;
                double j = (1+inter.x)/2.0;
                return CoordF((i+0.5)*m_faceHeight+3*m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
        case Face::Face9:
            {
                double i = (1+inter.x)/2;
                double j = (1-inter.y)/2.0;
                return CoordF((i+0.5)*m_faceHeight+4*m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face10:
            {
                double i = (inter.x+1)/2;
                double j = (1-inter.y)/2;
                return CoordF((i+0.5)*m_faceHeight+4*m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
        case Face::Face11:
            {
                double i = (inter.z+1)/2.0;
                double j = (1+inter.x)/2.0;
                return CoordF((i+0.5)*m_faceHeight+5*m_faceHeight,(j+0.5)*m_faceHeight);
            }
        case Face::Face12:
            {
                double i = (inter.z-1)/2.0;
                double j = (1-inter.y)/2.0;
                return CoordF((i+0.5)*m_faceHeight+5*m_faceHeight,(j+0.5)*m_faceHeight+m_faceHeight);
            }
    }
}
