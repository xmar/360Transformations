#include "layout.hpp"

#include "equirectangular.hpp"

using namespace IMT;

std::shared_ptr<Equirectangular> Layout::ToEquirectangular(const Picture& layoutPic, unsigned int width, unsigned int height) const
{
    /* (theta between [-pi,pi] and phi between [0,pi])
     *theta -> -pi              0                  pi
     *    0   +-----------------*-------------------+ 0
     *  phi   |                                     | y
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *        |                                     |
     *   pi   +-----------------+-------------------+ H
     *   x -> 0                                     W = 2.H
     */

    cv::Mat pic = cv::Mat::zeros(height, width, layoutPic.GetMat().type());
    auto eq = std::make_shared<Equirectangular>(pic);
    
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            double theta = 2*PI()*((double(i)/width)-0.5f);
            double phi = PI()*double(j)/height;
           auto coordLayout = fromSphereTo2d(theta, phi);
            if (coordLayout.x >= 0 && coordLayout.x < layoutPic.GetMat().cols && coordLayout.y >= 0 && coordLayout.y < layoutPic.GetMat().rows)
            {
                eq->SetValue(CoordI(i,j), layoutPic.GetInterPixel(coordLayout));
            }
            else
            {
                eq->SetValue(CoordI(i,j), Pixel(255,255,255));
            }
            if (i == 0 && j == 300)
            {
                std::cout << "DEBUG: (i,j) = ("<<i<<", "<<j<<"), (theta,phi)=(" << theta <<", "<< phi <<")" <<std::endl;
                eq->SetValue(CoordI(i,j), Pixel(255,0, 0));
            }
 
        }
    }  

    return eq;
}

std::shared_ptr<Picture> Layout::FromEquirectangular(const Equirectangular& eq, unsigned int width, unsigned int height) const
{
    cv::Mat picMat = cv::Mat::zeros(height, width, eq.GetMat().type());
    auto pic = std::make_shared<Picture>(picMat);
    for (auto i = 0; i < pic->GetMat().cols; ++i)
    {
        for (auto j = 0; j < pic->GetMat().rows; ++j)
        {
            auto polarP = from2dTo3dSpherical(i,j);
            pic->SetValue(CoordI(i,j), eq.GetPixelFromAngle(CoordF(polarP.y,polarP.z)));
        }
    }
    return pic;

}
        
std::shared_ptr<Picture> Layout::ToLayout(const Picture& layoutPic, const Layout& destLayout) const
{
    cv::Mat picMat = cv::Mat::zeros(destLayout.m_outHeight, destLayout.m_outWidth, layoutPic.GetMat().type());
    auto pic = std::make_shared<Picture>(picMat);
    #pragma omp parallel for collapse(2) shared(pic, layoutPic, destLayout)
    for (auto i = 0; i < pic->GetMat().cols; ++i)
    {
        for (auto j = 0; j < pic->GetMat().rows; ++j)
        {
            auto thisPixel3dPolar = destLayout.from2dTo3dSpherical(i,j); // coordinate of the pixel (i, j) in the output picture in the 3d space
            auto coordPixelOriginalPic = fromSphereTo2d(thisPixel3dPolar); //coordinate of the corresponding pixel in the input picture
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
    return pic;
}
