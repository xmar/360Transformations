#include "LayoutRhombicdodeca.hpp"

using namespace IMT;


unsigned int LayoutRhombicdodeca::IStartOffset(Faces f) const
{
    switch (f)
    {
        case Faces::Face1:
            return (m_colsMaxOffset[0]-GetRes(Faces::Face1))/2;
        case Faces::Face2:
            return (m_colsMaxOffset[0]-GetRes(Faces::Face2))/2;
        case Faces::Face3:
            return m_colsMaxOffset[0]+ (m_colsMaxOffset[1]-GetRes(Faces::Face3))/2;
        case Faces::Face4:
            return m_colsMaxOffset[0]+ (m_colsMaxOffset[1]-GetRes(Faces::Face4))/2;
        case Faces::Face5:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + (m_colsMaxOffset[2]-GetRes(Faces::Face5))/2;
        case Faces::Face6:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + (m_colsMaxOffset[2]-GetRes(Faces::Face6))/2;
        case Faces::Face7:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + (m_colsMaxOffset[3]-GetRes(Faces::Face7))/2;
        case Faces::Face8:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + (m_colsMaxOffset[3]-GetRes(Faces::Face8))/2;
        case Faces::Face9:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + (m_colsMaxOffset[4]-GetRes(Faces::Face9))/2;
        case Faces::Face10:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + (m_colsMaxOffset[4]-GetRes(Faces::Face10))/2;
        case Faces::Face11:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4] + (m_colsMaxOffset[5]-GetRes(Faces::Face11))/2;
        case Faces::Face12:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4] + (m_colsMaxOffset[5]-GetRes(Faces::Face12))/2;
        case Faces::Black:
            throw std::invalid_argument("IStartOffset: Black is not a valid face");
        case Faces::Last:
            throw std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutRhombicdodeca::IEndOffset(Faces f) const
{
    switch (f)
    {
        case Faces::Face1:
            return m_colsMaxOffset[0]-(m_colsMaxOffset[0]-GetRes(Faces::Face1))/2;
        case Faces::Face2:
            return m_colsMaxOffset[0]-(m_colsMaxOffset[0]-GetRes(Faces::Face2))/2;
        case Faces::Face3:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1]-(m_colsMaxOffset[1]-GetRes(Faces::Face3))/2;
        case Faces::Face4:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1]-(m_colsMaxOffset[1]-GetRes(Faces::Face4))/2;
        case Faces::Face5:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2]-(m_colsMaxOffset[2]-GetRes(Faces::Face5))/2;
        case Faces::Face6:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2]-(m_colsMaxOffset[2]-GetRes(Faces::Face6))/2;
        case Faces::Face7:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3]-(m_colsMaxOffset[3]-GetRes(Faces::Face7))/2;
        case Faces::Face8:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3]-(m_colsMaxOffset[3]-GetRes(Faces::Face8))/2;
        case Faces::Face9:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4]-(m_colsMaxOffset[4]-GetRes(Faces::Face9))/2;
        case Faces::Face10:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4]-(m_colsMaxOffset[4]-GetRes(Faces::Face10))/2;
        case Faces::Face11:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4] + m_colsMaxOffset[5]-(m_colsMaxOffset[5]-GetRes(Faces::Face11))/2;
        case Faces::Face12:
            return m_colsMaxOffset[0]+ m_colsMaxOffset[1] + m_colsMaxOffset[2] + m_colsMaxOffset[3] + m_colsMaxOffset[4] + m_colsMaxOffset[5]-(m_colsMaxOffset[5]-GetRes(Faces::Face12))/2;
        case Faces::Black:
            throw std::invalid_argument("IStartOffset: Black is not a valid face");
        case Faces::Last:
            throw std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutRhombicdodeca::JStartOffset(Faces f) const
{
    switch (f)
    {
        case Faces::Face1:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face1))/2;
        case Faces::Face2:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face2))/2;
        case Faces::Face3:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face3))/2;;
        case Faces::Face4:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face4))/2;
        case Faces::Face5:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face5))/2;;
        case Faces::Face6:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face6))/2;
        case Faces::Face7:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face7))/2;;
        case Faces::Face8:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face8))/2;
        case Faces::Face9:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face9))/2;;
        case Faces::Face10:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face10))/2;
        case Faces::Face11:
            return (m_rowsMaxOffset[0]-GetRes(Faces::Face11))/2;;
        case Faces::Face12:
            return m_rowsMaxOffset[0]+ (m_rowsMaxOffset[1]-GetRes(Faces::Face12))/2;
        case Faces::Black:
            throw std::invalid_argument("IStartOffset: Black is not a valid face");
        case Faces::Last:
            throw std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}
unsigned int LayoutRhombicdodeca::JEndOffset(Faces f) const
{
    switch (f)
    {
        case Faces::Face1:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face1))/2;
        case Faces::Face2:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face2))/2;
        case Faces::Face3:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face3))/2;
        case Faces::Face4:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face4))/2;
        case Faces::Face5:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face5))/2;
        case Faces::Face6:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face6))/2;
        case Faces::Face7:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face7))/2;
        case Faces::Face8:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face8))/2;
        case Faces::Face9:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face9))/2;
        case Faces::Face10:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face10))/2;
        case Faces::Face11:
            return m_rowsMaxOffset[0]-(m_rowsMaxOffset[0]-GetRes(Faces::Face11))/2;
        case Faces::Face12:
            return m_rowsMaxOffset[0]+ m_rowsMaxOffset[1]-(m_rowsMaxOffset[1]-GetRes(Faces::Face12))/2;
        case Faces::Black:
            throw std::invalid_argument("IStartOffset: Black is not a valid face");
        case Faces::Last:
            throw std::invalid_argument("IStartOffset: Last is not a valid face");
    }
}

Layout::NormalizedFaceInfo LayoutRhombicdodeca::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{
    auto f = LayoutToFace(pixel.x, pixel.y);
    if (f == Faces::Black)
    {
        return Layout::NormalizedFaceInfo(CoordF(0,0), static_cast<int>(f));
    }
    CoordF normalizedCoord(double(pixel.x-IStartOffset(f)) / GetRes(f), double(pixel.y-JStartOffset(f)) / GetRes(f));
    return Layout::NormalizedFaceInfo(normalizedCoord, static_cast<int>(f));
}
#define BORDER(x) (MAX(0,MIN(GetRes(f)-1,x)))
CoordF LayoutRhombicdodeca::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    auto f = static_cast<Faces>(ni.m_faceId);
    double i = BORDER(ni.m_normalizedFaceCoordinate.x*GetRes(f));
    double j = BORDER(ni.m_normalizedFaceCoordinate.y*GetRes(f));
    return CanonicLayoutCoordToLayoutCoord(i, j, f);
}

LayoutRhombicdodeca::Faces LayoutRhombicdodeca::LayoutToFace(unsigned int i, unsigned int j) const
{
    for(const auto& f: get_range<Faces>())
    {
        if (f != Faces::Last && f != Faces::Black && InFace(i,j,f))
        {
            return f;
        }
    }
    return Faces::Black;
}

CoordF LayoutRhombicdodeca::CanonicLayoutCoordToLayoutCoord(double i, double j, Faces f) const
{

    if (f == Faces::Last || f == Faces::Black)
    {
        throw std::invalid_argument("CanonicLayoutCoordToLayoutCoord: Last or Black are not a valid faces");
    }
    return CoordF(i+IStartOffset(f), j+JStartOffset(f));
}
