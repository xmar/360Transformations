#pragma once
#include "picture.hpp"

namespace IMT {

class Equirectangular: public Picture {
    public:
        Equirectangular(void): Picture(){};
        Equirectangular(cv::Mat img): Picture(img){};
        ~Equirectangular(void) = default;

        Pixel GetPixelFromAngle(const CoordF& cf) const;
    private:
};

}
