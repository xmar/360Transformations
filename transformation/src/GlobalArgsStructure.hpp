#include<string>
#include<vector>
using namespace std;

namespace IMT {

struct GlobalArgsStructure {
    unsigned int frameWidth;
    unsigned int frameHeight;
    unsigned int fps;
    std::vector<int> bitrates;

    bool configurableFlatFixed;
    double yaw;
    double pitch;
    double roll;
    double horizontalAngleOfVision;
    double verticalAngleOfVision;

    bool configurableCubeMapResolution;
    unsigned int cubeEdgeLengthFace1;
    unsigned int cubeEdgeLengthFace2;
    unsigned int cubeEdgeLengthFace3;
    unsigned int cubeEdgeLengthFace4;
    unsigned int cubeEdgeLengthFace5;
    unsigned int cubeEdgeLengthFace6;

    bool configurablePyramidResolution;
    unsigned int pyramidBaseEdgeLength;
    float pyramidHeightFace1;
    float pyramidHeightFace2;
    float pyramidHeightFace3;
    float pyramidHeightFace4;

    bool configurableRhombicDodecaResolution;
    unsigned int rhombEdgeLengthFace1;
    unsigned int rhombEdgeLengthFace2;
    unsigned int rhombEdgeLengthFace3;
    unsigned int rhombEdgeLengthFace4;
    unsigned int rhombEdgeLengthFace5;
    unsigned int rhombEdgeLengthFace6;
    unsigned int rhombEdgeLengthFace7;
    unsigned int rhombEdgeLengthFace8;
    unsigned int rhombEdgeLengthFace9;
    unsigned int rhombEdgeLengthFace10;
    unsigned int rhombEdgeLengthFace11;
    unsigned int rhombEdgeLengthFace12;
};

}
