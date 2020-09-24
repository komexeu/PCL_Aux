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
	//�ϼh���
	QStandardItemModel* standardModel ;
	//QList<QStandardItem*>* dataItemList=new QList<QStandardItem*>;
	QStandardItem* item;
	//---------------------
	////�ϼh�޲z���A�x�s�Ҧ��ϼh��T
	//std::vector<layerInformation>* Layer_Manager=new std::vector<layerInformation>;
	////�ثe�����W�I����T
	//layerInformation ViewCloudInformation;
	//�W�Ƥu��C
	std::vector<QToolButton*> ToolButtonManager;
	
	//����
	int viewMode = 0;
	//�Q�e���襤��ID
	//std::vector<int> circle_selectID;
	//��ܵ���
	boost::shared_ptr<visualization::PCLVisualizer> viewer;
	//�ϼh�I���Ȧs��
	std::vector<PointCloud<PointXYZRGB>::Ptr> SegClouds;

	//�w���I��
	PointCloud<PointXYZRGB>::Ptr Selected_cloud;
	//�w���I��id
	//std::vector<int> Selected_ID;


	 std::map<int, PointXYZRGB> select_map;

	//�x�s�Ҧ�Layer��SLOT ID
	QSignalMapper *layerMapper;

	//QSlider *ClusterSlider;
	//QSpinBox *ClusterSpinBox;

	bool keyBoard_ctrl = false;
	bool keyBoard_alt = false;
	bool BrushSelectMode = false;
	bool BrushKDEnd = true;
	bool AreaSelectMode = false;
	bool BrushMode = false;
	//����j�p
	float brush_radius = 0.5;
	//�I���K��(�I���������Z��)
	float nowCloud_avg_distance=0;

	int cameraView = 0;
};

namespace Ui_DataBase {
	class  dataBase :public DataBase {};
}