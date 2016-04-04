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

#include "boost/program_options.hpp"
#include <boost/config.hpp>

#include <opencv2/opencv.hpp>

#include "picture.hpp"
#include "equirectangular.hpp"
#include "cubeMap.hpp"
#include "layoutEquirectangular.hpp"
#include "layoutFlatFixed.hpp"
#include "layoutPyramidal.hpp"
#include "layoutRhombicdodeca.hpp"

using namespace IMT;

int main( int argc, const char* argv[] )
{
   namespace po = boost::program_options;
   po::options_description desc("Options");
   desc.add_options()
      ("help,h", "Produce this help message")
      ("inputVideo,i", po::value<std::string>(), "path to the input video")
      ("outputVideo,o", po::value<std::string>(), "path to the output video")
      ("nbFrames,n", po::value<int>(), "number Of Frame to process (if <= 0 process the whole video) [default 0]")
      ;

   po::variables_map vm;
   try
   {
      po::store(po::parse_command_line(argc, argv, desc),
            vm);

      //--help
      if ( vm.count("help") || !vm.count("inputVideo") || !vm.count("outputVideo") )
      {
         std::cout << "Help: trans -i path -o path "<< std::endl
            <<  desc << std::endl;
         return 0;
      }

      po::notify(vm);

      auto pathToInputVideo = vm["inputVideo"].as<std::string>();
      auto pathToOutputVideo = vm["outputVideo"].as<std::string>();
      int nbFrames = vm.count("nbFrames") && vm["nbFrames"].as<int>() > 0 ? vm["nbFrames"].as<int>() : 0;


      //TEST
      cv::VideoCapture cap(pathToInputVideo);
      //cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), cap.get(CV_CAP_PROP_FPS), cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
      //unsigned int cubeEdge = cap.get(CV_CAP_PROP_FRAME_WIDTH)/3;
      LayoutEquirectangular leq (cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
      LayoutEquirectangular leq2(cap.get(CV_CAP_PROP_FRAME_WIDTH)/4, cap.get(CV_CAP_PROP_FRAME_HEIGHT)/4);
      LayoutCubeMap lcm(cap.get(CV_CAP_PROP_FRAME_WIDTH));
      LayoutPyramidal lp(2.1, 0, 0, 0, cap.get(CV_CAP_PROP_FRAME_HEIGHT));
      LayoutPyramidal lp2(3.1, 0, 0, 0, cap.get(CV_CAP_PROP_FRAME_HEIGHT));
      LayoutRhombicdodeca lr(cap.get(CV_CAP_PROP_FRAME_HEIGHT)/4);
      LayoutFlatFixed lff(PI()/2.f, -PI()/4.f, 0.f, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT), 3*PI()/4.f);
      cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), 24, cv::Size(lcm.GetWidth(), lcm.GetHeight()));
      std::cout << "Nb frames: " << cap.get(CV_CAP_PROP_FRAME_COUNT)<< std::endl;
      cv::Mat img;
      int count = 0;
      while (cap.read(img))
      {
          Equirectangular pict(img);
          std::cout << "Read image" << std::endl;
          pict.ImgShowResize("Test", cv::Size(1200,600));
          //cv::waitKey(0);
          //cv::destroyAllWindows();
          //CubeMap cm(CubeMap::FromEquirectangular(pict, lcm));
          //auto cm = lcm.FromEquirectangular(pict, cap.get(CV_CAP_PROP_FRAME_WIDTH), 2*cap.get(CV_CAP_PROP_FRAME_WIDTH)/3);
          //cv::resize(cm->GetMat(), resizeImg, cv::Size(1200,600));
          //CubeMap cm2(resizeImg);
          //cm2.ImgShow("CubeMap");
          //auto cm = lcm.FromLayout(pict, leq);
          //cm->ImgShowResize("CubeMap2", cv::Size(1200,800));

          ////cv::destroyAllWindows();
          //vwriter << cm->GetMat();
          //auto eq = lcm.ToLayout(*cm, leq);
          //eq->ImgShowResize("Test2", cv::Size(1200,600));


          //auto ff = lff.FromLayout(pict, leq);
          //ff->ImgShowResize("Flat Fix", cv::Size(1200,600));

          //auto ff2 = lff.FromLayout(*cm, lcm);
          //ff2->ImgShowResize("Flat Fix2", cv::Size(1200,600));

          //auto p = lp.FromLayout(pict, leq);
          //p->ImgShowResize("Pyramidal", cv::Size(900,300));

          //auto eq2 =  lp.ToLayout(*p, leq);
          //eq2->ImgShowResize("PyramidalToEq", cv::Size(1200,600));

          //p = lp2.FromLayout(pict, leq);
          //p->ImgShowResize("Pyramidal2", cv::Size(900,300));

          //eq2 =  lp2.ToLayout(*p, leq);
          //eq2->ImgShowResize("Pyramidal2ToEq", cv::Size(1200,600));

          auto rhombic = lr.FromLayout(pict, leq);
          rhombic->ImgShowResize("Rhombicstuff", cv::Size(1200,400));

          auto eq4 =  leq2.FromLayout(*rhombic, lr);
          eq4->ImgShowResize("RhombicToEq", cv::Size(1200,600));

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
