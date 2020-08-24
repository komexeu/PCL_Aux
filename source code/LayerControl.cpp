#include "LayerControl.h"
#include<qdebug.h>

bool LayerController::AddLayer(QString layerName, std::string layerForParent, layerInformation layerInform) {
	std::string s = layerName.toStdString();
	QString LayerStringID = QString::fromStdString(layerForParent) + layerName;

	for (int i = 0; i < layer_information_->size(); i++)
	{
		if (layer_information_->at(i).object_name_ == s && layer_information_->at(i).parent_layer_name_ == layerForParent)
			return(false);
	}

	QFont font;
	font.setFamily(QString::fromUtf8("Taipei Sans TC Beta"));
	font.setPointSize(8);

	QPushButton* btn_layer;
	btn_layer = new QPushButton(nullptr);
	btn_layer->setObjectName(LayerStringID);
	btn_layer->setText(layerName);
	btn_layer->setFont(font);
	btn_layer->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255);"));
	btn_layer->setFlat(true);
	layer_layout_->setWidget(layer_layout_->count(), QFormLayout::SpanningRole, btn_layer);

	layer_signalmapper_->setMapping(btn_layer, LayerStringID);
	QObject::connect(btn_layer, SIGNAL(clicked()), layer_signalmapper_, SLOT(map()));

	layer_information_->push_back(layerInform);
	return(true);
}

void LayerController::DeleteLayer(QString LayerObjectName) {
	std::string s = LayerObjectName.toStdString();
	int index;
	for (int i = 0; i < layer_information_->size(); i++)
	{
		QString LayerID = QString::fromStdString(layer_information_->at(i).object_name_);
		if (LayerID == LayerObjectName) {
			index = i;
			break;
		}
	}
	layer_layout_->removeRow(index);
	layer_information_->erase(layer_information_->begin() + index);
	//--------------
	/*std::vector<std::string>::iterator it = std::find(layer_name_vec_.begin(), layer_name_vec_.end(), s);
	int index = std::distance(layer_name_vec_.begin(), it);

	if (it != layer_name_vec_.end())
		layer_name_vec_.erase(it);
	else
		return;

	delete layer_layout_->takeAt(index)->widget();
	qDebug() << layer_name_vec_.size();*/
}

layerInformation LayerController::RenameLayer(QString LayerObjectName, QString Rename, layerInformation originalInformation)
{
	std::string s = LayerObjectName.toStdString();
	QString LayerStringID;
	int index;
	for (int i = 0; i < layer_information_->size(); i++)
	{
		LayerStringID = QString::fromStdString(layer_information_->at(i).object_name_);
		qDebug() << "LayerID" << LayerStringID;
		if (LayerStringID == LayerObjectName) {
			index = i;
			LayerStringID = QString::fromStdString(layer_information_->at(index).parent_layer_name_) + Rename;
			break;
		}
		if (i == layer_information_->size() - 1)
		{
			qDebug() << "NO FOUND!";
			layerInformation noFound{ "NOFOUND__" ,NULL,"NOFOUND__" ,"NOFOUND__" };
			return noFound;
		}
	}

	layer_layout_->removeRow(index);
	//layer_information_->erase(layer_information_->begin() + index);

	layer_information_->at(index) = originalInformation;
	layer_information_->at(index).layer_name_ = Rename.toStdString();
	layer_information_->at(index).object_name_ = LayerStringID.toStdString();
	//originalInformation = layer_information_->at(index);

	QFont font;
	font.setFamily(QString::fromUtf8("Taipei Sans TC Beta"));
	font.setPointSize(8);

	QPushButton* btn_layer;
	btn_layer = new QPushButton(nullptr);
	btn_layer->setObjectName(LayerStringID);
	btn_layer->setText(Rename);
	btn_layer->setFont(font);
	btn_layer->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255);"));
	btn_layer->setFlat(true);
	layer_layout_->insertRow(index, btn_layer);
	//layer_layout_->setWidget(index, QFormLayout::SpanningRole, btn_layer);

	layer_signalmapper_->setMapping(btn_layer, LayerStringID);
	//À³¸Ó­nremove sender
	QObject::connect(btn_layer, SIGNAL(clicked()), layer_signalmapper_, SLOT(map()));
	return layer_information_->at(index);


	/*for (int i = layer_name_vec_.size() - 1; i >= 0; --i)
	{
		delete layer_layout_->takeAt(i)->widget();
		layer_name_vec_.pop_back();
	}*/
}
//--------class for tree---------
bool TreeLayerController::AddLayer(QString layerName, PointCloud<PointXYZRGB>::Ptr cloud) {
	QIcon icon;
	icon.addFile(QString::fromUtf8("C:/Users/USER/Downloads/cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);

	QStandardItem *qitem = new QStandardItem(layerName);
	QVariant itemCloud;
	itemCloud.setValue(cloud);
	qitem->setData(itemCloud);
	qitem->setIcon(icon);
	data_model_->appendRow(qitem);
	return(true);
}

//0 for new importdata
//1 for seg data
bool TreeLayerController::AddLayer(QString layerName, PointCloud<PointXYZRGB>::Ptr cloud, QModelIndex selectId) {
	QIcon icon;
	icon.addFile(QString::fromUtf8("./cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);

	QStandardItem *qitem = new QStandardItem(layerName);
	QVariant itemCloud;
	itemCloud.setValue(cloud);
	qitem->setData(itemCloud);
	qitem->setIcon(icon);

	data_model_->itemFromIndex(selectId)->appendRow(qitem);
	return(true);
}
//--------tree for struct data-----------
bool TreeLayerController::AddLayer(QString layerName, complax_cloudInformation complaxInform, QModelIndex selectId) {
QIcon icon;
	icon.addFile(QString::fromUtf8("./cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);

	QStandardItem *qitem = new QStandardItem(layerName);
	QVariant itemCloud;
	itemCloud.setValue(complaxInform);
	qitem->setData(itemCloud);
	qitem->setIcon(icon);

	data_model_->itemFromIndex(selectId)->appendRow(qitem);
	return(true);
}