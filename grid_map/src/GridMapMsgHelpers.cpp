/*
 * GridMapMsgHelpers.hpp
 *
 *  Created on: Sep 8, 2014
 *      Author: Péter Fankhauser
 *   Institute: ETH Zurich, Autonomous Systems Lab
 */

#include "grid_map/GridMapMsgHelpers.hpp"

// Boost
#include <boost/assign.hpp>

namespace grid_map {

const int nDimensions()
{
  return 2;
}

std::map<StorageIndices, std::string> storageIndexNames = boost::assign::map_list_of
    (StorageIndices::Column,  "column_index")
    (StorageIndices::Row, "row_index");

bool isRowMajor(const std_msgs::Float32MultiArray& messageData)
{
  if (messageData.layout.dim[0].label == storageIndexNames[StorageIndices::Column]) return false;
  else if (messageData.layout.dim[0].label == storageIndexNames[StorageIndices::Row]) return true;

  ROS_ERROR("elevation_map_msg: isRowMajor() failed because layout label is not set correctly.");
  return false;
}

unsigned int getCols(const std_msgs::Float32MultiArray& messageData)
{
  if (isRowMajor(messageData)) return messageData.layout.dim.at(1).size;
  return messageData.layout.dim.at(0).size;
}

unsigned int getRows(const std_msgs::Float32MultiArray& messageData)
{
  if (isRowMajor(messageData)) return messageData.layout.dim.at(0).size;
  return messageData.layout.dim.at(1).size;
}

bool multiArrayMessageCopyToMatrixEigen(const std_msgs::Float32MultiArray& m, Eigen::MatrixXf& e)
{
  if (e.IsRowMajor != isRowMajor(m))
  {
    ROS_ERROR("multiArrayMessageToMatrixEigen() failed because the storage order is not compatible.");
    return false;
  }

  Eigen::MatrixXf tempE(getRows(m), getCols(m));
  tempE = Eigen::Map<const Eigen::MatrixXf>(m.data.data(), getRows(m), getCols(m));
  e = tempE;
  return true;
}

bool multiArrayMessageMapToMatrixEigen(std_msgs::Float32MultiArray& m, Eigen::MatrixXf& e)
{
  if (e.IsRowMajor != isRowMajor(m))
  {
    ROS_ERROR("multiArrayMessageToMatrixEigen() failed because the storage order is not compatible.");
    return false;
  }

  e.resize(getRows(m), getCols(m));
  e = Eigen::Map<Eigen::MatrixXf>(m.data.data(), getRows(m), getCols(m));
  return true;
}

void transformColorVectorToValue(const Eigen::Vector3i& colorVector, float& colorValue)
{
  int color = (colorVector(0) << 16) + (colorVector(1) << 8) + colorVector(2);
  colorValue = *reinterpret_cast<float*>(&color);
}

} /* namespace */
