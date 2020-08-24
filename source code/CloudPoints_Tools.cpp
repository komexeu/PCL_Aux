#include "CloudPoints_Tools.h"
#include <qdebug.h>
//
//#include <iostream>
//#include <string>
//
//#include <pcl/io/ply_io.h>
//#include <pcl/point_types.h>
//#include <pcl/registration/icp.h>
//#include <pcl/visualization/pcl_visualizer.h>
//#include <pcl/console/time.h>   // TicToc

#include <Eigen/Core>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/time.h>
#include <pcl/console/print.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/fpfh_omp.h>
#include <pcl/filters/filter.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/io/pcd_io.h>
#include <pcl/registration/icp.h>
#include <pcl/registration/sample_consensus_prerejective.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/common/transforms.h>
#include <pcl/filters/statistical_outlier_removal.h>

using pcl::visualization::PointCloudColorHandlerGenericField;
using pcl::visualization::PointCloudColorHandlerCustom;

std::vector<PointIndices> CloudPoints_Tools::CloudSegmentation(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue) {
	std::vector<PointCloud<PointXYZRGB>::Ptr> cluster_clouds_;
	std::vector<PointIndices> cluster_indice;
	if (nowLayerCloud->size() > 700000)
	{
		return cluster_indice;
	}
	PointCloud<PointXYZRGB>::Ptr nowLayrCloudClone(new PointCloud<PointXYZRGB>);
	copyPointCloud(*nowLayerCloud, *nowLayrCloudClone);

	std::vector<int> k_indices;
	std::vector<float> k_sqr_distances;
	int n = 0;
	float norm = 0;
	int searched_points = 0;
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(nowLayrCloudClone);
	for (int i = 0; i < nowLayrCloudClone->size(); i++)
	{
		n = tree->nearestKSearch(i, 2, k_indices, k_sqr_distances);
		if (n == 2)
		{
			norm += sqrt(k_sqr_distances[1]);
			++searched_points;
		}
	}
	double point_avg_distance = 0;
	if (searched_points != 0)
		point_avg_distance = norm /= searched_points;
	qDebug() << point_avg_distance;
	if (point_avg_distance == 0)
	{
		return cluster_indice;
		qDebug() << "NO DATA";
	}

	EuclideanClusterExtraction<PointXYZRGB> ec;
	ec.setClusterTolerance(double(sliderValue *point_avg_distance*0.01));
	ec.setMinClusterSize(300);
	ec.setSearchMethod(tree);
	ec.setInputCloud(nowLayrCloudClone);
	ec.extract(cluster_indice);

	return cluster_indice;
}

std::vector<complax_cloudInformation> CloudPoints_Tools::cloudSegComplax(PointCloud<PointXYZRGB>::Ptr nowLayerCloud, int sliderValue) {
	std::vector<complax_cloudInformation> cluster_clouds_;
	if (nowLayerCloud->size() > 100000)
	{
		qDebug() << "Too Many points to be deal.";
		return cluster_clouds_;
	}	
	PointCloud<PointXYZRGB>::Ptr nowLayrCloudClone(new PointCloud<PointXYZRGB>);
	nowLayrCloudClone = nowLayerCloud;

	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(nowLayrCloudClone);
	//EuclideanClusterExtraction<PointXYZRGB>::KdTreePtr tree(new EuclideanClusterExtraction<PointXYZRGB>);
	//tree->setInputCloud(nowLayrCloudClone);

	EuclideanClusterExtraction<PointXYZRGB> ec;
	ec.setClusterTolerance(float(sliderValue *0.001));
	ec.setMinClusterSize(300);
	ec.setSearchMethod(tree);
	ec.setInputCloud(nowLayrCloudClone);
	std::vector<PointIndices> cluster_indice;
	ec.extract(cluster_indice);
	for (int i = 0; i < nowLayerCloud->size(); ++i)
	{
		nowLayerCloud->points[i].r = 255;
		nowLayerCloud->points[i].g = 255;
		nowLayerCloud->points[i].b = 255;
	}
	int irrr = 0;
	for (std::vector<PointIndices>::const_iterator i = cluster_indice.begin(); i != cluster_indice.end(); ++i)
	{
		++irrr;
		qDebug() << "--------" << irrr;
		int color_R = rand() % 250;
		int color_G = rand() % 250;
		int color_B = rand() % 250;
		//--------------
		complax_cloudInformation cluster;
		cluster.cloud_data.reset(new pcl::PointCloud<PointXYZRGB>);
		for (std::vector<int>::const_iterator point = i->indices.begin(); point != i->indices.end(); ++point) {
			nowLayrCloudClone->points[*point].r = color_R;
			nowLayrCloudClone->points[*point].g = color_G;
			nowLayrCloudClone->points[*point].b = color_B;
			cluster.cloud_data->points.push_back(nowLayrCloudClone->points[*point]);
			cluster.points_id.push_back(point - i->indices.begin());
			nowLayerCloud->points[*point].r = color_R;
			nowLayerCloud->points[*point].g = color_G;
			nowLayerCloud->points[*point].b = color_B;
		}
		cluster.cloud_data->width = cluster.cloud_data->points.size();
		cluster.cloud_data->height = 1;
		cluster.cloud_data->is_dense = true;

		if (cluster.cloud_data->points.size() <= 0)
			continue;

		cluster_clouds_.push_back(cluster);
	}
	return cluster_clouds_;
}

void CloudPoints_Tools::CloudColor(PointCloud<PointXYZRGB>::Ptr nowLayerCloud) {

}

PointCloud<PointXYZRGB>::Ptr CloudPoints_Tools::CloudSmooth(PointCloud<PointXYZRGB>::Ptr nowLayerCloud) {
	PointCloud<PointXYZRGB>::Ptr smoothCloud(new PointCloud<PointXYZRGB>);
	MovingLeastSquares<PointXYZRGB, PointXYZRGB>::Ptr mls(new MovingLeastSquares<PointXYZRGB, PointXYZRGB>);
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);

	tree->setInputCloud(nowLayerCloud);

	mls->setComputeNormals(true);
	mls->setInputCloud(nowLayerCloud);
	mls->setSearchMethod(tree);
	mls->setPolynomialOrder(2);
	mls->setNumberOfThreads(1);
	//mls->setPointDensity(nowLayerCloud->size());
	mls->setSearchRadius(0.01);
	//mls->setPolynomialFit(true);
	mls->process(*smoothCloud);

	return smoothCloud;
}
