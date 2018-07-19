#include "LayoutViewport.hpp"

using namespace IMT;

Layout::NormalizedFaceInfo LayoutViewport::From2dToNormalizedFaceInfo(const CoordI& pixel) const
{

    double normalizedI = (double(pixel.x)/m_outWidth); //between 0 and 1
    double normalizedJ = (double(pixel.y)/m_outHeight); //between 0 and 1
    return Layout::NormalizedFaceInfo(CoordF(normalizedI, normalizedJ), 0);
}
CoordF LayoutViewport::FromNormalizedInfoTo2d(const Layout::NormalizedFaceInfo& ni) const
{
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    return CoordF(coord.x*m_outWidth, (coord.y)*m_outHeight);
}
Layout::NormalizedFaceInfo LayoutViewport::From3dToNormalizedFaceInfo(const Coord3dSpherical& sphericalCoord) const
{
    Coord3dCart cardPosition = sphericalCoord;
    double i(-1), j(-1);
    if (cardPosition.GetX() > 0)
    {
        double t = 1.0/cardPosition.GetX();
        Coord3dCart intersection = t*cardPosition;
        i = double(intersection.GetY())/(2*m_maxHDist) + 0.5;
        j = 0.5-double(intersection.GetZ())/(2*m_maxVDist);
    }
    return Layout::NormalizedFaceInfo(CoordF(i,j), 0);
}
Coord3dCart LayoutViewport::FromNormalizedInfoTo3d(const Layout::NormalizedFaceInfo& ni) const
{
    const CoordF& coord(ni.m_normalizedFaceCoordinate);
    double u = (coord.x-0.5)*(2*m_maxHDist);
    double v = (0.5-coord.y)*(2*m_maxVDist);
    Coord3dCart coordBefRot(1, u, v);
    coordBefRot /= coordBefRot.Norm();
    return coordBefRot;
}

std::shared_ptr<Picture> LayoutViewport::ReadNextPictureFromVideoImpl(void)
{
    auto matptr = m_inputVideoPtr->GetNextPicture(0);
    return std::make_shared<Picture>(matptr->clone());
}

void LayoutViewport::WritePictureToVideoImpl(std::shared_ptr<Picture> pict)
{
    m_outputVideoPtr->Write( pict->GetMat(), 0);
}

std::shared_ptr<IMT::LibAv::VideoReader> LayoutViewport::InitInputVideoImpl(std::string pathToInputVideo, unsigned nbFrame)
{
    std::shared_ptr<IMT::LibAv::VideoReader> vrPtr = std::make_shared<IMT::LibAv::VideoReader>(pathToInputVideo);
    vrPtr->Init(nbFrame);
    if (vrPtr->GetNbStream() != 1)
    {
        std::cout << "Unsupported number of stream for FlatFixed input video: "<<vrPtr->GetNbStream() <<" instead of 1" << std::endl;
        return nullptr;
    }
    //we could add some other check for instance on the width, height of each stream
    return vrPtr;
}

std::shared_ptr<IMT::LibAv::VideoWriter> LayoutViewport::InitOutputVideoImpl(std::string pathToOutputVideo, std::string codecId, unsigned fps, unsigned gop_size, std::vector<int> bit_rateVect)
{
    std::shared_ptr<IMT::LibAv::VideoWriter> vwPtr = std::make_shared<IMT::LibAv::VideoWriter>(pathToOutputVideo);
    vwPtr->Init<1>(codecId, {{m_outWidth}}, {{m_outHeight}}, fps, gop_size, {{bit_rateVect[0]}});
    return vwPtr;
}

REGISTER_LAYOUT("viewport", LayoutConfigParserViewport);
