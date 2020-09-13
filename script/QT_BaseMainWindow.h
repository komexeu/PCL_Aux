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
	//��s�e��
	void ViewCloudUpdate(PointCloud<PointXYZRGB>::Ptr updateCloud, bool resetCamera);
	//����аO�ϰ�R��
	static void RedSelectClear();
	//��l�ƼҦ�
	static void initModes();

	//�ϰ���
	static void Area_PointCloud_Selector(const pcl::visualization::AreaPickingEvent& event);
	//�w���I��ID
	//static std::vector<int> indices;
	
	//��L�ƥ�
	static void KeyBoard_eventController(const pcl::visualization::KeyboardEvent& event);
	//�ƹ��ƥ�
	static void cursor_BrushSelector(const pcl::visualization::MouseEvent& event);

	static Ui_DataBase::dataBase data;
	static Ui::PCLAuxilaryClass ui;

public Q_SLOTS:
	//���o�I��
	void realsense_getpoints();

	//�𪫥��I��
	void Tree_selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/);
	//�𪫥�ͦ�
	void Tree_importCloud();
	void Tree_exportCloud();
	void Tree_confirmSegCloud();
	void Tree_Slider_PreSegCloud();
	void Tree_Smooth();
	void Tree_deleteLayer();
	//�ۿ��I������(�ͦ��ϼh)
	void Tree_UserSegmentation();

private:
	void roll(QList<QModelIndex> &indexList, QModelIndex index);
	QList<QModelIndex> GetChildrenIndex(QModelIndex index);
};
//std::vector<int> QT_BaseMainWindow::indices;
Ui::PCLAuxilaryClass QT_BaseMainWindow::ui;
Ui_DataBase::dataBase QT_BaseMainWindow::data;
