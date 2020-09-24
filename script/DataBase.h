#pragma once

//stl
#include<vector>
#include<string>

//qt
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qslider.h>
#include <QtWidgets/qspinbox.h>
#include <qsignalmapper.h>

#include <qstandarditemmodel.h>

//pcl
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <qtoolbutton.h>



using namespace pcl;

//layerInformation{parent_layer_name_,layer_cloud_,layer_name_,object_name_}
struct layerInformation {
	std::string parent_layer_name_;
	PointCloud<PointXYZRGB>::Ptr layer_cloud_;
	std::string layer_name_;
	std::string object_name_;
};

class DataBase
{
public:
	//tree view data
	//圖層資料
	QStandardItemModel* standardModel ;
	//QList<QStandardItem*>* dataItemList=new QList<QStandardItem*>;
	QStandardItem* item;
	//---------------------
	////圖層管理員，儲存所有圖層資訊
	//std::vector<layerInformation>* Layer_Manager=new std::vector<layerInformation>;
	////目前視窗上點雲資訊
	//layerInformation ViewCloudInformation;
	//上排工具列
	std::vector<QToolButton*> ToolButtonManager;
	
	//視角
	int viewMode = 0;
	//被畫筆選中的ID
	//std::vector<int> circle_selectID;
	//顯示視窗
	boost::shared_ptr<visualization::PCLVisualizer> viewer;
	//圖層點雲暫存組
	std::vector<PointCloud<PointXYZRGB>::Ptr> SegClouds;

	//已選點雲
	PointCloud<PointXYZRGB>::Ptr Selected_cloud;
	//已選點雲id
	//std::vector<int> Selected_ID;


	 std::map<int, PointXYZRGB> select_map;

	//儲存所有Layer的SLOT ID
	QSignalMapper *layerMapper;

	//QSlider *ClusterSlider;
	//QSpinBox *ClusterSpinBox;

	bool keyBoard_ctrl = false;
	bool keyBoard_alt = false;
	bool BrushSelectMode = false;
	bool BrushKDEnd = true;
	bool AreaSelectMode = false;
	bool BrushMode = false;
	//筆刷大小
	float brush_radius = 0.5;
	//點雲密度(點雲間平均距離)
	float nowCloud_avg_distance=0;

	int cameraView = 0;
};

namespace Ui_DataBase {
	class  dataBase :public DataBase {};
}