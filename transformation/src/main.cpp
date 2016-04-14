/**********************************
 * Institut Mine-Telecom / Telecom Bretagne
 * Author: Xavier CORBILLON
 *
 * Program developped to test the 360° coding methods
 */

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <memory>
#include <math.h>

#include "boost/program_options.hpp"
#include <boost/config.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>

#include <opencv2/opencv.hpp>

#include "picture.hpp"
#include "layoutCubeMap.hpp"
#include "layoutCubeMap2.hpp"
#include "layoutEquirectangular.hpp"
#include "layoutFlatFixed.hpp"
#include "layoutPyramidal.hpp"
#include "layoutRhombicdodeca.hpp"
#include "GlobalArgsStructure.hpp"

using namespace IMT;

int main( int argc, const char* argv[] )
{
   namespace po = boost::program_options;
   namespace pt = boost::property_tree;
   po::options_description desc("Options");
   desc.add_options()
      ("help,h", "Produce this help message")
      ("inputVideo,i", po::value<std::string>(), "path to the input video")
      ("outputVideo,o", po::value<std::string>(), "path to the output video")
      ("nbFrames,n", po::value<int>(), "number Of Frame to process (if <= 0 process the whole video) [default 0]")
      ("config,c", po::value<std::string>(),"Path to the configuration file")
      ;

   po::variables_map vm;
   try
   {
      po::store(po::parse_command_line(argc, argv, desc),
            vm);

      //--help
      if ( vm.count("help") || !vm.count("inputVideo") || !vm.count("outputVideo") || !vm.count("config"))
      {
         std::cout << "Help: trans -i path -o path -c config"<< std::endl
            <<  desc << std::endl;
         return 0;
      }

      po::notify(vm);

      auto pathToInputVideo = vm["inputVideo"].as<std::string>();
      auto pathToOutputVideo = vm["outputVideo"].as<std::string>();
      int nbFrames = vm.count("nbFrames") && vm["nbFrames"].as<int>() > 0 ? vm["nbFrames"].as<int>() : 0;
      std::string pathToIni = vm["config"].as<std::string>();

      std::cout << "Path to the ini file: " <<pathToIni << std::endl;
      GlobalArgsStructure sga;

      //read the ini file the feed the GlobalArgsStructure
      pt::ptree ptree;
      pt::ptree ptree_json;
      pt::ini_parser::read_ini(pathToIni, ptree);
      //read the bitrates array
      std::stringstream ss(ptree.get<std::string>("Global.bitrates"));
      pt::json_parser::read_json(ss, ptree_json);
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, ptree_json.get_child(""))
      {
    	  auto br = boost::lexical_cast<int>(v.second.data()); sga.bitrates.push_back(br);
      }

      sga.frameWidth = ptree.get<int>("Global.frameWidth");
      sga.frameHeight = ptree.get<int>("Global.frameHeight");
      sga.fps = ptree.get<int>("Global.fps");

      sga.configurableFlatFixed = ptree.get<bool>("FlatFixed.configurableFlatFixed");
      sga.yaw = ptree.get<float>("FlatFixed.yaw");
      sga.pitch = ptree.get<float>("FlatFixed.pitch");
      sga.roll = ptree.get<float>("FlatFixed.roll");
      sga.horizontalAngleOfVision = ptree.get<float>("FlatFixed.horizontalAngleOfVision");
      sga.verticalAngleOfVision = ptree.get<float>("FlatFixed.verticalAngleOfVision");

      sga.configurableCubeMapResolution = ptree.get<bool>("CubeMap.configurableCubeMapResolution");
      sga.cubeEdgeLengthFace1 = ptree.get<int>("CubeMap.cubeEdgeLengthFace1");
      sga.cubeEdgeLengthFace2 = ptree.get<int>("CubeMap.cubeEdgeLengthFace2");
      sga.cubeEdgeLengthFace3 = ptree.get<int>("CubeMap.cubeEdgeLengthFace3");
      sga.cubeEdgeLengthFace4 = ptree.get<int>("CubeMap.cubeEdgeLengthFace4");
      sga.cubeEdgeLengthFace5 = ptree.get<int>("CubeMap.cubeEdgeLengthFace5");
      sga.cubeEdgeLengthFace6 = ptree.get<int>("CubeMap.cubeEdgeLengthFace6");

      sga.configurablePyramidResolution = ptree.get<bool>("Pyramid.configurablePyramidResolution");
      sga.pyramidBaseEdgeLength = ptree.get<int>("Pyramid.pyramidBaseEdgeLength");
      sga.pyramidHeightFace1 = ptree.get<float>("Pyramid.pyramidHeightFace1");
      sga.pyramidHeightFace2 = ptree.get<float>("Pyramid.pyramidHeightFace2");
      sga.pyramidHeightFace3 = ptree.get<float>("Pyramid.pyramidHeightFace3");
      sga.pyramidHeightFace4 = ptree.get<float>("Pyramid.pyramidHeightFace4");

      sga.configurableRhombicDodecaResolution = ptree.get<bool>("RhombicDodeca.configurableRhombicDodecaResolution");
      sga.rhombEdgeLengthFace1 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace1");
      sga.rhombEdgeLengthFace2 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace2");
      sga.rhombEdgeLengthFace3 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace3");
      sga.rhombEdgeLengthFace4 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace4");
      sga.rhombEdgeLengthFace5 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace5");
      sga.rhombEdgeLengthFace6 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace6");
      sga.rhombEdgeLengthFace7 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace7");
      sga.rhombEdgeLengthFace8 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace8");
      sga.rhombEdgeLengthFace9 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace9");
      sga.rhombEdgeLengthFace10 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace10");
      sga.rhombEdgeLengthFace11 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace11");
      sga.rhombEdgeLengthFace12 = ptree.get<int>("RhombicDodeca.rhombEdgeLengthFace12");

      //TEST
      cv::VideoCapture cap(pathToInputVideo);
      //cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), cap.get(CV_CAP_PROP_FPS), cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
      //unsigned int cubeEdge = cap.get(CV_CAP_PROP_FRAME_WIDTH)/3;
      LayoutEquirectangular leq (cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
      LayoutEquirectangular leq2(cap.get(CV_CAP_PROP_FRAME_WIDTH)/4, cap.get(CV_CAP_PROP_FRAME_HEIGHT)/4);

      unsigned int cubeMapEdgeLength;
      double pyramidEdgeLength;
      unsigned int pyramidHeight;
      unsigned int rhombicEdgeLength;
      if (sga.configurableCubeMapResolution==false) {
            cubeMapEdgeLength = cap.get(CV_CAP_PROP_FRAME_WIDTH)/4;
      } else {
            cubeMapEdgeLength = sga.cubeEdgeLengthFace1;
      }
      LayoutCubeMap lcm(cubeMapEdgeLength);
      LayoutCubeMap2 lcm2(cubeMapEdgeLength);
      //LayoutCubeMap2 lcm2 = LayoutCubeMap2::GenerateLayout({{cubeMapEdgeLength, cubeMapEdgeLength/3, cubeMapEdgeLength/2,
                                                           cubeMapEdgeLength/2, cubeMapEdgeLength/2, cubeMapEdgeLength/2}});

      if (sga.configurablePyramidResolution==false) {
            pyramidEdgeLength = cap.get(CV_CAP_PROP_FRAME_WIDTH)/4;
            pyramidHeight = cap.get(CV_CAP_PROP_FRAME_WIDTH)/4;
      } else {
            pyramidEdgeLength = sga.pyramidBaseEdgeLength;
            pyramidHeight = sga.pyramidHeightFace1;
      }
      LayoutPyramidal lp(pyramidEdgeLength, 0, 0, 0, pyramidHeight);
      LayoutPyramidal lp2(pyramidEdgeLength+1, 0, 0, 0, pyramidHeight);

      if (sga.configurableRhombicDodecaResolution==false) {
            rhombicEdgeLength = cap.get(CV_CAP_PROP_FRAME_HEIGHT)/2;
      } else {
            rhombicEdgeLength = sga.rhombEdgeLengthFace1;
      }
      LayoutRhombicdodeca lr(rhombicEdgeLength);

      unsigned int frameWidth;
      unsigned int frameHeight;
      if (sga.configurableFlatFixed==false) {
            frameWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
            frameHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
           // LayoutFlatFixed lff(PI()/2.f, -PI()/4.f, 0.f, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT), 3*PI()/4.f);
      } else {
            frameWidth = sga.frameWidth;
            frameHeight = sga.frameHeight;
           // LayoutFlatFixed lff(sga.yaw, sga.pitch, sga.roll, sga.frameWidth, sga.frameHeight, sga.horizontalAngleOfVision);
      }
      LayoutFlatFixed lff(sga.yaw, sga.pitch, sga.roll, frameWidth, frameHeight, sga.horizontalAngleOfVision);
      cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), sga.fps, cv::Size(lcm.GetWidth(), lcm.GetHeight()));
      std::cout << "Nb frames: " << cap.get(CV_CAP_PROP_FRAME_COUNT)<< std::endl;
      cv::Mat img;
      int count = 0;
      while (cap.read(img))
      {
          Picture pict(img);
          std::cout << "Read image" << std::endl;
          pict.ImgShowResize("Test", cv::Size(1200,600));
          //cv::waitKey(0);
          //cv::destroyAllWindows();

//          auto cm = lcm.FromLayout(pict, leq);
//          cm->ImgShowResize("CubeMap", cv::Size(1200,800));
//
//          ////cv::destroyAllWindows();
//          //vwriter << cm->GetMat();
//
//          auto eq = lcm.ToLayout(*cm, leq);
//          eq->ImgShowResize("Test2", cv::Size(1200,600));
//
//
//         auto ff = lff.FromLayout(pict, leq);
//          ff->ImgShowResize("Flat Fix", cv::Size(1200,600));
////
//          auto ff2 = lff.FromLayout(*cm, lcm);
//          ff2->ImgShowResize("Flat Fix2", cv::Size(1200,600));
////
//           auto p = lp.FromLayout(pict, leq);
//           p->ImgShowResize("Pyramidal", cv::Size(900,300));
//
//           auto eq2 =  lp.ToLayout(*p, leq);
//           eq2->ImgShowResize("PyramidalToEq", cv::Size(1200,600));
//
//           p = lp2.FromLayout(pict, leq);
//           p->ImgShowResize("Pyramidal2", cv::Size(900,300));
//
//           eq2 =  lp2.ToLayout(*p, leq);
//           eq2->ImgShowResize("Pyramidal2ToEq", cv::Size(1200,600));
//
//           auto rhombic = lr.FromLayout(pict, leq);
//           rhombic->ImgShowResize("Rhombicstuff", cv::Size(1200,400));
//
//           auto eq4 =  leq.FromLayout(*rhombic, lr);
//           eq4->ImgShowResize("RhombicToEq", cv::Size(1200,600));
////
//          //auto ff3 = lff.FromLayout(*rhombic, lr);
//        //  ff3->ImgShowResize("Flat Fix3", cv::Size(1200,600));
////
          auto cm2 = lcm2.FromLayout(pict, leq);
          cm2->ImgShowResize("CubeMap2", cv::Size(1200,900));

          auto eq5 = leq.FromLayout(*cm2, lcm2);
          eq5->ImgShowResize("CubeMap2ToEq", cv::Size(1200,600));

          cv::waitKey(0);
          cv::destroyAllWindows();
          if (++count == nbFrames)
          {
              break;
          }
      }
   }
   catch(const po::error& e)
   {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl
         << "Help: remuxer pathToInputVideo pathToOutputVideo [-hs] [--keepAll] [-r [--randomLossSize]] "<< std::endl
         << desc << std::endl;
      return 1;
   }
   catch(...)
   {
      std::cerr << "Uncatched exception" << std::endl
         << "Help: remuxer -i pathToInputVideo -o pathToOutputVideo [-hs] [--keepAll] [-r [--randomLossSize]] "<< std::endl
         << desc << std::endl;
      return 1;

   }

   return 0;
}
