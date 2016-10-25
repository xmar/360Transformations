/**
 * Author: Xavier Corbillon
 * Wrapper to generate a position that change with the time
 */

#include "dynamicPosition.hpp"
#include <sstream>

using namespace IMT;

#define DEBUG_DYNAMIC_POSITION 0
#if DEBUG_DYNAMIC_POSITION
#include <iostream>
#endif

void DynamicPosition::SetNextPosition(double relatifTimestamp)
{
  if (!m_isStatic)
  {
    if (m_inputPositionsTrace.eof())
    { //no new position to read. We stay at the same position
      return;
    }
    else
    {
      unsigned int positionId(0);
      double timestamp(0.0);
      //Read one line from the trace positionId, timestamp, rotMat(0,0), rotMat(0,1), ... rotMat(2,2)
      std::string line;
      while (std::getline(m_inputPositionsTrace, line))
      {
        std::istringstream ss(line);
        std::string value;
        unsigned int counter(0);
        while (getline( ss, value, ',' ))
        {
          if (counter == 0)
          {
              positionId = std::stoul(value);
          }
          else if (counter == 1)
          {
              timestamp = std::stod(value);
              if (m_firstTimestamp < 0)
              {
                m_firstTimestamp = timestamp;
              }
              timestamp -= m_firstTimestamp;
              if (relatifTimestamp < timestamp)
              {
                m_inputPositionsTrace.seekg(-line.size()-1,m_inputPositionsTrace.cur);
                return;
              }
          }
          else
          {
              unsigned int c = counter - 2;
              unsigned int i = c % 3;
              unsigned int j = c / 3;
              m_rotMat(j,i) = std::stod(value);
          }
          ++counter;
        }
        RotMat changeAxisPosition(1,0,0,
                                  0,0,1,
                                  0,1,0);
        m_rotMat = changeAxisPosition*m_rotMat*changeAxisPosition;
#if DEBUG_DYNAMIC_POSITION
        std::cout << positionId << " " << timestamp << " " << m_rotMat(0,0) << " " << m_rotMat(2,2) << std::endl;
        double yaw(0.0), pitch(0.0), roll(0.0);
        std::tie(yaw,pitch,roll) = GetEulerFromRotMat(m_rotMat);
        std::cout << "(yaw, pitch, roll) = ("<<yaw*180.0/PI()<<","<<pitch*180.0/PI()<<","<<roll*180.0/PI()<<")" << std::endl;
#endif
      }
    }
  }
}
