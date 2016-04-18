#include "picture.hpp"

using namespace IMT;

Pixel Picture::GetInterPixel(CoordF pt) const
{
    const cv::Mat& img = m_pictMat;
    assert(!img.empty());
    assert(img.channels() == 3);

    int x = (int)pt.x;
    int y = (int)pt.y;

    int x0 = cv::borderInterpolate(x,   img.cols, cv::BORDER_REFLECT_101);
    int x1 = cv::borderInterpolate(x+1, img.cols, cv::BORDER_REFLECT_101);
    int y0 = cv::borderInterpolate(y,   img.rows, cv::BORDER_REFLECT_101);
    int y1 = cv::borderInterpolate(y+1, img.rows, cv::BORDER_REFLECT_101);

    float a = pt.x - (float)x;
    float c = pt.y - (float)y;

    uchar b = (uchar)cvRound((img.at<Pixel>(y0, x0)[0] * (1.f - a) + img.at<Pixel>(y0, x1)[0] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[0] * (1.f - a) + img.at<Pixel>(y1, x1)[0] * a) * c);
    uchar g = (uchar)cvRound((img.at<Pixel>(y0, x0)[1] * (1.f - a) + img.at<Pixel>(y0, x1)[1] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[1] * (1.f - a) + img.at<Pixel>(y1, x1)[1] * a) * c);
    uchar r = (uchar)cvRound((img.at<Pixel>(y0, x0)[2] * (1.f - a) + img.at<Pixel>(y0, x1)[2] * a) * (1.f - c)
                           + (img.at<Pixel>(y1, x0)[2] * (1.f - a) + img.at<Pixel>(y1, x1)[2] * a) * c);

    return Pixel(b, g, r);
}

void Picture::ImgShowWithLimit(std::string txt, cv::Size s) const
{
    unsigned int width = s.width;
    unsigned int height = s.height;
    const unsigned int pictWidth = m_pictMat.cols;
    const unsigned int pictheight = m_pictMat.rows;
    if (height < pictheight && width < pictWidth)
    {
        if (pictheight > pictWidth)
        {
            width = float(height) * pictWidth / pictheight;
        }
        else
        {
            height = float(width) * pictheight / pictWidth;
        }
    }
    else if (width < pictWidth)
    {
        height = float(width) * pictheight / pictWidth;
    }
    else if (height < pictheight)
    {
        width = float(height) * pictWidth / pictheight;
    }
    else
    {
        height = pictheight;
        width = pictWidth;
        ImgShow(txt);
        return;
    }
    ImgShowResize(txt, cv::Size(width,height));
}

double Picture::GetMSE(const Picture& pic) const
{
    if (pic.GetHeight()!= GetHeight() && pic.GetWidth() != GetWidth())
    {
        throw std::invalid_argument("MSE computation require pictures to have the same width and height");
    }
    cv::Mat tmp(cv::Mat::zeros(GetHeight(), GetWidth(), m_pictMat.type()));
    cv::subtract(m_pictMat, pic.m_pictMat, tmp);
    cv::multiply(tmp, tmp, tmp);
    return cv::mean(tmp).val[0];
}

double Picture::GetPSNR(const Picture& pic) const
{
    auto mse = GetMSE(pic);
    return mse != 0 ? 10.0*std::log10(255*255/mse) : 100.0;
}
