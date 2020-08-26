#pragma once

#include "DataBase.h"

//pcl
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

//stl
#include <vector>

//qt
#include <qpushbutton.h>
#include <qfont.h>
#include <qstring.h>
#include <qwidget.h>
#include <qformlayout.h>
#include <qsignalmapper.h>
#include <qstandarditemmodel.h>

#include <qdebug.h>

struct complax_cloudInformation
{
	pcl::PointCloud<PointXYZRGB>::Ptr cloud_data;
	std::vector<int> points_id;
};

Q_DECLARE_METATYPE(pcl::PointCloud<PointXYZRGB>::Ptr)
Q_DECLARE_METATYPE(complax_cloudInformation)


//---------圖層控制類----------
//新增/刪除圖層
//可見/隱藏圖層
class ILayerControl
{
public:
	//virtual bool AddLayer() = 0;
	//virtual bool AddLayer(QString layerName) = 0;	
	//virtual bool AddLayer(QString layerName, std::string layerForParent, layerInformation layerInform) = 0;
	//virtual void DeleteLayer(QString LayerObjectName) = 0;
	//virtual layerInformation RenameLayer(QString LayerObjectName, QString Rename, layerInformation originalInformation) = 0;
};

class LayerController :public ILayerControl
{
public:

	LayerController(QSignalMapper* LayerMapper, QFormLayout* LayerLayout, std::vector<layerInformation>* LayerManager) :
		ILayerControl(), layer_information_(), layer_signalmapper_(), layer_layout_()
	{
		layer_signalmapper_ = LayerMapper;
		layer_layout_ = LayerLayout;
		layer_information_ = LayerManager;
	}

	virtual bool AddLayer(QString layerName, std::string layerForParent, layerInformation layerInform);
	void DeleteLayer(QString LayerObjectName);
	layerInformation RenameLayer(QString LayerObjectName, QString Rename, layerInformation originalInformation);

private:
	QSignalMapper* layer_signalmapper_;
	std::vector<layerInformation>* layer_information_;
	QFormLayout* layer_layout_;
};

class TreeLayerController :public ILayerControl {
public:
	TreeLayerController(QStandardItemModel* model) :data_model_() {
		data_model_ = model;
	}
	//this is override
	virtual bool AddLayer(QString layerName, PointCloud<PointXYZRGB>::Ptr cloud);
	virtual bool AddLayer(QString layerName, PointCloud<PointXYZRGB>::Ptr cloud, QModelIndex selectId);
	virtual bool TreeLayerController::AddLayer(QString layerName, complax_cloudInformation complaxInform, QModelIndex selectId);
private:
	QStandardItemModel* data_model_;
};

