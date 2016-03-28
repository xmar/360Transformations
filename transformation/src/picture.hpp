#pragma once

#include <opencv2/opencv.hpp>

namespace IMT {
    typedef cv::Vec3b Pixel;
    typedef cv::Point2f CoordF;
    typedef cv::Point2i CoordI;

    constexpr double PI() {return std::atan(1)*4;}
class Picture {
    public:
        Picture(void): m_pictMat(){};
        Picture(cv::Mat pictMat): m_pictMat(std::move(pictMat)){};
        ~Picture(void) = default;

        Pixel GetInterPixel(CoordF pt) const;
        Pixel GetPixel(CoordI pt) const {return m_pictMat.at<Pixel>(pt);}

        void ImgShow(std::string txt) const{
          cv::imshow(txt, m_pictMat);
        }
        const cv::Mat& GetMat(void) const {return m_pictMat;}
        void SetValue(CoordI pt, Pixel val) { m_pictMat.at<Pixel>(pt) = val; }
    protected:
        cv::Mat m_pictMat;
    private:
};
}
