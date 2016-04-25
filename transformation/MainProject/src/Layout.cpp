#include "Layout.hpp"
#include <stdexcept>


using namespace IMT;

Coord3dCart Layout::From2dTo3d(const CoordI& pixelCoord) const
{
    return FromNormalizedInfoTo3d(From2dToNormalizedFaceInfo(pixelCoord));
}

CoordF Layout::FromSphereTo2d(const Coord3dSpherical& sphericalCoord) const
{
    return FromNormalizedInfoTo2d(From3dToNormalizedFaceInfo(sphericalCoord));
}

std::shared_ptr<Picture> Layout::ToLayout(const Picture& layoutPic, const Layout& destLayout) const
{
    if (!m_isInit)
    {
        throw std::logic_error("Layout have to be initialized first before using it");
    }
    cv::Mat picMat = cv::Mat::zeros(destLayout.m_outHeight, destLayout.m_outWidth, layoutPic.GetMat().type());
    auto pic = std::make_shared<Picture>(picMat);
    #pragma omp parallel for collapse(2) shared(pic, layoutPic, destLayout) schedule(dynamic)
    for (auto i = 0; i < pic->GetMat().cols; ++i)
    {
        for (auto j = 0; j < pic->GetMat().rows; ++j)
        {
            Coord3dSpherical thisPixel3dPolar = destLayout.From2dTo3d(CoordI(i,j)); // coordinate of the pixel (i, j) in the output picture in the 3d space
            if (norm(thisPixel3dPolar) != 0)
            {//Keep the pixel black (i.e. do nothing) if == 0
                auto coordPixelOriginalPic = FromSphereTo2d(thisPixel3dPolar); //coordinate of the corresponding pixel in the input picture
                if (inInterval(coordPixelOriginalPic.x, 0, layoutPic.GetMat().cols) && inInterval(coordPixelOriginalPic.y, 0, layoutPic.GetMat().rows))
                {
                    pic->SetValue(CoordI(i,j), layoutPic.GetInterPixel(coordPixelOriginalPic));
                }
                else
                {
//                std::cout << "(i,j)= (" << i << "(max = "<< pic->GetMat().cols << "), "<<j <<") : theta = " << thisPixel3dPolar.y << "; phi=" << thisPixel3dPolar.z << "; iOrr = " << coordPixelOriginalPic.x << "; jOrr = " << coordPixelOriginalPic.y << "Max (i,j)orr = " << layoutPic.GetMat().cols << ", " << layoutPic.GetMat().rows << std::endl;
                }
            }
        }
    }
    return pic;
}
