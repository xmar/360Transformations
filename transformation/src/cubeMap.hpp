#pragma once
#include "picture.hpp"
#include "layoutCubeMap.hpp"
#include <opencv2/opencv.hpp>

namespace IMT {

class Equirectangular;

class CubeMap: public Picture {
    public:
        CubeMap(LayoutCubeMap lcm, int type): m_lcm(lcm), Picture(cv::Mat::zeros(lcm.GetHeight(), lcm.GetWidth(), type)) {}
        CubeMap(cv::Mat img): Picture(img), m_lcm(img.cols) {}
        ~CubeMap(void) = default;

        static CubeMap FromEquirectangular(const Equirectangular& er, LayoutCubeMap& lcm);
    private:
        LayoutCubeMap m_lcm;
};

}
