/**
 * Author: Xavier Corbillon
 * Wrapper to generate a position that change with the time
 */
#pragma once

#include <string>
#include <fstream>

#include "Common.hpp"

namespace IMT {

class DynamicPosition
{
public:
  /** Constructor for a static position */
  DynamicPosition(double yaw, double pitch, double roll): m_isStatic(true), m_firstTimestamp(0.0), m_rotMat(GetRotMatrice(yaw, pitch, roll)), m_inputPositionsTrace(){}
  /** Constructor that take the path to an input positions trace file */
  DynamicPosition(std::string pathToInputPositionsTrace): m_isStatic(false), m_firstTimestamp(-1.0), m_rotMat(), m_inputPositionsTrace(pathToInputPositionsTrace){}

  const RotMat& GetNextPosition(void) const { return m_rotMat;}

  void SetNextPosition(double relatifTimestamp);

private:
  bool m_isStatic;
  double m_firstTimestamp;
  RotMat m_rotMat;
  std::ifstream m_inputPositionsTrace;
};
}
