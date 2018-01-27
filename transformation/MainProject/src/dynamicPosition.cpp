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
      double timestamp(0.0);
      unsigned int frameId(0);
      SCALAR w(1), x(0), y(0), z(0);
      //Read one line from the trace (formated like for this dataset: dash.ipv6.enstb.fr/headMovements/)
      // timestamp frameId w x y z
      std::string line;
      while (std::getline(m_inputPositionsTrace, line))
      {
        std::istringstream ss(line);
        std::string value;
        unsigned int counter(0);
        while (getline( ss, value, ' ' ))
        {
          if (counter == 0)
          {
              timestamp = std::stod(value);
              if (m_firstTimestamp < 0)
              {
                m_firstTimestamp = timestamp;
              }
              timestamp -= m_firstTimestamp;
              if (relatifTimestamp < timestamp && m_rotQuaternion != Quaternion(0))
              {
                m_inputPositionsTrace.seekg(-line.size()-1,m_inputPositionsTrace.cur);
                return;
              }
          }
          else if (counter == 1)
          {
              frameId = std::stoul(value);
          }
          else if (counter == 2)
          {
              w = std::stod(value);
          }
          else if (counter == 3)
          {
              x = std::stod(value);
          }
          else if (counter == 4)
          {
              y = std::stod(value);
          }
          else if (counter == 5)
          {
              z = std::stod(value);
          }
          ++counter;
        }
        m_rotQuaternion = Quaternion(w, VectorCartesian(x, y, z));
#if DEBUG_DYNAMIC_POSITION
        // std::cout << positionId << " " << timestamp << " " << m_rotMat(0,0) << " " << m_rotMat(2,2) << std::endl;
        // double yaw(0.0), pitch(0.0), roll(0.0);
        // std::tie(yaw,pitch,roll) = GetEulerFromRotMat(m_rotMat);
        // std::cout << "(yaw, pitch, roll) = ("<<yaw*180.0/PI()<<","<<pitch*180.0/PI()<<","<<roll*180.0/PI()<<")" << std::endl;
#endif
      }
    }
  }
}
