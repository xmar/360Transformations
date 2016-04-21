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

#include "picture.hpp"
#include "layout.hpp"
#include "configParser.hpp"

using namespace IMT;

int main( int argc, const char* argv[] )
{
   namespace po = boost::program_options;
   namespace pt = boost::property_tree;
   po::options_description desc("Options");
   desc.add_options()
      ("help,h", "Produce this help message")
      ("inputVideo,i", po::value<std::string>(), "path to the input video")
      ("nbFrames,n", po::value<int>(), "number Of Frame to process (if <= 0 process the whole video) [default 0]")
      ("config,c", po::value<std::string>(),"Path to the configuration file")
      ;

   po::variables_map vm;
   try
   {
      po::store(po::parse_command_line(argc, argv, desc),
            vm);

      //--help
      if ( vm.count("help") || !vm.count("inputVideo") || !vm.count("config"))
      {
         std::cout << "Help: trans -i path -o path -c config"<< std::endl
            <<  desc << std::endl;
         return 0;
      }

      po::notify(vm);

      auto pathToInputVideo = vm["inputVideo"].as<std::string>();
      int nbFrames = vm.count("nbFrames") && vm["nbFrames"].as<int>() > 0 ? vm["nbFrames"].as<int>() : 0;
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
          std::vector<std::string> lfs;
          BOOST_FOREACH(boost::property_tree::ptree::value_type & u, v.second)
          {
              lfs.push_back(u.second.data());
          }
          if (lfs.size()>0)
          {
              layoutFlowSections.push_back(std::move(lfs));
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

        cv::VideoCapture cap(pathToInputVideo);

        std::vector<std::vector<std::shared_ptr<Layout>>> layoutFlowVect;
        for(auto& lfsv: layoutFlowSections)
        {
            bool isFirst = true;
            layoutFlowVect.push_back(std::vector<std::shared_ptr<Layout>>());
            for(auto& lfs: lfsv)
            {
                layoutFlowVect.back().push_back(InitialiseLayout(lfs, ptree, isFirst, cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
                layoutFlowVect.back().back()->Init();
                isFirst = false;
            }
        }

        std::vector<cv::VideoWriter> cvVideoWriters;
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
                cvVideoWriters.emplace_back(path, cv::VideoWriter::fourcc('H','E','V','C'), fps,
                                            cv::Size(l->GetWidth(), l->GetHeight()));
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
      std::cout << "Nb frames: " << cap.get(CV_CAP_PROP_FRAME_COUNT)<< std::endl;
      cv::Mat img;
      int count = 0;
      double averageDuration = 0;
      while (cap.read(img))
      {
          auto startTime = std::chrono::high_resolution_clock::now();
          auto pict = std::make_shared<Picture>(img);
          std::cout << "Read image " << count << std::endl;

        unsigned int j = 0;
        decltype(pict) firstPict(nullptr);
        for(auto& lf: layoutFlowVect)
        {
            auto pictOut = pict;
            for (unsigned int i = 1; i < lf.size(); ++i)
            {
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
                std::cout << "Flow " << j << ": MS-SSIM = " << msssim << ", PSNR = " << firstPict->GetPSNR(*pictOut) << ", SSIM: " << firstPict->GetSSIM(*pictOut) << std::endl;
                *qualityWriterVect[j-1] << msssim << std::endl;
            }
            if (!cvVideoWriters.empty())
            {
                cvVideoWriters[j] << pictOut->GetMat();
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
