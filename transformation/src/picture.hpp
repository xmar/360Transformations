#pragma once

#include <opencv2/opencv.hpp>
#include "common.hpp"

namespace IMT {
class Picture {
    public:
        Picture(void): m_pictMat(){};
        Picture(cv::Mat pictMat): m_pictMat(std::move(pictMat)){};
        virtual ~Picture(void) = default;

        Pixel GetInterPixel(CoordF pt) const;
        Pixel GetPixel(CoordI pt) const {return m_pictMat.at<Pixel>(pt);}

        void ImgShow(std::string txt) const{
          cv::imshow(txt, m_pictMat);
        }
        void ImgShowResize(std::string txt, cv::Size s) const {
            cv::Mat resizeImg;
            cv::resize(m_pictMat, resizeImg, s);
            cv::imshow(txt, resizeImg);
        }
        /** \brief Display the picture with it current dimention but will resize the picture not to exceed the size limit (keeping the picture propotions)
         *
         * \param txt std::string Name of the display window
         * \param s cv::Size Size limit the picture will not exceed
         *
         */
        void ImgShowWithLimit(std::string txt, cv::Size s) const;
        const cv::Mat& GetMat(void) const {return m_pictMat;}
        void SetValue(CoordI pt, Pixel val) { m_pictMat.at<Pixel>(pt) = val; }
    protected:
        cv::Mat m_pictMat;
    private:
};
}
