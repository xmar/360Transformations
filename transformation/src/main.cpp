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

#include "boost/program_options.hpp"
#include <boost/config.hpp>

#include <opencv2/opencv.hpp>

#include "picture.hpp"
#include "equirectangular.hpp"
#include "cubeMap.hpp"

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
      LayoutCubeMap lcm(cap.get(CV_CAP_PROP_FRAME_WIDTH));
      cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), 24, cv::Size(lcm.GetWidth(), lcm.GetHeight()));
      std::cout << "Nb frames: " << cap.get(CV_CAP_PROP_FRAME_COUNT)<< std::endl;
      cv::Mat img;
      int count = 0;
      while (cap.read(img))
      {
          Equirectangular pict(img);
          std::cout << "Read image" << std::endl;
          //cv::Mat resizeImg;
          //cv::resize(img, resizeImg, cv::Size(1200,600));
          //Equirectangular pict2(resizeImg);
          //pict2.ImgShow("Test");
          //cv::waitKey(0);
          //cv::destroyAllWindows();
          CubeMap cm(CubeMap::FromEquirectangular(pict, lcm));
          //cv::resize(cm.GetMat(), resizeImg, cv::Size(1200,600));
          //CubeMap cm2(resizeImg);
          //cm2.ImgShow("CubeMap");
          //cv::waitKey(0);
          //cv::destroyAllWindows();
          vwriter << cm.GetMat();
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
