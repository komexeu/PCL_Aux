#include "QT_BaseMainWindow.h"

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/search/kdtree.h>
#include <pcl/filters/filter.h>

#include <qdebug.h>

#include<qsignalmapper.h>
//test
#include <qtreeview.h>
#include <qstandarditemmodel.h>
//--------------------
#include "realsense_control.h"
#include "CloudPoints_IO.h"
#include "LayerControl.h"
#include <omp.h>
//test
#include "VTK_AuxStyle.h"
#include <vtkSmartPointer.h>
#include <vtkImageActor.h>
#include <vtkImageMapper.h>
#include <vtkImageMapper3D.h>
#include <vtkJPEGReader.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkProp3DCollection.h>
#include <vtkNamedColors.h>

#include "InteractorStyle_override.h"

using namespace pcl;

QT_BaseMainWindow::QT_BaseMainWindow(QWidget *parent) :QMainWindow(parent)
{
	ui.setupUi(this);
	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->setWindowTitle("AI 3D Mark");
	//---init data---
	data.Selected_cloud.reset(new PointCloud<PointXYZRGB>);
	data.layerMapper = new QSignalMapper();
	//--------visualization init--------------
	pcl::visualization::PCLVisualizerInteractorStyle *sty_ovr = InteractorStyle_override::New();
	int gc; char** gv;
	data.viewer.reset(new pcl::visualization::PCLVisualizer(gc, gv, "viewer", sty_ovr, false));
	data.viewer->setBackgroundColor(0.8, 0.8, 0.8);
	ui.qvtkWidget->SetRenderWindow(data.viewer->getRenderWindow());
	data.viewer->setupInteractor(ui.qvtkWidget->GetInteractor(), ui.qvtkWidget->GetRenderWindow());
	//---------------Segmentation slider-------------
	ui.seg_horizontalSlider->setStyleSheet("QSlider::sub-page:Horizontal { background-color: #D1466C; }"
		"QSlider::add-page:Horizontal { background-color: #F4A9C9; }"
		"QSlider::groove:Horizontal { background: transparent; height:4px; }"
		"QSlider::handle:Horizontal { width:10px; border-radius:5px; background:#D1466C; margin: -5px 0px -5px 0px; }");
	ui.seg_horizontalSlider->setRange(1, 400);
	ui.seg_spinBox->setRange(1, 400);
	//---------------Brush slider-------------
	ui.Brush_ClusterSlider->setRange(1, 100);
	ui.Brush_ClusterSlider->setStyleSheet("QSlider::sub-page:Horizontal { background-color: #D1466C; }"
		"QSlider::add-page:Horizontal { background-color: #F4A9C9; }"
		"QSlider::groove:Horizontal { background: transparent; height:4px; }"
		"QSlider::handle:Horizontal { width:10px; border-radius:5px; background:#D1466C; margin: -5px 0px -5px 0px; }");
	ui.Brush_ClusterSpinBox->setRange(1, 100);
	//---------------smooth slider-------------
	qDebug() << "smooth 1";
	ui.smooth_horizontalSlider->setRange(1, 100);
	ui.smooth_horizontalSlider->setStyleSheet("QSlider::sub-page:Horizontal { background-color: #D66251; }"
		"QSlider::add-page:Horizontal { background-color: rgb(235,158,134); }"
		"QSlider::groove:Horizontal { background: transparent; height:4px; }"
		"QSlider::handle:Horizontal { width:10px; border-radius:5px; background:#D66251; margin: -5px 0px -5px 0px; }");
	//SpinBox SetRange 前後需有QDebug，否則無法編譯完成(BUG?)
	qDebug() << "smooth 2";
	ui.smooth_spinBox->setRange(1, 100);
	//-------------toolBar-----------
	QT_ToolBarManager toolManager(this);
	data.ToolButtonManager.push_back(toolManager.AddToolButton("./PCLAuxilary_pic_sorce/images.png", "deleteLayer"));
	QObject::connect(data.ToolButtonManager[0], SIGNAL(clicked()), this, SLOT(Tree_deleteLayer()));
	data.ToolButtonManager.push_back(toolManager.AddToolButton("./PCLAuxilary_pic_sorce/rename_icon.png", "renameLayer"));
	QObject::connect(data.ToolButtonManager[1], SIGNAL(clicked()), this, SLOT(ToolBTN_renameLayer()));
	data.ToolButtonManager.push_back(toolManager.AddToolButton("./PCLAuxilary_pic_sorce/cursor1-2.png", "brush"));
	data.ToolButtonManager.push_back(toolManager.AddToolButton("./PCLAuxilary_pic_sorce/AreaSelect.png", "areaSelect"));
	//------treeView-----------------
	data.standardModel = new QStandardItemModel(ui.treeView);
	ui.treeView->setHeaderHidden(true);
	ui.treeView->setStyleSheet("color:rgb(255,255,255);"
		"background-color: #706073;");
	data.item = data.standardModel->invisibleRootItem();
	ui.treeView->setModel(data.standardModel);
	ui.treeView->expandAll();
	//---------------------------------------CONNECT------------------------------------------------
	//-------realsense work----------
	QObject::connect(ui.pushButton_5, SIGNAL(clicked()), this, SLOT(realsense_getpoints()));
	//-------tree click connect-------
	QItemSelectionModel *selectionModel = ui.treeView->selectionModel();
	connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
		SLOT(Tree_selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
	//-------cloud deal-----------
	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(Tree_importCloud()));
	QObject::connect(ui.pushButton_6, SIGNAL(clicked()), this, SLOT(Tree_exportCloud()));
	QObject::connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(Tree_Smooth()));
	//--------layer button clicked----------
	//QObject::connect(data.layerMapper, SIGNAL(mapped(QString)), this, SLOT(BTN_LayerClicked(QString)));
	//----------segment---------------
	QObject::connect(ui.pushButton_4, SIGNAL(clicked()), this, SLOT(Tree_UserSegmentation()));
	//------AUTO segment------------
	QObject::connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(Tree_confirmSegCloud()));
	QObject::connect(ui.seg_horizontalSlider, SIGNAL(valueChanged(int)), ui.seg_spinBox, SLOT(setValue(int)));
	QObject::connect(ui.seg_spinBox, SIGNAL(valueChanged(int)), ui.seg_horizontalSlider, SLOT(setValue(int)));
	QObject::connect(ui.seg_spinBox, SIGNAL(valueChanged(int)), this, SLOT(Tree_Slider_PreSegCloud()));
	//----BRUSH---------
	QObject::connect(ui.Brush_ClusterSlider, SIGNAL(valueChanged(int)), ui.Brush_ClusterSpinBox, SLOT(setValue(int)));
	QObject::connect(ui.Brush_ClusterSpinBox, SIGNAL(valueChanged(int)), ui.Brush_ClusterSlider, SLOT(setValue(int)));
	QObject::connect(ui.Brush_ClusterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(Brush_change()));
	//----SMOOTH SLIDER---------
	QObject::connect(ui.smooth_horizontalSlider, SIGNAL(valueChanged(int)), ui.smooth_spinBox, SLOT(setValue(int)));
	QObject::connect(ui.smooth_spinBox, SIGNAL(valueChanged(int)), ui.smooth_horizontalSlider, SLOT(setValue(int)));
	//----------event callback----------
	data.viewer->registerAreaPickingCallback(&Area_PointCloud_Selector);
	data.viewer->registerMouseCallback(&cursor_BrushSelector);
	data.viewer->registerKeyboardCallback(&KeyBoard_eventController);
	//test	
	PointCloud<PointXYZRGB>::Ptr nullCloud(new PointCloud<PointXYZRGB>);
	data.viewer->addPointCloud(nullCloud, "cld");
	data.viewer->addPointCloud(nullCloud, "Red_ChosenPoints");
	data.viewer->addPointCloud(nullCloud, "White_BrushCursorPoints");
	data.viewer->addPointCloud(nullCloud, "segment_colorPoints");
	ui.retranslateUi(this);
	ui.qvtkWidget->update();
}

void QT_BaseMainWindow::ViewCloudUpdate(PointCloud<PointXYZRGB>::Ptr updateCloud, bool resetCamera) {
	data.viewer->removePointCloud("cld");
	data.viewer->addPointCloud(updateCloud, "cld");
	if (resetCamera)
		data.viewer->resetCamera();
	ui.qvtkWidget->update();
}

void QT_BaseMainWindow::RedSelectClear() {
	data.select_map.clear();
	data.Selected_cloud->clear();
	/*data.Selected_cloud->clear();
	data.viewer->updatePointCloud(data.Selected_cloud, "Red_ChosenPoints");*/
}

void QT_BaseMainWindow::initModes() {
	QIcon icon;

	data.BrushMode = false;
	icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);
	data.ToolButtonManager[2]->setIcon(icon);

	PointCloud<PointXYZRGB>::Ptr nullCloud(new PointCloud<PointXYZRGB>);
	data.viewer->removePointCloud("White_BrushCursorPoints");
	data.viewer->addPointCloud(nullCloud, "White_BrushCursorPoints");
}

#include "PCL_AUX_Button.h"

//SLOTS
void QT_BaseMainWindow::realsense_getpoints() {
	PCL_AUX_Button *w = new PCL_AUX_Button(ui.formWidget);
	w->setObjectName(QString::fromUtf8("ww"));
	w->setMouseTracking(true);
	w->setText("JOOOO");
	w->setFramesNum(9);
	w->show();

	//QObject::connect(w, SIGNAL(clicked()), this, SLOT(w->PLAY()));

	/*QPropertyAnimation *animation = new QPropertyAnimation(ui.pushButton_5,"alpha");
	animation->setDuration(1000);
	animation->setKeyValueAt(0, 255);
	animation->setKeyValueAt(0.5, 100);
	animation->setKeyValueAt(1, 255);
	animation->setLoopCount(-1);
	animation->start();*/


	//m_haloAnimation->start();	

	//================================
	/*ui.label->setText("Picking cloud...");
	WinExec("cmd /c start ./realsense_externalExe/realsense_externalExe.exe", SW_HIDE);
	ui.label->setText("END");*/
	//======================
	/*vector<PointCloud<PointXYZRGB>::Ptr> clouds;
	clouds.resize(2);
	for (int i = 0; i < 2; i++) {
		clouds[i].reset(new PointCloud<PointXYZRGB>);
	}

	qDebug() << "source->" << io::loadPCDFile<PointXYZRGB>("D:/testDataForAlign/0001.pcd", *clouds[0]);
	qDebug() << "target->" << io::loadPCDFile<PointXYZRGB>("D:/testDataForAlign/0002.pcd", *clouds[1]);

	CloudPoints_Tools cpTools;
	cpTools.registrationClouds(clouds);*/
	//=================
	//realsense_control *rs_con = new realsense_control();
	//rs_con->initRs();
	//PointCloud<PointXYZRGB>::Ptr nowCloud = rs_con->Realsense_LinkCloud()->makeShared();

	//if (nowCloud->size() != 0)
	//{
	//	std::vector<int> k_indices;
	//	std::vector<float> k_sqr_distances;
	//	int n = 0;
	//	double norm = 0;
	//	int searched_points = 0;
	//	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	//	tree->setInputCloud(nowCloud);
	//	qDebug() << "001";
	//	//取1000點做平均取距離
	//	for (int i = 0; i < (nowCloud->size() >= 1000 ? 1000 : nowCloud->size()); ++i)
	//	{
	//		n = tree->nearestKSearch(i, 2, k_indices, k_sqr_distances);
	//		if (n == 2)
	//		{
	//			norm += sqrt(k_sqr_distances[1]);
	//			//qDebug() << norm;
	//			++searched_points;
	//		}
	//	}
	//	qDebug() << "002";
	//	if (searched_points != 0) {
	//		data.nowCloud_avg_distance = norm /= searched_points;
	//		data.brush_radius = data.nowCloud_avg_distance * 50;
	//	}
	//	else {
	//		data.nowCloud_avg_distance = 0;
	//		data.brush_radius = 0;
	//	}
	//	qDebug() << nowCloud->size();
	//	ui.label->setText("success.");

	//	data.viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//	data.viewer->setBackgroundColor(0.7, 0.7, 0.7);
	//	ui.qvtkWidget->SetRenderWindow(data.viewer->getRenderWindow());
	//	data.viewer->setupInteractor(ui.qvtkWidget->GetInteractor(), ui.qvtkWidget->GetRenderWindow());

	//	ViewCloudUpdate(nowCloud, true);
	//}
	//else
	//{
	//	ui.label->setText("No Device or No Data");
	//}
	////-------------------
	////rs_con->initRs();
	//////bool result = rs_con->rsShow();
	////qDebug() << result;
	////qDebug() << result;
	////qDebug() << result;
	////qDebug() << result;
	////qDebug() << result;
	////if (result)
	////{
	////	ui.label->setText("success.");
	////}
	////else
	////{
	////	ui.label->setText( "NO DEVICE!");
	////}
	//delete rs_con;
}

void QT_BaseMainWindow::Tree_selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/) {
	RedSelectClear();
	data.SegClouds.clear();
	initModes();

	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	if (index.row() == -1) {
		PointCloud<PointXYZRGB>::Ptr nullCloud(new PointCloud<PointXYZRGB>);
		ViewCloudUpdate(nullCloud, true);
		return;
	}

	int size = 0;
	PointCloud<PointXYZRGB>::Ptr nowCloud(new PointCloud<PointXYZRGB>);
	if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "complax_cloudInformation")
	{
		size = data.standardModel->itemFromIndex(index)->data().value<complax_cloudInformation>().cloud_data->size();
		nowCloud = data.standardModel->itemFromIndex(index)->data().value<complax_cloudInformation>().cloud_data;
	}
	else if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr")
	{
		size = data.standardModel->itemFromIndex(index)->data().value<PointCloud<PointXYZRGB>::Ptr>()->size();
		nowCloud = data.standardModel->itemFromIndex(index)->data().value<PointCloud<PointXYZRGB>::Ptr>();
	}
	//data.Selected_cloud->resize(nowCloud->size());
	data.Selected_cloud->clear();
	data.Selected_cloud = nowCloud->makeShared();
	ViewCloudUpdate(nowCloud, true);
	qDebug() << "ViewCloudUpdate";
	QString selectedText =/*index.data(Qt::DisplayRole).toString() + " have "+*/QString::fromStdString(std::to_string(size)) + " points.";
	ui.label->setText(selectedText);
	//-------------
	std::vector<int> k_indices;
	std::vector<float> k_sqr_distances;
	int n = 0;
	double norm = 0;
	int searched_points = 0;
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(nowCloud);
	qDebug() << "001";
	//取1000點做平均取距離
	for (int i = 0; i < (nowCloud->size() >= 1000 ? 1000 : nowCloud->size()); ++i)
	{
		n = tree->nearestKSearch(i, 2, k_indices, k_sqr_distances);
		if (n == 2)
		{
			double n = sqrt(k_sqr_distances[1]);
			if (n < VTK_DOUBLE_MIN || n>VTK_DOUBLE_MAX)
				continue;
			norm += n;
			//qDebug() << norm;
			++searched_points;
		}
	}

	if (searched_points != 0) {
		data.nowCloud_avg_distance = norm / searched_points;
		data.brush_radius = data.nowCloud_avg_distance * 50;
	}
	else {
		data.nowCloud_avg_distance = 0;
		data.brush_radius = 0;
	}

	qDebug() << "AVGDISTANCE" << data.nowCloud_avg_distance;
	qDebug() << "FINISH!";
}
//-----------
#include<qinputdialog.h>
void QT_BaseMainWindow::Tree_importCloud() {
	RedSelectClear();
	initModes();

	qDebug() << "HIIII for import";
	CloudPoints_IO IO_Tool;
	if (!IO_Tool.CloudImport()) {
		qDebug() << "ERROW!!!!!";
		return;
	}

	qDebug() << "HIIII for import END";
	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
		tr("Layer name:"), QLineEdit::Normal,
		QString::fromStdString(IO_Tool.file_name_), &ok);
	if (ok && !text.isEmpty()) {
		std::string BaseLayerName = text.toStdString();
		std::string objName = "NONE" + text.toStdString();
		layerInformation layer_inform{ "NONE", IO_Tool.import_cloud_->makeShared(),BaseLayerName  ,objName };
		TreeLayerController *tree_layerController = new TreeLayerController(data.standardModel);
		if (!tree_layerController->AddLayer(text, IO_Tool.import_cloud_->makeShared()))
			return;
		qDebug() << "Here is in importCloud!";
	}
}
//---------------
void  QT_BaseMainWindow::roll(QList<QModelIndex> &indexList, QModelIndex index) {
	if (data.standardModel->itemFromIndex(index)->hasChildren())
	{
		int childCount = data.standardModel->itemFromIndex(index)->rowCount();
		for (int i = 0; i < childCount; ++i)
		{
			QModelIndex idx = data.standardModel->itemFromIndex(index)->child(i, 0)->index();
			roll(indexList, idx);
		}
	}
	else
	{
		indexList.push_back(index);
	}
}
QList<QModelIndex> QT_BaseMainWindow::GetChildrenIndex(QModelIndex index) {
	QModelIndex childIndex;
	QList<QModelIndex> indexList;
	roll(indexList, index);
	return indexList;
}
void QT_BaseMainWindow::Tree_exportCloud() {
	if (ui.treeView->selectionModel()->currentIndex().row() == -1)
		return;
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	QList<QModelIndex> childIndex = GetChildrenIndex(index);
	ui.label->setText(QString::fromStdString(std::to_string(childIndex.size())));
	vector<PointCloud<PointXYZRGB>::Ptr> children_CloudData;
	for (int i = 0; i < childIndex.size(); i++)
	{
		PointCloud<PointXYZRGB>::Ptr cld(new PointCloud<PointXYZRGB>);
		if (std::string(data.standardModel->itemFromIndex(childIndex[i])->data().typeName()) == "complax_cloudInformation")
			cld = data.standardModel->itemFromIndex(childIndex[i])->data().value<complax_cloudInformation>().cloud_data;
		else if (std::string(data.standardModel->itemFromIndex(childIndex[i])->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr")
			cld = data.standardModel->itemFromIndex(childIndex[i])->data().value<PointCloud<PointXYZRGB>::Ptr>();
		children_CloudData.push_back(cld);
	}

	RedSelectClear();
	qDebug() << "HIIII for export";
	CloudPoints_IO IO_Tool;
	if (IO_Tool.CloudExport(children_CloudData))
	{
		ui.label->setText("Export Success!");
	}
	else
	{
		ui.label->setText("Export Fail!");
	}
}
//---------------
void QT_BaseMainWindow::Tree_Slider_PreSegCloud() {
	if (ui.treeView->selectionModel()->currentIndex().row() == -1)
		return;
	data.SegClouds.clear();
	CloudPoints_Tools cpTools;
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();

	qDebug() << "1SLIDER1111111111111111111";

	PointCloud<PointXYZRGB>::Ptr database_cloud(new PointCloud<PointXYZRGB>);
	PointCloud<PointXYZRGB>::Ptr cld(new PointCloud<PointXYZRGB>);
	if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "complax_cloudInformation") {
		copyPointCloud(*data.standardModel->itemFromIndex(index)->data().value<complax_cloudInformation>().cloud_data, *database_cloud);
		copyPointCloud(*data.standardModel->itemFromIndex(index)->data().value<complax_cloudInformation>().cloud_data, *cld);
	}
	else if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr") {
		copyPointCloud(*data.standardModel->itemFromIndex(index)->data().value<PointCloud<PointXYZRGB>::Ptr>(), *database_cloud);
		copyPointCloud(*data.standardModel->itemFromIndex(index)->data().value<PointCloud<PointXYZRGB>::Ptr>(), *cld);
	}

	std::vector<PointIndices> seg_cloud_2 = cpTools.CloudSegmentation(cld, ui.seg_spinBox->value(), data.nowCloud_avg_distance);
	qDebug() << "WHITE";
	for (int i = 0; i < cld->size(); i++)
	{
		cld->points[i].r = 255;
		cld->points[i].g = 255;
		cld->points[i].b = 255;
	}
	for (vector<PointIndices>::const_iterator i = seg_cloud_2.begin(); i < seg_cloud_2.end(); i++)
	{
		int color_R = rand() % 250;
		int color_G = rand() % 250;
		int color_B = rand() % 250;
		PointCloud<PointXYZRGB>::Ptr tmp(new PointCloud<PointXYZRGB>);
		for (std::vector<int>::const_iterator j = i->indices.begin(); j < i->indices.end(); j++)
		{
			tmp->push_back(database_cloud->points[*j]);
			cld->points[*j].r = color_R;
			cld->points[*j].g = color_G;
			cld->points[*j].b = color_B;
		}
		data.SegClouds.push_back(tmp);
	}
	ViewCloudUpdate(cld, false);

	//---clear red_chosenPoints
	RedSelectClear();
}
//-----------------
void QT_BaseMainWindow::Tree_confirmSegCloud() {
	qDebug() << ui.treeView->selectionModel()->currentIndex();
	if (ui.treeView->selectionModel()->currentIndex().row() == -1)
		return;
	for (int i = 0; i < data.SegClouds.size(); ++i)
	{
		QString segLayer = ui.treeView->selectionModel()->currentIndex().data(Qt::DisplayRole).toString() + "_" + QString::fromStdString(std::to_string(i));
		TreeLayerController ly(data.standardModel);

		QModelIndex index = ui.treeView->selectionModel()->currentIndex();
		if (!ly.AddLayer(segLayer, data.SegClouds[i], index))
			return;
	}
}
//------------
void QT_BaseMainWindow::Tree_Smooth() {
	if (ui.treeView->selectionModel()->currentIndex().row() == -1)
		return;
	RedSelectClear();
	CloudPoints_Tools cpTools;
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	PointCloud<PointXYZRGB>::Ptr cld = data.standardModel->itemFromIndex(index)->data().value<PointCloud<PointXYZRGB>::Ptr>();
	qDebug() << data.nowCloud_avg_distance;
	//30為搜尋範圍，*0.5搜尋半徑
	PointCloud<PointXYZRGB>::Ptr smooth_cld = cpTools.CloudSmooth(cld, data.nowCloud_avg_distance *ui.smooth_spinBox->value() * 0.5);

	if (smooth_cld->size() > 0)
	{
		//data update
		QVariant itemCloud;
		itemCloud.setValue(smooth_cld);
		data.standardModel->itemFromIndex(index)->setData(itemCloud);

		data.Selected_cloud = smooth_cld->makeShared();
		//view update
		ViewCloudUpdate(smooth_cld, false);
	}
	else
	{
		ui.label->setText("NO DATA AFTER SMOOTH,Please set a bigger value.");
	}
}
//--------------------
void QT_BaseMainWindow::Tree_UserSegmentation() {
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();

	PointCloud<PointXYZRGB>::Ptr LayerCloud;
	if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "complax_cloudInformation")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <complax_cloudInformation>().cloud_data;
	else if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <PointCloud<PointXYZRGB>::Ptr>();

	if (index.row() == -1)
		return;
	if (data.select_map.size() > 0)
	{
		bool ok;
		QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
			tr("Layer name:"), QLineEdit::Normal,
			QDir::home().dirName(), &ok);
		if (ok && !text.isEmpty())
		{
			TreeLayerController ly(data.standardModel);
			PointCloud<PointXYZRGB>::Ptr newCloud(new PointCloud<PointXYZRGB>);

			for (map<int, PointXYZRGB>::iterator iter = data.select_map.begin(); iter != data.select_map.end(); ++iter)
				newCloud->push_back(LayerCloud->points.at(iter->first));
			
			if (!ly.AddLayer(text, newCloud->makeShared(), index))
				return;

			RedSelectClear();
		}
	}
}
//-------------
void QT_BaseMainWindow::Tree_deleteLayer() {
	if (ui.treeView->selectionModel()->currentIndex().row() == -1)
		return;
	RedSelectClear();
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	//多層刪除
	if (data.standardModel->itemFromIndex(index)->hasChildren())
		data.standardModel->itemFromIndex(index)->removeRows(0, data.standardModel->itemFromIndex(index)->rowCount());
	//沒有(上層)父類
	if (index.parent().row() == -1)
		data.standardModel->removeRow(index.row());
	else
		data.standardModel->itemFromIndex(index)->parent()->removeRow(index.row());
}

//Select For Tree
void QT_BaseMainWindow::Area_PointCloud_Selector(const pcl::visualization::AreaPickingEvent& event) {
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	if (index.row() == -1)
		return;

	PointCloud<PointXYZRGB>::Ptr LayerCloud;
	if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "complax_cloudInformation")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <complax_cloudInformation>().cloud_data;
	else if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <PointCloud<PointXYZRGB>::Ptr>();

	//AREA PICK CLOUD
	std::vector<int> foundPointID;
	if (event.getPointsIndices(foundPointID) <= 0) {
		//qDebug() << "ZERO" << foundPointID.size();
		////快速多次選取閃退(原因為更新點雲ViewCloudUpdate)
		if (!data.select_map.empty()) {
			data.select_map.clear();
			data.Selected_cloud->clear();
			ViewCloudUpdate(LayerCloud->makeShared(), false);
		}
		//	qDebug() << "ZERO END";
		return;
	}

	if (data.keyBoard_ctrl) {
		for (int i = 0; i < foundPointID.size(); ++i) {
			int nowLayer_selectedID = foundPointID[i];
			if (data.select_map.find(nowLayer_selectedID) == data.select_map.end())
			{
				//data.Selected_cloud->points.at(nowLayer_selectedID) = LayerCloud->points.at(nowLayer_selectedID);
				data.select_map.insert(pair<int, PointXYZRGB>(nowLayer_selectedID, LayerCloud->points.at(nowLayer_selectedID)));
			}
		}
		/*	visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(data.Selected_cloud, 255, 0, 0);
			data.viewer->removePointCloud("Red_ChosenPoints");
			data.viewer->addPointCloud(data.Selected_cloud, red, "Red_ChosenPoints");
			ui.qvtkWidget->update();*/
	}
	else if (data.keyBoard_alt)
	{
		for (int i = 0; i < foundPointID.size(); ++i) {
			int nowLayer_selectedID = foundPointID[i];
			if (data.select_map.find(nowLayer_selectedID) != data.select_map.end())
			{
				//PointXYZRGB nullPoint;
				//data.Selected_cloud->points.at(nowLayer_selectedID) = nullPoint;
				data.select_map.erase(nowLayer_selectedID);
			}
		}
		/*visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(data.Selected_cloud, 255, 0, 0);
		data.viewer->removePointCloud("Red_ChosenPoints");
		data.viewer->addPointCloud(data.Selected_cloud, red, "Red_ChosenPoints");
		ui.qvtkWidget->update();*/
	}
	else
	{
		//qDebug() << "NO KEY" << foundPointID.size();
		data.select_map.clear();
		data.Selected_cloud->clear();
		//data.Selected_cloud.reset(new PointCloud<PointXYZRGB>);
		data.Selected_cloud->resize(LayerCloud->size());
		for (int i = 0; i < foundPointID.size(); ++i) {
			int nowLayer_selectedID = foundPointID[i];
			if (data.select_map.find(nowLayer_selectedID) == data.select_map.end()) {
				//data.Selected_cloud->points[nowLayer_selectedID] = LayerCloud->points[nowLayer_selectedID];
				data.select_map.insert(pair<int, PointXYZRGB>(nowLayer_selectedID, LayerCloud->points.at(nowLayer_selectedID)));
			}
		}
		/*visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(data.Selected_cloud, 255, 0, 0);
		data.viewer->removePointCloud("Red_ChosenPoints");
		data.viewer->addPointCloud(data.Selected_cloud, red, "Red_ChosenPoints");
		ui.qvtkWidget->update();*/
	}
	data.Selected_cloud = LayerCloud->makeShared();
	for (map<int, PointXYZRGB>::iterator iter = data.select_map.begin(); iter != data.select_map.end(); ++iter)
	{
		data.Selected_cloud->points.at(iter->first).r = 255;
		data.Selected_cloud->points.at(iter->first).g = 0;
		data.Selected_cloud->points.at(iter->first).b = 0;
	}
	ViewCloudUpdate(data.Selected_cloud, false);
	/*visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(data.Selected_cloud, 255, 0, 0);
	data.viewer->removePointCloud("Red_ChosenPoints");
	data.viewer->addPointCloud(data.Selected_cloud, red, "Red_ChosenPoints");
	ui.qvtkWidget->update();*/
}
//----------------------
void QT_BaseMainWindow::cursor_BrushSelector(const pcl::visualization::MouseEvent& event) {
	//qDebug() << "INNN";
	QModelIndex index = ui.treeView->selectionModel()->currentIndex();
	if (index.row() == -1)
		return;
	PointCloud<PointXYZRGB>::Ptr LayerCloud;
	if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "complax_cloudInformation")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <complax_cloudInformation>().cloud_data;
	else if (std::string(data.standardModel->itemFromIndex(index)->data().typeName()) == "pcl::PointCloud<PointXYZRGB>::Ptr")
		LayerCloud = data.standardModel->itemFromIndex(index)->data().value <PointCloud<PointXYZRGB>::Ptr>();

	if (LayerCloud->size() > 0 && !data.AreaSelectMode)
	{
		if (event.getType() == event.MouseMove && data.BrushMode)
		{
			vtkRenderWindowInteractor *viewer_interactor = data.viewer->getRenderWindow()->GetInteractor();
			vtkPointPicker* point_picker = vtkPointPicker::SafeDownCast(viewer_interactor->GetPicker());
			float mouseX = (viewer_interactor->GetEventPosition()[0]);
			float mouseY = (viewer_interactor->GetEventPosition()[1]);
			viewer_interactor->StartPickCallback();
			vtkRenderer *ren = viewer_interactor->FindPokedRenderer(mouseX, mouseY);
			//qDebug() << "x:" << mouseX << "y:" << mouseY;
			point_picker->Pick(mouseX, mouseY, 0.0, ren);
			double picked[3]; point_picker->GetPickPosition(picked);

			PointCloud<PointXYZRGB>::Ptr cursor_premark(new PointCloud<PointXYZRGB>);
			KdTreeFLANN<PointXYZRGB>::Ptr tree(new KdTreeFLANN<PointXYZRGB>);
			std::vector<int> foundPointID;
			std::vector<float> foundPointSquaredDistance;
			tree->setInputCloud(LayerCloud);
			PointXYZRGB pickPoint(picked[0], picked[1], picked[2]);
			pickPoint.x = picked[0]; pickPoint.y = picked[1]; pickPoint.z = picked[2];
			pickPoint.r = 255, pickPoint.g = 255, pickPoint.b = 255;

			if (tree->radiusSearch(pickPoint, data.brush_radius, foundPointID, foundPointSquaredDistance) > 0)
			{
				for (int i = 0; i < foundPointID.size() - 1; ++i) {
					//if (i > foundPointID.size()*0.9)
					cursor_premark->push_back(LayerCloud->points[foundPointID[i]]);

					int nowLayer_selectedID = foundPointID[i];
					if (data.keyBoard_ctrl && data.select_map.find(nowLayer_selectedID) == data.select_map.end())
					{
						data.Selected_cloud->points.at(nowLayer_selectedID) = LayerCloud->points.at(nowLayer_selectedID);
						data.Selected_cloud->points.at(nowLayer_selectedID).r = 255;
						data.Selected_cloud->points.at(nowLayer_selectedID).g = 0;
						data.Selected_cloud->points.at(nowLayer_selectedID).b = 0;
						data.select_map.insert(pair<int, PointXYZRGB>(nowLayer_selectedID, LayerCloud->points.at(nowLayer_selectedID)));
					}

					else if (data.keyBoard_alt && data.select_map.find(nowLayer_selectedID) != data.select_map.end())
					{
						//convert
						//PointXYZRGB nullPoint;
						//data.Selected_cloud->points.at(nowLayer_selectedID) = nullPoint;
						data.Selected_cloud->points.at(nowLayer_selectedID).r = LayerCloud->points.at(nowLayer_selectedID).r;
						data.Selected_cloud->points.at(nowLayer_selectedID).g = LayerCloud->points.at(nowLayer_selectedID).g;
						data.Selected_cloud->points.at(nowLayer_selectedID).b = LayerCloud->points.at(nowLayer_selectedID).b;
						data.select_map.erase(nowLayer_selectedID);
					}
				}
			}

			if (data.keyBoard_ctrl || data.keyBoard_alt)
			{
				ViewCloudUpdate(data.Selected_cloud->makeShared(), false);

				/*visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(data.Selected_cloud, 255, 0, 0);
				data.viewer->removePointCloud("Red_ChosenPoints");
				data.viewer->addPointCloud(data.Selected_cloud, red, "Red_ChosenPoints");
				ui.qvtkWidget->update();*/
			}

			visualization::PointCloudColorHandlerCustom<PointXYZRGB> white(cursor_premark, 255, 255, 255);
			data.viewer->removePointCloud("White_BrushCursorPoints");
			data.viewer->addPointCloud(cursor_premark, white, "White_BrushCursorPoints");
			ui.qvtkWidget->update();
		}
		if (event.getButton() == event.MiddleButton && event.getType() == event.MouseButtonPress)
		{
		}
		if (event.getButton() == event.RightButton && event.getType() == event.MouseButtonRelease)
		{
		}
		if (event.getButton() == event.LeftButton && data.BrushMode)
		{
		}
		if (event.getButton() == event.MouseScrollDown || event.getButton() == event.MouseScrollUp)
		{

		}
		else
		{

		}
	}
}

void QT_BaseMainWindow::KeyBoard_eventController(const pcl::visualization::KeyboardEvent& event) {
	if (event.isCtrlPressed())
		data.keyBoard_ctrl = true;
	else if (event.isAltPressed())
		data.keyBoard_alt = true;

	if (event.keyUp()) {
		data.keyBoard_ctrl = false;
		data.keyBoard_alt = false;
	}

	if ((event.getKeySym() == "x" || event.getKeySym() == "X") && event.keyDown()) {
		data.AreaSelectMode = !data.AreaSelectMode;
		PointCloud<PointXYZRGB>::Ptr nullCloud(new PointCloud<PointXYZRGB>);
		data.viewer->removePointCloud("White_BrushCursorPoints");
		data.viewer->addPointCloud(nullCloud, "White_BrushCursorPoints");
		ui.qvtkWidget->update();

		if (data.AreaSelectMode)
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/AreaSelect_Press.png"), QSize(), QIcon::Normal, QIcon::Off);
			data.ToolButtonManager[3]->setIcon(icon);
		}
		else
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/AreaSelect.png"), QSize(), QIcon::Normal, QIcon::Off);
			data.ToolButtonManager[3]->setIcon(icon);
		}

		data.BrushMode = false;
		QIcon icon;
		icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);
		data.ToolButtonManager[2]->setIcon(icon);
	}

	if ((event.getKeySym() == "n" || event.getKeySym() == "N") && event.keyDown() && data.BrushMode) {
		data.brush_radius <= data.nowCloud_avg_distance ?
			data.brush_radius = data.nowCloud_avg_distance :
			data.brush_radius -= data.nowCloud_avg_distance;

		ui.label->setText(std::to_string(data.brush_radius).c_str());
		ui.qvtkWidget->update();
	}
	if ((event.getKeySym() == "m" || event.getKeySym() == "M") && event.keyDown() && data.BrushMode) {
		data.brush_radius += data.nowCloud_avg_distance;
		ui.label->setText(std::to_string(data.brush_radius).c_str());
		ui.qvtkWidget->update();
	}

	if ((event.getKeySym() == "b" || event.getKeySym() == "B") && event.keyDown()) {
		if (data.AreaSelectMode) {
			data.AreaSelectMode = !data.AreaSelectMode;
			QIcon icon;
			icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/AreaSelect.png"), QSize(), QIcon::Normal, QIcon::Off);
			data.ToolButtonManager[3]->setIcon(icon);
			//return;
		}

		data.BrushMode = !data.BrushMode;
		if (!data.BrushMode)
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/cursor1-2.png"), QSize(), QIcon::Normal, QIcon::Off);
			data.ToolButtonManager[2]->setIcon(icon);
			PointCloud<PointXYZRGB>::Ptr nullCloud(new PointCloud<PointXYZRGB>);
			data.viewer->removePointCloud("White_BrushCursorPoints");
			data.viewer->addPointCloud(nullCloud, "White_BrushCursorPoints");
		}
		else
		{
			QIcon icon;
			icon.addFile(QString::fromUtf8("./PCLAuxilary_pic_sorce/cursor1-2_press.png"), QSize(), QIcon::Normal, QIcon::Off);
			data.ToolButtonManager[2]->setIcon(icon);
		}
	}

	if ((event.getKeySym() == "z" || event.getKeySym() == "Z") && event.keyDown())
	{
		data.BrushSelectMode = true;
		ui.label->setText(std::to_string(event.keyDown()).c_str());
	}
	else if ((event.getKeySym() == "z" || event.getKeySym() == "Z") && !event.keyDown())
	{
		data.BrushSelectMode = false;
		ui.label->setText(std::to_string(event.keyDown()).c_str());
	}

	if ((event.getKeySym() == "a" || event.getKeySym() == "A") && event.keyDown())
	{
		visualization::Camera cameraParameters;
		data.viewer->getCameraParameters(cameraParameters);
		data.cameraView %= 12;

		float avgData = data.nowCloud_avg_distance * 2000;
		switch (data.cameraView)
		{
		case 0:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		case 1:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, -1, 0, 0);
			break;
		case 2:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] - avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		case 3:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] - avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, -1, 0, 0);
			break;
		case 4:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1] + avgData, cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				1, 0, 0, 0);
			break;
		case 5:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1] + avgData, cameraParameters.focal[2] + avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				-1, 0, 0, 0);
			break;
		case 6:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1] - avgData, cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				1, 0, 0, 0);
			break;
		case 7:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1] - avgData, cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				-1, 0, 0, 0);
			break;
		case 8:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0] + avgData, cameraParameters.focal[1], cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 0, 1, 0);
			break;
		case 9:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0] + avgData, cameraParameters.focal[1], cameraParameters.focal[2] + avgData,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 0, -1, 0);
			break;
		case 10:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0] - avgData, cameraParameters.focal[1], cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 0, 1, 0);
			break;
		case 11:
			data.viewer->setCameraPosition(
				cameraParameters.focal[0] - avgData, cameraParameters.focal[1], cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 0, -1, 0);
			break;
		}
		data.cameraView++;
	}
}

void  QT_BaseMainWindow::Brush_change() {
	if (data.BrushMode)
	{
		data.brush_radius = data.nowCloud_avg_distance *ui.Brush_ClusterSpinBox->value();
		//qDebug() << data.brush_radius;

		ui.label->setText(std::to_string(data.brush_radius).c_str());
		ui.qvtkWidget->update();
	}
}