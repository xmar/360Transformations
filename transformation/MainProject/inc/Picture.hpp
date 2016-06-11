#pragma once

#include <tuple>
#include <opencv2/opencv.hpp>
#include "Common.hpp"

namespace IMT {
class Picture {
    public:
        Picture(void): m_pictMat(){};
        Picture(cv::Mat pictMat): m_pictMat(std::move(pictMat)){};
        virtual ~Picture(void) {m_pictMat.release();};

        Pixel GetInterPixel(CoordF pt) const;
        Pixel GetPixel(CoordI pt) const {return m_pictMat.at<Pixel>(pt);}

        void ImgShow(std::string txt) const{
          cv::imshow(txt, m_pictMat);
        }
        void ImgShowResize(std::string txt, cv::Size s) const {
            cv::Mat resizeImg;
            cv::resize(m_pictMat, resizeImg, s, 0, 0, cv::INTER_LINEAR);
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
        /** \brief Return the Mean Square Error of the picture pic and this picture
         *
         * \param pic const Picture& The picture from witch we compare
         * \return double The computed Mean Square Error
         *
         */
        double GetMSE(const Picture& pic) const;
        /** \brief Return the PSNR of the picture pic with this Picture as the reference
         *
         * \param pic const Picture& picture of with we want to compute the PSNR
         * \return double The PSNR of the picture pic with this picture as reference
         *
         */
        double GetPSNR(const Picture& pic) const;

        double GetSSIM(const Picture& pic) const;
        double GetMSSSIM(const Picture& pic) const;

        const int& GetWidth(void) const {return m_pictMat.cols;}
        const int& GetHeight(void) const {return m_pictMat.rows;}
    protected:
        cv::Mat m_pictMat;
    private:
        static constexpr int m_nlevs = 5;
        static double m_mssimWeight[m_nlevs];
        static constexpr double m_ssim_c1 = 6.5025;
        static constexpr double m_ssim_c2 = 58.5225;

        void ApplyGaussianBlur(const cv::Mat& src, cv::Mat& dst, int ksize, double sigma) const;
        std::tuple<double,double> ComputeSSIM(const cv::Mat& img1, const cv::Mat& img2) const;

};
}
