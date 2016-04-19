#include "layoutPyramidal2.hpp"
#include "layoutPyramidalBased.hpp"

using namespace IMT;

unsigned int LayoutPyramidal2::IStartOffset(LayoutPyramidalBased::Faces f, unsigned int j) const
{   //hypothesis: Faces::Front is the biggest face
    switch(f)
    {
    case Faces::Base:
        return GetRes(Faces::Left);
    case Faces::Left:
        return 0;
    case Faces::Right:
        return GetRes(Faces::Left)+GetRes(Faces::Base);
    case Faces::Top:
        return GetRes(Faces::Left)+(GetRes(Faces::Base)-GetRes(Faces::Base)*j/GetRes(Faces::Top))/2;
    case Faces::Bottom:
        return GetRes(Faces::Left)+(GetRes(Faces::Base)-GetRes(Faces::Base)*(GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom)-j)/GetRes(Faces::Bottom))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutPyramidal2::IEndOffset(LayoutPyramidalBased::Faces f, unsigned int j) const
{   //hypothesis: Faces::Front is the biggest face
    switch(f)
    {
    case Faces::Base:
        return IStartOffset(Faces::Right, j);
    case Faces::Left:
        return IStartOffset(Faces::Base, j);
    case Faces::Right:
        return GetRes(Faces::Left)+GetRes(Faces::Base)+GetRes(Faces::Right);
    case Faces::Top:
        return IStartOffset(Faces::Right, j)-(GetRes(Faces::Base)-GetRes(Faces::Base)*j/GetRes(Faces::Top))/2;
    case Faces::Bottom:
        return IStartOffset(Faces::Right, j)-(GetRes(Faces::Base)-GetRes(Faces::Base)*(GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom)-j)/GetRes(Faces::Bottom))/2;
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutPyramidal2::JStartOffset(LayoutPyramidalBased::Faces f, unsigned int i) const
{   //hypothesis: Faces::Front is the biggest face
    switch(f)
    {
    case Faces::Base:
        return GetRes(Faces::Top);
    case Faces::Left:
        return GetRes(Faces::Top)+(GetRes(Faces::Base)-GetRes(Faces::Base)*i/GetRes(Faces::Left))/2;
    case Faces::Right:
        return GetRes(Faces::Top)+(GetRes(Faces::Base)-GetRes(Faces::Base)*(-i+GetRes(Faces::Left)+GetRes(Faces::Base)+GetRes(Faces::Right))/GetRes(Faces::Right))/2;
    case Faces::Top:
        return 0;
    case Faces::Bottom:
        return GetRes(Faces::Top)+GetRes(Faces::Base);
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

unsigned int LayoutPyramidal2::JEndOffset(LayoutPyramidalBased::Faces f, unsigned int i) const
{   //hypothesis: Faces::Front is the biggest face
    switch(f)
    {
    case Faces::Base:
        return JStartOffset(Faces::Bottom, i);
    case Faces::Left:
        return JStartOffset(Faces::Bottom, i)-(GetRes(Faces::Base)-GetRes(Faces::Base)*i/GetRes(Faces::Left))/2;
    case Faces::Right:
        return JStartOffset(Faces::Bottom, i)-(GetRes(Faces::Base)-GetRes(Faces::Base)*(-i+GetRes(Faces::Left)+GetRes(Faces::Base)+GetRes(Faces::Right))/GetRes(Faces::Right))/2;
    case Faces::Top:
        return JStartOffset(Faces::Base, i);
    case Faces::Bottom:
        return GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom);
    case Faces::Black:
    case Faces::Last:
        std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

LayoutPyramidalBased::Faces LayoutPyramidal2::From2dToFace(unsigned int i, unsigned int j) const
{//hypothesis:  GetRes(Faces::Front) is greater than GetRes for any other faces
	if ( InFace(i,j,Faces::Base))
    {
        return Faces::Base;
    }
    else if (InFace(i,j,Faces::Left))
    {
        return Faces::Left;
    }
    else if (InFace(i,j,Faces::Right))
    {
        return Faces::Right;
    }
    else if (InFace(i,j,Faces::Top))
    {
        return Faces::Top;
    }
    else if (InFace(i,j,Faces::Bottom))
    {
        return Faces::Bottom;
    }
    else
    {
        return Faces::Black;
    }
}

Layout::NormalizedFaceInfo LayoutPyramidal2::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    unsigned int i=pixel.x;
    unsigned int j=pixel.y;
    auto f = From2dToFace(i,j);
    if (f == Faces::Black)
    {
        return NormalizedFaceInfo(CoordF(0, 0), static_cast<int>(f));
    }
    switch(f)
    {
        case Faces::Base:
        {
           double normalizedI = double(i-IStartOffset(f,j))/GetRes(f);
           double normalizedJ = double(j-JStartOffset(f,i))/GetRes(f);
           return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
        }
        case Faces::Left:
        {
           double normalizedI = double(i-IStartOffset(f,j))/GetRes(f);
           double normalizedJ = i == 0? 0.5 : double(j-JStartOffset(f,i))/(GetRes(Faces::Base)*i/GetRes(Faces::Left));
           return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
        }
        case Faces::Right:
        {
           double normalizedI = double(i-IStartOffset(f,j))/GetRes(f);
           double normalizedJ = double(j-JStartOffset(f,i))/(GetRes(Faces::Base)*(-i+GetRes(Faces::Left)+GetRes(Faces::Base)+GetRes(Faces::Right))/GetRes(Faces::Right));
           return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
        }
        case Faces::Top:
        {
            double normalizedI = j == 0? 0.5 : double(i-IStartOffset(f,j))/(GetRes(Faces::Base)*j/GetRes(Faces::Top));
            double normalizedJ = double(j-JStartOffset(f,i))/GetRes(f);
            return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
        }
        case Faces::Bottom:
        {
            double normalizedI = double(i-IStartOffset(f,j))/(GetRes(Faces::Base)*(GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom)-j)/GetRes(Faces::Bottom));
            double normalizedJ = double(j-JStartOffset(f,i))/GetRes(f);
            return NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(f));
        }
    }


/*
    unsigned int i = pixel.x;
    unsigned int j = pixel.y;
    double normalizedI = double(i%m_edge)/m_edge;
    double normalizedJ = double(j%m_edge)/m_edge;
    if (inInterval(i, m_edge, 2*m_edge-1) && inInterval(j, m_edge, 2*m_edge-1))
    {//then we are on the base
         return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Base));
    }
    if (inInterval(i, 0, m_edge) && inInterval(j, 3.0/2*m_edge - 1.0/2*i, 3.0/2*m_edge + 1.0/2*i))
    {//then left
        if (normalizedI == 0) {normalizedJ = 0.5;}
        else {normalizedJ = (normalizedJ-0.5)/normalizedI+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Left));
    }
    if ( inInterval(j, 0, m_edge) && inInterval(i, 3.0/2*m_edge - 1.0/2*j, 3.0/2*m_edge + 1.0/2*j))
    {//then top
        if (normalizedJ == 0) {normalizedI = 0.5;}
        else {normalizedI = (normalizedI-0.5)/normalizedJ+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Top));
    }
    if ( inInterval(j, 2*m_edge, 3*m_edge) && inInterval(i, m_edge + 1.0/2*(j-2*m_edge), 2*m_edge - 1.0/2*(j-2*m_edge)))
    {//then bottom
        if ( normalizedJ == 1) {normalizedI = 0.5;}
        else {normalizedI = (normalizedI-0.5)/(1-normalizedJ)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Bottom));
    }
    if ( inInterval(i, 2*m_edge, 3*m_edge) && inInterval(j, m_edge + 1.0/2*(i-2*m_edge), 2*m_edge - 1.0/2*(i-2*m_edge)))
    {//then right
        if ( normalizedI == 1) {normalizedJ = 0.5;}
        else {normalizedJ = (normalizedJ-0.5)/(1-normalizedI)+0.5;}
        return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), static_cast<int>(Faces::Right));
    }
    //otherwise it's black
    return Layout::NormalizedFaceInfo(CoordF(0,0), static_cast<int>(Faces::Black));
    */
}

CoordF LayoutPyramidal2::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    const CoordF& coord (ni.m_normalizedFaceCoordinate);
    const double& normalizedI (coord.x);
    const double& normalizedJ (coord.y);

    if (f != Faces::Black && f != Faces::Last)
    {
        switch(f)
        {
            case Faces::Base:
                return CoordF(GetRes(f)*normalizedI+IStartOffset(f,coord.y), GetRes(f)*normalizedJ+JStartOffset(f,coord.x));
            case Faces::Left:
            {
                double i = GetRes(f)*normalizedI;
                double j = (GetRes(Faces::Base)*i/GetRes(Faces::Left))*normalizedJ+JStartOffset(f,i);
                return CoordF(i, j);
            }
            case Faces::Right:
            {
                double i = GetRes(f)*normalizedI + GetRes(Faces::Left) + GetRes(Faces::Base);
                double j = (GetRes(Faces::Base)*(-i+GetRes(Faces::Left)+GetRes(Faces::Base)+GetRes(Faces::Right))/GetRes(Faces::Right))*normalizedJ+JStartOffset(f,i);
                return CoordF(i, j);
            }
            case Faces::Top:
            {
                double j = GetRes(f)*normalizedJ;
                double i = (GetRes(Faces::Base)*j/GetRes(Faces::Top))*normalizedI+IStartOffset(f,j);
                return CoordF(i, j);
            }
            case Faces::Bottom:
            {
                double j = GetRes(f)*normalizedJ + GetRes(Faces::Top)+GetRes(Faces::Base);
                //double i = (GetRes(Faces::Base)*(GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom)-j)/GetRes(Faces::Bottom))*normalizedI+IStartOffset(f,j);
                double i = IStartOffset(f,j) + normalizedI * ((GetRes(Faces::Base)*(GetRes(Faces::Top)+GetRes(Faces::Base)+GetRes(Faces::Bottom)-j)/GetRes(Faces::Bottom)));
                return CoordF(i, j);
            }
        }
    }
    else
    {
        std::invalid_argument("FromNormalizedInfoTo2d: Last is not a valid face");
    }

/*
    switch (f)
    {
        case Faces::Base:
            return CoordF(m_edge*normalizedI+m_edge, normalizedJ*m_edge+m_edge);
        case Faces::Left:
            return CoordF(m_edge*normalizedI, (normalizedI*(normalizedJ-0.5)+0.5)*m_edge+m_edge);
        case Faces::Right:
            return CoordF(m_edge*normalizedI+2*m_edge, ((1-normalizedI)*(normalizedJ-0.5)+0.5)*m_edge+m_edge);
        case Faces::Top:
            return CoordF(m_edge*(normalizedJ*(normalizedI-0.5)+0.5)+m_edge, m_edge*normalizedJ);
        case Faces::Bottom:
            return CoordF(m_edge*((1-normalizedJ)*(normalizedI-0.5)+0.5)+m_edge, m_edge*normalizedJ+2*m_edge);
        case Faces::Last:
        case Faces::Black:
            throw std::invalid_argument("FromNormalizedInfoTo2d: Last or Black are not valid faces");

    }
*/
}
