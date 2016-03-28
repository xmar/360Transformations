#pragma once
#include "picture.hpp"
#include <opencv2/opencv.hpp>

namespace IMT {

class Equirectangular;

class CubeMap: public Picture {
    public:
        CubeMap(void): Picture() {}
        CubeMap(cv::Mat img): Picture(img) {}
        ~CubeMap(void) = default;

        static CubeMap FromEquirectangular(const Equirectangular& er);
    private:
};

}
