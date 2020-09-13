#pragma once

//stl
#include <vector>

//self
#include "ui_pclauxilary.h"
#include "DataBase.h"
#include "QT_ToolBarManager.h"
#include "CloudPoints_Tools.h"

//qt
#include <QtWidgets/QMainWindow>

//vtk
#include <vtkRenderWindow.h>
#include <vtkAutoInit.h>
#include <vtkRenderWindow.h>
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);
#include <vtkLODActor.h>
#include <vtkPointpicker.h>
#include <vtkPropPicker.h>
#include <glut.h>
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)

using namespace pcl;

class QT_BaseMainWindow :public QMainWindow
{
	Q_OBJECT
public:

	QT_BaseMainWindow(QWidget *parent = Q_NULLPTR);
	//更新畫面
	void ViewCloudUpdate(PointCloud<PointXYZRGB>::Ptr updateCloud, bool resetCamera);
	//紅色標記區域刪除
	static void RedSelectClear();
	//初始化模式
	static void initModes();

	//區域選取
	static void Area_PointCloud_Selector(const pcl::visualization::AreaPickingEvent& event);
	//已選點雲ID
	//static std::vector<int> indices;
	
	//鍵盤事件
	static void KeyBoard_eventController(const pcl::visualization::KeyboardEvent& event);
	//滑鼠事件
	static void cursor_BrushSelector(const pcl::visualization::MouseEvent& event);

	static Ui_DataBase::dataBase data;
	static Ui::PCLAuxilaryClass ui;

public Q_SLOTS:
	//取得點雲
	void realsense_getpoints();

	//樹物件點擊
	void Tree_selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/);
	//樹物件生成
	void Tree_importCloud();
	void Tree_exportCloud();
	void Tree_confirmSegCloud();
	void Tree_Slider_PreSegCloud();
	void Tree_Smooth();
	void Tree_deleteLayer();
	//自選點雲分割(生成圖層)
	void Tree_UserSegmentation();

private:
	void roll(QList<QModelIndex> &indexList, QModelIndex index);
	QList<QModelIndex> GetChildrenIndex(QModelIndex index);
};
//std::vector<int> QT_BaseMainWindow::indices;
Ui::PCLAuxilaryClass QT_BaseMainWindow::ui;
Ui_DataBase::dataBase QT_BaseMainWindow::data;
