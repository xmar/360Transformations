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
#include <chrono>

#include "boost/program_options.hpp"
#include <boost/config.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "Picture.hpp"
#include "Layout.hpp"
#include "ConfigParser.hpp"
#include "VideoWriter.hpp"
#include "VideoReader.hpp"

#define DEBUG 1
#ifdef DEBUG
#define PRINT_DEBUG(x) std::cout << x << std::endl;
#elif
#define PRINT_DEBUG(x) {}
#endif // DEBUG

using namespace IMT;

int main( int argc, const char* argv[] )
{
   namespace po = boost::program_options;
   namespace pt = boost::property_tree;
   po::options_description desc("Options");
   desc.add_options()
      ("help,h", "Produce this help message")
      //("inputVideo,i", po::value<std::string>(), "path to the input video")
      ("config,c", po::value<std::string>(),"Path to the configuration file")
      ;

   po::variables_map vm;
   try
   {
      po::store(po::parse_command_line(argc, argv, desc),
            vm);

      //--help
      if ( vm.count("help") || !vm.count("config"))
      {
         std::cout << "Help: trans -c config"<< std::endl
            <<  desc << std::endl;
         return 0;
      }

      po::notify(vm);

      std::vector<std::string> pathToInputVideos;// = vm["inputVideo"].as<std::string>();
      std::string pathToIni = vm["config"].as<std::string>();

      std::cout << "Path to the ini file: " <<pathToIni << std::endl;

//      GlobalArgsStructure sga;
//
      //read the ini file the feed the GlobalArgsStructure
      pt::ptree ptree;
      pt::ptree ptree_json;
      pt::ini_parser::read_ini(pathToIni, ptree);

      std::vector<std::vector<std::string>> layoutFlowSections;
      try {
      std::stringstream ss(ptree.get<std::string>("Global.layoutFlow"));
      pt::json_parser::read_json(ss, ptree_json);
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, ptree_json.get_child(""))
      {
          std::vector<std::string> lfsv;
          bool first = true;
          BOOST_FOREACH(boost::property_tree::ptree::value_type & u, v.second)
          {
              if (first)
              {
                  first = false;
                  pathToInputVideos.push_back(u.second.data());
              }
              else
              {
                  lfsv.push_back(u.second.data());
              }
          }
          if (lfsv.size()>0)
          {
              layoutFlowSections.push_back(std::move(lfsv));
          }
      }
      }
      catch (std::exception &e)
      {
          std::cout << "Error while parsing the Global.layoutFlow: " << e.what() << std::endl;
          exit(1);
      }

      std::string pathToOutputVideo = ptree.get<std::string>("Global.videoOutputName");
      std::string pathToOutputQuality = ptree.get<std::string>("Global.qualityOutputName");
      bool displayFinalPict = ptree.get<bool>("Global.displayFinalPict");
      auto nbFrames = ptree.get<unsigned int>("Global.nbFrames");
      auto videoOutputBitRate = ptree.get<unsigned int>("Global.videoOutputBitRate");

        std::vector<std::vector<std::shared_ptr<Layout>>> layoutFlowVect;

        unsigned j = 0;
        for(auto& lfsv: layoutFlowSections)
        {
            bool isFirst = true;
            layoutFlowVect.push_back(std::vector<std::shared_ptr<Layout>>());
            CoordI refResolution ( 0, 0 );
            unsigned k = 0;
            for(auto& lfs: lfsv)
            {
                layoutFlowVect.back().push_back(InitialiseLayout(lfs, ptree, isFirst, refResolution.x, refResolution.y));
                layoutFlowVect.back().back()->Init();
                refResolution = layoutFlowVect.back().back()->GetReferenceResolution();
                isFirst = false;
                ++k;
            }
            ++j;
        }

      j = 0;
      for (auto& inputPath:pathToInputVideos)
      {
          PRINT_DEBUG("Start init input video for flow "<<j+1)
          layoutFlowVect[j][0]->InitInputVideo(inputPath, nbFrames);
          PRINT_DEBUG("Done init input video for flow "<<j+1)
          ++j;
      }

        if (!pathToOutputVideo.empty())
        {
            size_t lastindex = pathToOutputVideo.find_last_of(".");
            std::string pathToOutputVideoExtension = pathToOutputVideo.substr(lastindex, pathToOutputVideo.size());
            pathToOutputVideo = pathToOutputVideo.substr(0, lastindex);
            double fps = ptree.get<double>("Global.fps");
            unsigned int j = 0;
            for(auto& lfsv: layoutFlowSections)
            {
                const auto& l = layoutFlowVect[j].back();
                std::string path = pathToOutputVideo+std::to_string(j+1)+lfsv.back()+pathToOutputVideoExtension;
                std::cout << "Output video path for flow "<< j+1 <<": " << path << std::endl;

                auto bitrate = GetBitrateVector(lfsv.back(), ptree, videoOutputBitRate);
                l->InitOutputVideo(path, "libx265", fps, int(fps/2), bitrate);
                ++j;
            }
        }
        std::vector<std::shared_ptr<std::ofstream>> qualityWriterVect;
        if (!pathToOutputQuality.empty())
        {
            size_t lastindex = pathToOutputQuality.find_last_of(".");
            std::string pathToOutputQualityExtension = pathToOutputQuality.substr(lastindex, pathToOutputQuality.size());
            pathToOutputQuality = pathToOutputQuality.substr(0, lastindex);
            unsigned int j = 0;
            for(auto& lfsv: layoutFlowSections)
            {
                if (j != 0)
                {
                    const auto& l = layoutFlowVect[j].back();
                    std::string path = pathToOutputQuality+std::to_string(j+1)+lfsv.back()+pathToOutputQualityExtension;
                    std::cout << "Quality path for flow "<< j+1 <<": " << path << std::endl;
                    qualityWriterVect.push_back(std::make_shared<std::ofstream>(path));
                }
                ++j;
            }
        }
//      cv::VideoWriter vwriter(pathToOutputVideo, cv::VideoWriter::fourcc('D','A','V','C'), sga.fps, cv::Size(lcm.GetWidth(), lcm.GetHeight()));

      cv::Mat img;
      int count = 0;
      double averageDuration = 0;
      while (count < nbFrames)
      {
          auto startTime = std::chrono::high_resolution_clock::now();

          std::cout << "Read image " << count << std::endl;

        unsigned int j = 0;
        std::shared_ptr<Picture> firstPict(nullptr);
        for(auto& lf: layoutFlowVect)
        {
            std::shared_ptr<Picture> pict = lf[0]->ReadNextPictureFromVideo();
            auto pictOut = pict;
            std::cout << "Flow " << j << ": " << layoutFlowSections[j][0];
            for (unsigned int i = 1; i < lf.size(); ++i)
            {
                std::cout << " -> " << layoutFlowSections[j][i];
                pictOut = lf[i]->FromLayout(*pictOut, *lf[i-1]);
                lf[i]->NextStep();
            }
            if (firstPict == nullptr)
            {
                firstPict = pictOut;
            }
            if (displayFinalPict)
            {
                pictOut->ImgShowWithLimit("Output"+std::to_string(j)+": "+layoutFlowSections[j][lf.size()-1], cv::Size(1200,900));
            }
            if (!qualityWriterVect.empty() && j != 0)
            {
                auto msssim = firstPict->GetMSSSIM(*pictOut);
                auto psnr = firstPict->GetPSNR(*pictOut);
                std::cout << "Flow " << j << ": MS-SSIM = " << msssim << " PSNR = " << psnr << std::endl;
                *qualityWriterVect[j-1] << msssim << " " << psnr << std::endl;
            }
            if (!pathToOutputVideo.empty())
            {
                PRINT_DEBUG("Send picture to encoder "<<j+1)
                lf.back()->WritePictureToVideo(pictOut);
            }
            ++j;
        }

        if (displayFinalPict)
        {
            cv::waitKey(0);
            cv::destroyAllWindows();
        }

          auto endTime = std::chrono::high_resolution_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::microseconds>( endTime - startTime ).count();
          averageDuration = (averageDuration*count + duration)/(count+1);
          std::cout << "Elapsed time for this picture: "<< float(duration)/1000000.f<< "s "
            "estimated remaining time = " << (nbFrames-count-1)*averageDuration/1000000.f << "s "  << std::endl;
          if (++count == nbFrames)
          {
              break;
          }
      }
   }
   catch(const po::error& e)
   {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl
         << desc << std::endl;
      return 1;
   }
   catch(std::exception& e)
   {
      std::cerr << "Uncatched exception: " << e.what() << std::endl
         << desc << std::endl;
      return 1;

   }
   catch(...)
   {
      std::cerr << "Uncatched exception" << std::endl
        << desc << std::endl;
      return 1;

   }

   return 0;
}
