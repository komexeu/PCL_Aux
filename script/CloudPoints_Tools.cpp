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
//typedef pcl::PointXYZRGB PointT;
//typedef pcl::PointCloud<PointT> PointCloudT;
//
//bool next_iteration = false;

//typedef pcl::PointNormal PointNT;
//typedef pcl::PointCloud<PointNT> PointCloudT;
//typedef pcl::FPFHSignature33 FeatureT;
//typedef pcl::FPFHEstimationOMP<PointNT, PointNT, FeatureT> FeatureEstimationT;
//typedef pcl::PointCloud<FeatureT> FeatureCloudT;
//typedef pcl::visualization::PointCloudColorHandlerCustom<PointXYZRGB> ColorHandlerT;

//void
//keyboardEventOccurred(const pcl::visualization::KeyboardEvent& event,
//	void* nothing)
//{
//	if (event.getKeySym() == "space" && event.keyDown())
//		next_iteration = true;
//}

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
	qDebug() << "IM IN!!!!!";
	qDebug() << "IM IN!!!!!";
	qDebug() << "IM IN!!!!!";
	PointCloud<PointXYZRGB>::Ptr nowLayrCloudClone(new PointCloud<PointXYZRGB>);
	nowLayrCloudClone = nowLayerCloud;

	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(nowLayrCloudClone);
	//EuclideanClusterExtraction<PointXYZRGB>::KdTreePtr tree(new EuclideanClusterExtraction<PointXYZRGB>);
	//tree->setInputCloud(nowLayrCloudClone);

	EuclideanClusterExtraction<PointXYZRGB> ec;
	ec.setClusterTolerance(float(sliderValue *0.001));
	qDebug() << "11111111111111";
	ec.setMinClusterSize(300);
	qDebug() << "2222222222222";
	ec.setSearchMethod(tree);
	ec.setInputCloud(nowLayrCloudClone);
	std::vector<PointIndices> cluster_indice;
	qDebug() << "333333333333";
	ec.extract(cluster_indice);
	qDebug() << "IM TREE!!!!!";
	qDebug() << "IM TREE!!!!!";
	qDebug() << "IM TREE!!!!!";
	qDebug() << "IM TREE!!!!!";
	for (int i = 0; i < nowLayerCloud->size(); ++i)
	{
		nowLayerCloud->points[i].r = 255;
		nowLayerCloud->points[i].g = 255;
		nowLayerCloud->points[i].b = 255;
	}
	qDebug() << "IM CLUSTING!!!!!";
	qDebug() << "IM CLUSTING!!!!!";
	qDebug() << "IM CLUSTING!!!!!";
	qDebug() << "IM CLUSTING!!!!!";
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

void print4x4Matrix(const Eigen::Matrix4d & matrix)    //打印旋转矩阵和平移矩阵
{
	printf("Rotation matrix :\n");
	printf("    | %6.3f %6.3f %6.3f | \n", matrix(0, 0), matrix(0, 1), matrix(0, 2));
	printf("R = | %6.3f %6.3f %6.3f | \n", matrix(1, 0), matrix(1, 1), matrix(1, 2));
	printf("    | %6.3f %6.3f %6.3f | \n", matrix(2, 0), matrix(2, 1), matrix(2, 2));
	printf("Translation vector :\n");
	printf("t = < %6.3f, %6.3f, %6.3f >\n\n", matrix(0, 3), matrix(1, 3), matrix(2, 3));
}
pcl::visualization::PCLVisualizer *p;
int vp_1, vp_2;
void showCloudsRight(const PointCloud<PointNormal>::Ptr cloud_target, const PointCloud<PointNormal>::Ptr cloud_source)
{
	p->removePointCloud("source");
	p->removePointCloud("target");


	PointCloudColorHandlerGenericField<PointNormal> tgt_color_handler(cloud_target, "curvature");
	if (!tgt_color_handler.isCapable())
		PCL_WARN("Cannot create curvature color handler!");

	PointCloudColorHandlerGenericField<PointNormal> src_color_handler(cloud_source, "curvature");
	if (!src_color_handler.isCapable())
		PCL_WARN("Cannot create curvature color handler!");


	p->addPointCloud(cloud_target, tgt_color_handler, "target", vp_2);
	p->addPointCloud(cloud_source, src_color_handler, "source", vp_2);

	p->spinOnce();
}
void showCloudsLeft(const PointCloud<PointXYZRGB>::Ptr cloud_target, const PointCloud<PointXYZRGB>::Ptr cloud_source)
{
	p->removePointCloud("vp1_target");
	p->removePointCloud("vp1_source");

	PointCloudColorHandlerCustom<PointXYZRGB> tgt_h(cloud_target, 0, 255, 0);
	PointCloudColorHandlerCustom<PointXYZRGB> src_h(cloud_source, 255, 0, 0);
	p->addPointCloud(cloud_target, tgt_h, "vp1_target", vp_1);
	qDebug() << cloud_target->size();
	p->addPointCloud(cloud_source, src_h, "vp1_source", vp_1);
	qDebug() << cloud_source->size();
	PCL_INFO("Press q to begin the registration.\n");
	p->spin();
}
void pairAlign(const PointCloud<PointXYZRGB>::Ptr sourceCloud,
	const PointCloud<PointXYZRGB>::Ptr targetCloud,
	PointCloud<PointXYZRGB>::Ptr output,
	Eigen::Matrix4f &final_transform, bool downsample = false) {

	PointCloud<PointXYZRGB>::Ptr src(new PointCloud<PointXYZRGB>);
	PointCloud<PointXYZRGB>::Ptr tgt(new PointCloud<PointXYZRGB>);
	
	//=============================
	//減少點雲
	qDebug() << "VoxelGrid";
	VoxelGrid<PointXYZRGB> grid;
	const float leaf = 0.002f;
	grid.setLeafSize(leaf, leaf, leaf);
	grid.setInputCloud(targetCloud);
	grid.filter(*tgt);
	grid.setInputCloud(sourceCloud);
	grid.filter(*src);

	//估計法線
	qDebug() << "Normal";
	NormalEstimation<PointXYZRGB, pcl::PointNormal> n;
	PointCloud<pcl::PointNormal>::Ptr target_normal(new PointCloud<pcl::PointNormal>);
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(targetCloud);
	n.setInputCloud(tgt);
	n.setSearchMethod(tree);
	//n.setRadiusSearch(0.05);
	n.setKSearch(30);
	n.compute(*target_normal);
	pcl::copyPointCloud(*tgt, *target_normal);
	qDebug() << target_normal->size();

	NormalEstimation<PointXYZRGB, pcl::PointNormal> n2;
	PointCloud<pcl::PointNormal>::Ptr src_normal(new PointCloud<pcl::PointNormal>);
	search::KdTree<PointXYZRGB>::Ptr tree2(new search::KdTree<PointXYZRGB>);
	tree2->setInputCloud(sourceCloud);
	n2.setInputCloud(src);
	n2.setSearchMethod(tree2);
	//n2.setRadiusSearch(0.05);
	n2.setKSearch(30);
	n2.compute(*src_normal);
	pcl::copyPointCloud(*src, *src_normal);
	qDebug() << src_normal->size();


	//-----------
	MyPointRepresentation point_representation;
	float alpha[4] = {1.0,1.0,1.0,1.0 };
	point_representation.setRescaleValues(alpha);
	//--------------------
	pcl::IterativeClosestPointNonLinear<PointNormal, PointNormal> reg;
	reg.setTransformationEpsilon(1e-10);
	reg.setMaxCorrespondenceDistance(0.01);
	/*reg.setEuclideanFitnessEpsilon(0.005);*/
	reg.setMaximumIterations(1);
	// Set the point representation
	reg.setPointRepresentation(boost::make_shared<const MyPointRepresentation>(point_representation));

	reg.setInputSource(src_normal);
	reg.setInputTarget(target_normal);

	Eigen::Matrix4f Ti = Eigen::Matrix4f::Identity(), prev, targetToSource;
	PointCloud<PointNormal>::Ptr reg_result(new PointCloud<PointNormal>);
	reg_result= src_normal;
	for (int i = 0; i <30; ++i)
	{
		PCL_INFO("Iteration Nr. %d.\n", i);

		// save cloud for visualization purpose
		src_normal = reg_result;
		qDebug() <<"src_normal"<< src_normal->size();

		// Estimate
		reg.setInputSource(src_normal);
		qDebug() << "align...";
		reg.align(*reg_result);
		qDebug ()<< "align_normal" << reg_result->size();

		//accumulate transformation between each Iteration
		Ti = reg.getFinalTransformation() * Ti;

		//if the difference between this transformation and the previous one
		//is smaller than the threshold, refine the process by reducing
		//the maximal correspondence distance
		if (std::abs((reg.getLastIncrementalTransformation() - prev).sum()) < reg.getTransformationEpsilon())
			reg.setMaxCorrespondenceDistance(reg.getMaxCorrespondenceDistance() - 0.005);

		prev = reg.getLastIncrementalTransformation();

		// visualize current state
		showCloudsRight(target_normal, src_normal);
	}

	//
  // Get the transformation from target to source
	targetToSource = Ti.inverse();

	//
	// Transform target back in source frame
	pcl::transformPointCloud(*targetCloud, *output, targetToSource);

	p->removePointCloud("source");
	p->removePointCloud("target");

	PointCloudColorHandlerCustom<PointXYZRGB> cloud_tgt_h(output, 0, 255, 0);
	PointCloudColorHandlerCustom<PointXYZRGB> cloud_src_h(sourceCloud, 255, 0, 0);
	p->addPointCloud(output, cloud_tgt_h, "target", vp_2);
	p->addPointCloud(sourceCloud, cloud_src_h, "source", vp_2);

	PCL_INFO("Press q to continue the registration.\n");
	p->spin();

	p->removePointCloud("source");
	p->removePointCloud("target");

	//add the source to the transformed target
	*output += *sourceCloud;

	final_transform = targetToSource;
}

void CloudPoints_Tools::registrationClouds(vector<PointCloud<PointXYZRGB>::Ptr> clouds) {
	if (clouds.size() < 2)
		return;
	////==============
	//Target為移動目標
	PointCloud<PointXYZRGB>::Ptr targetCloud(new PointCloud<PointXYZRGB>);
	targetCloud = clouds[1]->makeShared();
	PointCloud<PointXYZRGB>::Ptr sourceCloud(new PointCloud<PointXYZRGB>);
	sourceCloud = clouds[0]->makeShared();
	////----------濾除離群點---------
	//VoxelGrid<PointXYZRGB> grid;
	//const float leaf = 0.0008f;
	//grid.setLeafSize(leaf, leaf, leaf);
	//grid.setInputCloud(targetCloud);
	//grid.filter(*targetCloud);

	//grid.setLeafSize(leaf, leaf, leaf);
	//grid.setInputCloud(sourceCloud);
	//grid.filter(*sourceCloud);
	//PointCloud<PointXYZRGB>::Ptr outFile_source(new PointCloud<PointXYZRGB>);
	//PointCloud<PointXYZRGB>::Ptr outFile_target(new PointCloud<PointXYZRGB>);

	//pcl::StatisticalOutlierRemoval<PointXYZRGB> sor;
	//sor.setInputCloud(sourceCloud);
	//sor.setMeanK(500);
	//sor.setStddevMulThresh(0.01);
	//sor.filter(*outFile_source);

	//sor.setInputCloud(targetCloud);	
	//sor.filter(*outFile_target);
	
	//============================
	p = new pcl::visualization::PCLVisualizer();
	p->createViewPort(0.0, 0, 0.5, 1.0, vp_1);
	p->createViewPort(0.5, 0, 1.0, 1.0, vp_2);

	Eigen::Matrix4f GlobalTransform = Eigen::Matrix4f::Identity(), pairTransform;
	showCloudsLeft(sourceCloud, targetCloud);
	//--------------------------- 
	PointCloud<PointXYZRGB>::Ptr temp(new PointCloud<PointXYZRGB>);
	//可以做到極度貼近，但ALIGN會出錯
	pairAlign(sourceCloud, targetCloud, temp, pairTransform, true);
	PointCloud<PointXYZRGB>::Ptr result(new PointCloud<PointXYZRGB>);
	pcl::transformPointCloud(*targetCloud, *result, pairTransform);
	GlobalTransform *= pairTransform;
	//result->clear();
	//pcl::transformPointCloud(*sourceCloud, *result, GlobalTransform);
	pcl::io::savePCDFileASCII("D:/transformed.pcd", *result);

	qDebug() << result->size();
	//--------------
}
