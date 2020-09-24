#pragma once

//pcl
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/surface/mls.h>

//Robust pose estimation
//#include <pcl/common/time.h>
//#include <Eigen/Core>
//#include <pcl/features/normal_3d_omp.h>
//#include <pcl/features/fpfh.h>
//#include <pcl/filters/filter.h>
//#include <pcl/filters/voxel_grid.h>
//#include <pcl/io/pcd_io.h>
//#include <pcl/registration/icp.h>
//#include <pcl/registration/sample_consensus_prerejective.h>
//#include <pcl/segmentation/sac_segmentation.h>

//ICP
#include <pcl/point_representation.h>

#include <pcl/io/pcd_io.h>

#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/filter.h>

#include <pcl/registration/ndt.h>
#include <pcl/filters/approximate_voxel_grid.h>

#include <pcl/features/normal_3d.h>

#include <pcl/registration/icp.h>
#include <pcl/registration/icp_nl.h>
#include <pcl/registration/transforms.h>


#include <pcl/common/transforms.h>

//have to delete use the struct
#include "LayerControl.h"
using namespace pcl;
using namespace std;

class ICloudPoints_Tools
{
public:
	virtual std::vector<PointIndices> CloudSegmentation(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue, float nowCloud_avg_distance) = 0;
	virtual std::vector<complax_cloudInformation> cloudSegComplax(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue) = 0;
	virtual void CloudColor(PointCloud<PointXYZRGB>::Ptr nowLayerCloud) = 0;
	virtual PointCloud<PointXYZRGB>::Ptr CloudSmooth(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, float smooth_strength) = 0;
	virtual void registrationClouds(vector<PointCloud<PointXYZRGB>::Ptr> clouds) = 0;
};
//點雲處理工具，用於對點雲進行分割、平滑化、顏色變換等處理
class CloudPoints_Tools :ICloudPoints_Tools
{
public:
	CloudPoints_Tools() :ICloudPoints_Tools()
	{

	}

	std::vector<PointIndices> CloudSegmentation(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue, float nowCloud_avg_distance);
	std::vector<complax_cloudInformation> cloudSegComplax(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue);
	void CloudColor(PointCloud<PointXYZRGB>::Ptr nowLayerCloud);
	PointCloud<PointXYZRGB>::Ptr CloudSmooth(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, float smooth_strength);
	void registrationClouds(vector<PointCloud<PointXYZRGB>::Ptr> clouds);
private:
};
class MyPointRepresentation : public pcl::PointRepresentation <pcl::PointNormal>
{
	using pcl::PointRepresentation<pcl::PointNormal>::nr_dimensions_;
public:
	MyPointRepresentation()
	{
		// Define the number of dimensions
		nr_dimensions_ = 4;
	}

	// Override the copyToFloatArray method to define our feature vector
	virtual void copyToFloatArray(const pcl::PointNormal &p, float * out) const
	{
		// < x, y, z, curvature >
		out[0] = p.x;
		out[1] = p.y;
		out[2] = p.z;
		out[3] = p.curvature;
	}
};