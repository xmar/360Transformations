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
#define BORDER(x) (x)
//(MAX(0,MIN(GetRes(f)-1,x)))
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

std::shared_ptr<Picture> LayoutRhombicdodeca::ReadNextPictureFromVideoImpl(void)
{
    bool isInit = false;
    cv::Mat outputMat;
    for (unsigned i = 0; i < 12; ++i)
    {
        Faces f = static_cast<Faces>(i);
        cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetRes(f), GetRes(f) );
        auto facePictPtr = m_inputVideoPtr->GetNextPicture(i);
        if (!isInit)
        {
            outputMat = cv::Mat( m_outHeight, m_outWidth, facePictPtr->type());
            isInit = true;
        }
        cv::Mat facePictMat ( outputMat, roi);
        facePictPtr->copyTo(facePictMat);
    }
    return std::make_shared<Picture>(outputMat);
}

void LayoutRhombicdodeca::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
{
    for (unsigned i = 0; i < 12; ++i)
    {
        Faces f = static_cast<Faces>(i);
        cv::Rect roi( IStartOffset(f),  JStartOffset(f), GetRes(f), GetRes(f) );
        cv::Mat facePictMat ( pict->GetMat(), roi);
        m_outputVideoPtr->Write( facePictMat, i);
    }
}

std::shared_ptr<IMT::LibAv::VideoReader> LayoutRhombicdodeca::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
{
    std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
    vrPtr->Init(nbFrame);
    if (vrPtr->GetNbStream() != 12)
    {
        std::cout << "Unsupported number of stream for Rhombicdodeca input video: "<<vrPtr->GetNbStream() <<" instead of 12" << std::endl;
        return nullptr;
    }
    //we could add some other check for instance on the width, height of each stream
    return vrPtr;
}

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutRhombicdodeca::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<unsigned> bit_rateVect)
{
    std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
    std::array<unsigned, 12> br;
    std::copy_n(std::make_move_iterator(bit_rateVect.begin()), 12, br.begin());
    std::array<unsigned, 12> resArr;
    for (unsigned i = 0; i < 12; ++i)
    {
        resArr[i] = GetRes(static_cast<Faces>(i));
    }
    vwPtr->Init<12>(codecId, resArr, resArr, fps, gop_size, br);
    return vwPtr;
}
