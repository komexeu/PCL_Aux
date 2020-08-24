#include "pclauxilary.h"

PCLAuxilary::PCLAuxilary(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setMouseTracking(true);

	ui_Data.layerMapper = new QSignalMapper();
	ui_Data.Selected_points.reset(new PointCloud<PointXYZRGB>);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	this->setWindowTitle("PCL viewer");
	ui_Data.VTKWindow_Cloud.reset(new PointCloud<PointXYZRGB>);
	ui_Data.viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	ui.qvtkWidget->SetRenderWindow(ui_Data.viewer->getRenderWindow());
	ui_Data.viewer->setupInteractor(ui.qvtkWidget->GetInteractor(), ui.qvtkWidget->GetRenderWindow());
	//ui_Data.viewer->getRenderWindow()->HideCursor();

	ui_Data.ClusterSlider = new QSlider(Qt::Horizontal);
	ui_Data.ClusterSlider->setMinimum(1);
	ui_Data.ClusterSlider->setMaximum(500);
	ui_Data.ClusterSlider->setStyleSheet("QSlider::sub-page:Horizontal { background-color: #D1466C; }"
		"QSlider::add-page:Horizontal { background-color: #F4A9C9; }"
		"QSlider::groove:Horizontal { background: transparent; height:4px; }"
		"QSlider::handle:Horizontal { width:10px; border-radius:5px; background:#D1466C; margin: -5px 0px -5px 0px; }");
	ui_Data.ClusterSpinBox = new QSpinBox();
	ui_Data.ClusterSpinBox->setRange(ui_Data.ClusterSlider->minimum(), ui_Data.ClusterSlider->maximum());
	ui.formLayout->setWidget(ui.formLayout->count() + 1, QFormLayout::FieldRole, ui_Data.ClusterSlider);
	ui.formLayout->setWidget(ui.formLayout->count(), QFormLayout::LabelRole, ui_Data.ClusterSpinBox);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(BTN_ImportCPPress()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(BTN_AddClusterLayer()));
	connect(ui_Data.layerMapper, SIGNAL(mapped(int)), this, SLOT(BTN_GetLayer(int)));
	connect(ui_Data.ClusterSpinBox, SIGNAL(valueChanged(int)), ui_Data.ClusterSlider, SLOT(setValue(int)));
	connect(ui_Data.ClusterSlider, SIGNAL(valueChanged(int)), ui_Data.ClusterSpinBox, SLOT(setValue(int)));
	connect(ui_Data.ClusterSpinBox, SIGNAL(valueChanged(int)), this, SLOT(Slider_CloudCluster()));
	connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(BTN_Smooth()));
	connect(ui.pushButton_4, SIGNAL(clicked()), this, SLOT(BTN_UserSegmentation()));

	ui_Data.viewer->registerAreaPickingCallback(&Area_PointCloud_Selector);
	ui_Data.viewer->registerKeyboardCallback(KeyBoard_Selector);
	ui_Data.viewer->registerMouseCallback(mousePos_CircleSelector);

	ui_Data.viewer->addPointCloud(ui_Data.VTKWindow_Cloud, "cloud");
	ui_Data.viewer->resetCamera();
	ui.retranslateUi(this);
	ui.qvtkWidget->update();
}

void PCLAuxilary::BTN_ImportCPPress() {
	QString FileRoot = QFileDialog::getOpenFileName(NULL, tr("open a pcd."), "D:/", tr("All file(*.*)"));

	if (!FileRoot.isEmpty())
	{
		ui_Data.viewer->removeShape("circle");

		foreach(QPushButton* btn, ui_Data.vec_BTNLayer)
			delete btn;
		ui_Data.Layer_Cloud.clear();
		ui_Data.vec_BTNLayer.clear();
		ui_Data.layerMapper->destroyed();

		QFileInfo qfi(FileRoot);
		QByteArray ba = qfi.filePath().toLocal8Bit();
		const char* filename = ba.data();
		ba = qfi.suffix().toLocal8Bit();
		std::string fileExtension = ba.data();

		PointCloud<PointXYZRGB>::Ptr importCloud(new PointCloud<PointXYZRGB>);

		//QMessageBox::warning(NULL, "Warning!", tr(fileExtension.c_str()), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (fileExtension == "csv")
		{
			QFile csvfile(FileRoot);
			if (csvfile.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QTextStream infile(&csvfile);
				int Line_i = 0;

				while (!infile.atEnd()) {
					QString lineStr = infile.readLine();
					Line_i++;

					if (Line_i == 1)//欄位名稱跳過				
						continue;

					QStringList res = lineStr.split(" ");//分割","
					PointXYZRGB tmp;
					if (res.size() == 6)
					{
						tmp.x = atof(res[0].toStdString().c_str());
						tmp.y = atof(res[1].toStdString().c_str());
						tmp.z = atof(res[2].toStdString().c_str());
						tmp.r = atof(res[3].toStdString().c_str());
						tmp.g = atof(res[4].toStdString().c_str());
						tmp.b = atof(res[5].toStdString().c_str());
					}
					else
						break;
					importCloud->push_back(tmp);
				}
				csvfile.close();
			}
		}
		else if (fileExtension == "pcd") {
			io::loadPCDFile<PointXYZRGB>(filename, *importCloud);
		}

		ui_Data.VTKWindow_Cloud = importCloud->makeShared();
		ui_Data.Layer_Cloud.push_back(importCloud);
		ui_Data.viewer->updatePointCloud(ui_Data.VTKWindow_Cloud, "cloud");

		std::string tmpName = "LayerBase";
		QPushButton* layer;
		QFont font;
		font.setFamily(QString::fromUtf8("Taipei Sans TC Beta"));
		font.setPointSize(12);
		layer = new QPushButton(ui.formWidget_2);
		layer->setObjectName(QString::fromUtf8(tmpName.c_str()));
		layer->setText(tmpName.c_str());
		layer->setFont(font);
		layer->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255);"));
		layer->setFlat(true);
		ui_Data.vec_BTNLayer.push_back(layer);
		ui.formLayout_2->setWidget(ui.formLayout_2->count() + 1, QFormLayout::SpanningRole,
			ui_Data.vec_BTNLayer[ui_Data.vec_BTNLayer.size() - 1]);
		connect(layer, SIGNAL(clicked()), ui_Data.layerMapper, SLOT(map()));
		ui_Data.layerMapper->setMapping(layer, ui_Data.vec_BTNLayer.size() - 1);

		PCLAuxilary::indices.clear();
		ui_Data.Selected_points->clear();
		ui_Data.viewer->removePointCloud("chosenCloud");
		ui_Data.viewer->resetCamera();
		ui.qvtkWidget->update();
		//ui_Data.viewer->updateCamera();
		//ui_Data.viewer->resetCamera();
	}
}

void PCLAuxilary::BTN_UserSegmentation() {
	if (ui_Data.Selected_points->size() > 0)
	{
		PointCloud<PointXYZRGB>::Ptr segCloud(new PointCloud<PointXYZRGB>);
		for (int i = 0; i < ui_Data.Selected_points->size(); i++)
			segCloud->push_back(ui_Data.VTKWindow_Cloud->points.at(PCLAuxilary::indices[i]));
		ui_Data.Layer_Cloud.push_back(segCloud);

		PCLAuxilary::indices.clear();
		ui_Data.Selected_points->clear();
		ui_Data.viewer->removePointCloud("chosenCloud");
		ui_Data.viewer->removePointCloud("brushSelectCloud");

		std::string tmpName = "Layer" + std::to_string(ui.formLayout_2->count());
		QPushButton* layer;
		QFont font;
		font.setFamily(QString::fromUtf8("Taipei Sans TC Beta"));
		font.setPointSize(12);
		layer = new QPushButton(ui.formWidget_2);
		layer->setObjectName(QString::fromUtf8(tmpName.c_str()));
		layer->setText(tmpName.c_str());
		layer->setFont(font);
		layer->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255);"));
		layer->setFlat(true);
		ui_Data.vec_BTNLayer.push_back(layer);
		ui.formLayout_2->setWidget(ui.formLayout_2->count() + 1, QFormLayout::SpanningRole,
			ui_Data.vec_BTNLayer[ui_Data.vec_BTNLayer.size() - 1]);
		ui_Data.layerMapper->setMapping(layer, ui_Data.vec_BTNLayer.size() - 1);
		connect(layer, SIGNAL(clicked()), ui_Data.layerMapper, SLOT(map()));

		ui.qvtkWidget->update();
	}
}

void PCLAuxilary::Slider_CloudCluster() {
	ui_Data.Layer_CloudTmp.clear();
	PCLAuxilary::indices.clear();
	ui_Data.Selected_points->clear();
	ui_Data.viewer->removePointCloud("chosenCloud");

	for (int i = 0; i < ui_Data.VTKWindow_Cloud->size(); ++i)
	{
		ui_Data.VTKWindow_Cloud->points[i].r = 255;
		ui_Data.VTKWindow_Cloud->points[i].g = 255;
		ui_Data.VTKWindow_Cloud->points[i].b = 255;
	}
	PointCloud<PointXYZRGB>::Ptr cluster_cloud(new PointCloud<PointXYZRGB>);
	cluster_cloud = ui_Data.VTKWindow_Cloud->makeShared();
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
	tree->setInputCloud(cluster_cloud);
	EuclideanClusterExtraction<PointXYZRGB> ec;
	ec.setClusterTolerance(float(ui_Data.ClusterSlider->value() *0.001));
	ec.setMinClusterSize(300);
	ec.setSearchMethod(tree);
	ec.setInputCloud(cluster_cloud);
	std::vector<PointIndices> cluster_indice;
	ec.extract(cluster_indice);
	for (std::vector<PointIndices>::const_iterator i = cluster_indice.begin(); i != cluster_indice.end(); ++i)
	{
		int color_R = rand() % 250;
		int color_G = rand() % 250;
		int color_B = rand() % 250;
		PointCloud<PointXYZRGB>::Ptr cluster(new PointCloud<PointXYZRGB>);
		for (std::vector<int>::const_iterator point = i->indices.begin(); point != i->indices.end(); ++point) {
			cluster_cloud->points[*point].r = color_R;
			cluster_cloud->points[*point].g = color_G;
			cluster_cloud->points[*point].b = color_B;
			cluster->points.push_back(cluster_cloud->points[*point]);
			ui_Data.VTKWindow_Cloud->points[*point].r = color_R;
			ui_Data.VTKWindow_Cloud->points[*point].g = color_G;
			ui_Data.VTKWindow_Cloud->points[*point].b = color_B;
		}
		cluster->width = cluster->points.size();
		cluster->height = 1;
		cluster->is_dense = true;

		if (cluster->points.size() <= 0)
			continue;

		ui_Data.Layer_CloudTmp.push_back(cluster);
	}
	ui_Data.viewer->updatePointCloud(ui_Data.VTKWindow_Cloud, "cloud");
	ui.qvtkWidget->update();
}

void PCLAuxilary::BTN_GetLayer(int i) {
	PCLAuxilary::indices.clear();
	ui_Data.Selected_points->clear();
	ui_Data.viewer->removePointCloud("chosenCloud");
	ui_Data.VTKWindow_Cloud = ui_Data.Layer_Cloud[i]->makeShared();
	ui_Data.viewer->updatePointCloud(ui_Data.VTKWindow_Cloud, "cloud");
	ui.qvtkWidget->update();
}

void PCLAuxilary::BTN_AddClusterLayer() {
	if (ui_Data.Layer_CloudTmp.size() != 0 && ui_Data.Selected_points->size() <= 0)
	{
		//只保留第一個LAYER，其他同按鈕刪除
		for (int i = ui_Data.Layer_Cloud.size() - 1; i > 0; i--) {
			ui_Data.Layer_Cloud.pop_back();
			delete ui_Data.vec_BTNLayer[i];
			ui_Data.vec_BTNLayer.pop_back();
		}
		for (int i = 0; i < ui_Data.Layer_CloudTmp.size(); i++)
			ui_Data.Layer_Cloud.push_back(ui_Data.Layer_CloudTmp[i]);

		for (int i = 0; i < ui_Data.Layer_Cloud.size() - 1; i++)
		{
			std::string tmpName = "Layer" + std::to_string(ui.formLayout_2->count());
			QPushButton* layer;
			QFont font;
			font.setFamily(QString::fromUtf8("Taipei Sans TC Beta"));
			font.setPointSize(12);
			layer = new QPushButton(ui.formWidget_2);
			layer->setObjectName(QString::fromUtf8(tmpName.c_str()));
			layer->setText(tmpName.c_str());
			layer->setFont(font);
			layer->setStyleSheet(QString::fromUtf8("color:rgb(255,255,255);"));
			layer->setFlat(true);
			ui_Data.vec_BTNLayer.push_back(layer);
			ui.formLayout_2->setWidget(ui.formLayout_2->count() + 1, QFormLayout::SpanningRole,
				ui_Data.vec_BTNLayer[ui_Data.vec_BTNLayer.size() - 1]);
			ui_Data.layerMapper->setMapping(layer, i + 1);
			connect(layer, SIGNAL(clicked()), ui_Data.layerMapper, SLOT(map()));
		}
		ui_Data.Layer_CloudTmp.clear();
		PCLAuxilary::indices.clear();
		ui_Data.Selected_points->clear();
	}
	else { QMessageBox::warning(NULL, "Warning!", "0"); }
}

void PCLAuxilary::BTN_Smooth() {
	ui_Data.viewer->removePointCloud("chosenCloud");
	ui.qvtkWidget->update();
	PointCloud<PointXYZRGB>::Ptr smoothCloud(new PointCloud<PointXYZRGB>);
	MovingLeastSquares<PointXYZRGB, PointXYZRGB>::Ptr mls(new MovingLeastSquares<PointXYZRGB, PointXYZRGB>);
	search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);

	tree->setInputCloud(ui_Data.VTKWindow_Cloud);

	mls->setComputeNormals(true);
	mls->setInputCloud(ui_Data.VTKWindow_Cloud);
	mls->setSearchMethod(tree);
	mls->setNumberOfThreads(1);
	mls->setPointDensity(ui_Data.VTKWindow_Cloud->size());
	mls->setSearchRadius(5);
	mls->setPolynomialFit(true);
	mls->process(*smoothCloud);

	ui_Data.VTKWindow_Cloud = smoothCloud->makeShared();
	ui_Data.viewer->updatePointCloud(ui_Data.VTKWindow_Cloud, "cloud");
	ui.qvtkWidget->update();
}

void PCLAuxilary::Area_PointCloud_Selector(const pcl::visualization::AreaPickingEvent& event) {
	std::vector<int> undoIndices = PCLAuxilary::indices;
	std::vector<int> deIndices;
	PCLAuxilary::indices.clear();

	if (ui_Data.keyBoard_ctrl)
	{
		if (event.getPointsIndices(PCLAuxilary::indices) > 0) {
			for (int i = 0; i < PCLAuxilary::indices.size(); ++i) {
				if (std::find(undoIndices.begin(), undoIndices.end(), indices[i])
					== undoIndices.end()) {
					ui_Data.Selected_points->points.push_back(ui_Data.VTKWindow_Cloud->points.at(PCLAuxilary::indices[i]));
					undoIndices.push_back(PCLAuxilary::indices[i]);
				}
			}
			PCLAuxilary::indices = undoIndices;
			visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(ui_Data.Selected_points, 255, 0, 0);
			ui_Data.viewer->removePointCloud("chosenCloud");
			ui_Data.viewer->addPointCloud(ui_Data.Selected_points, red, "chosenCloud");
			ui.qvtkWidget->update();
		}
		else
		{
			PCLAuxilary::indices = undoIndices;
		}
	}
	else if (ui_Data.keyBoard_alt)
	{
		if (event.getPointsIndices(deIndices) > 0)
		{
			PointCloud<PointXYZRGB>::Ptr deSelected_points(new PointCloud<PointXYZRGB>);
			PCLAuxilary::indices.clear();
			/*QMessageBox::warning(NULL, "Warning!", std::to_string(indices.size()).c_str());
			QMessageBox::warning(NULL, "Warning!", std::to_string(deIndices.size()).c_str());*/
			for (int i = 0; i < undoIndices.size(); i++)
			{
				if (std::find(deIndices.begin(), deIndices.end(), undoIndices[i])
					== deIndices.end()) {
					deSelected_points->push_back(ui_Data.VTKWindow_Cloud->points.at(undoIndices[i]));
					PCLAuxilary::indices.push_back(undoIndices[i]);
				}
			}
			ui_Data.Selected_points = deSelected_points->makeShared();
			visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(ui_Data.Selected_points, 255, 0, 0);
			ui_Data.viewer->removePointCloud("chosenCloud");
			ui_Data.viewer->addPointCloud(ui_Data.Selected_points, red, "chosenCloud");
			ui.qvtkWidget->update();
		}
		else
		{
			PCLAuxilary::indices = undoIndices;
		}
	}
	else
	{
		if (event.getPointsIndices(PCLAuxilary::indices) > 0)
		{
			ui_Data.Selected_points->clear();
			for (int i = 0; i < PCLAuxilary::indices.size(); ++i)
				ui_Data.Selected_points->points.push_back(ui_Data.VTKWindow_Cloud->points.at(PCLAuxilary::indices[i]));

			visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(ui_Data.Selected_points, 255, 0, 0);
			ui_Data.viewer->removePointCloud("chosenCloud");
			ui_Data.viewer->addPointCloud(ui_Data.Selected_points, red, "chosenCloud");
			ui.qvtkWidget->update();
		}
		else
		{
			PCLAuxilary::indices.clear();
			ui_Data.Selected_points->clear();
			visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(ui_Data.Selected_points, 255, 0, 0);
			ui_Data.viewer->addPointCloud(ui_Data.Selected_points, red, "chosenCloud");
			ui_Data.viewer->removePointCloud("chosenCloud");
			ui.qvtkWidget->update();
		}
	}
}

void PCLAuxilary::mousePos_CircleSelector(const pcl::visualization::MouseEvent& event) {
	if (ui_Data.VTKWindow_Cloud->size() > 0 && !ui_Data.AreaSelectMode)
	{
		//ui_Data.viewer->getRenderWindow()->HideCursor();
		if (event.getType() == event.MouseMove && ui_Data.BrushMode && ui_Data.circleSelectMode && ui_Data.selectDone)
		{
			if (ui_Data.Selected_points->size() == ui_Data.VTKWindow_Cloud->size())
				return;

			ui_Data.selectDone = false;
			PointCloud<PointXYZRGB>::Ptr brushRadisu_Cursor(new PointCloud<PointXYZRGB>);

			vtkRenderWindowInteractor *viewer_interactor = ui_Data.viewer->getRenderWindow()->GetInteractor();
			vtkPointPicker* point_picker = vtkPointPicker::SafeDownCast(viewer_interactor->GetPicker());
			int mouseX = (viewer_interactor->GetEventPosition()[0]);
			int mouseY = (viewer_interactor->GetEventPosition()[1]);

			viewer_interactor->StartPickCallback();
			vtkRenderer *ren = viewer_interactor->FindPokedRenderer(mouseX, mouseY);
			point_picker->Pick(mouseX, mouseY, 0.0, ren);
			int selectPoint_ID = static_cast<int> (point_picker->GetPointId());

			if (selectPoint_ID >= 0 && selectPoint_ID < ui_Data.VTKWindow_Cloud->size())
			{
				if (std::find(ui_Data.circle_selectID.begin(), ui_Data.circle_selectID.end(), selectPoint_ID)
					== ui_Data.circle_selectID.end()) {
					ui_Data.circle_selectID.push_back(selectPoint_ID);

					//ui.label->setText(std::to_string(ui_Data.Selected_points->size()).c_str());
				}
			}

			ui_Data.selectDone = true;
		}
		//if (event.getType() == event.MouseMove && ui_Data.BrushMode)
		//{
		//	PointCloud<PointXYZRGB>::Ptr brushCursor(new PointCloud<PointXYZRGB>);

		//	vtkRenderWindowInteractor *iren = ui_Data.viewer->getRenderWindow()->GetInteractor();
		//	vtkPointPicker* point_picker = vtkPointPicker::SafeDownCast(iren->GetPicker());
		//	int mouseX = (iren->GetEventPosition()[0]);
		//	int mouseY = (iren->GetEventPosition()[1]);
		//	iren->StartPickCallback();
		//	vtkRenderer *ren = iren->FindPokedRenderer(mouseX, mouseY);
		//	point_picker->Pick(mouseX, mouseY, 0.0, ren);
		//	int selectPoint_ID = static_cast<int> (point_picker->GetPointId());
		//	if (selectPoint_ID != -1 && selectPoint_ID < ui_Data.VTKWindow_Cloud->size())
		//	{
		//		search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
		//		std::vector<int> foundPointID;
		//		std::vector<float> foundPointSquaredDistance;
		//		tree->setInputCloud(ui_Data.VTKWindow_Cloud);
		//		tree->radiusSearch(ui_Data.VTKWindow_Cloud->points.at(selectPoint_ID), ui_Data.brush_radius, foundPointID, foundPointSquaredDistance);

		//		for (int i = 0; i < foundPointID.size(); i++)
		//		{
		//			//PCLAuxilary::indices.push_back(foundPointID[i]);
		//			brushCursor->push_back(ui_Data.VTKWindow_Cloud->points.at(foundPointID[i]));
		//		}
		//		visualization::PointCloudColorHandlerCustom<PointXYZRGB> white(brushCursor, 255, 255, 255);
		//		ui_Data.viewer->removePointCloud("BrushCloud");
		//		ui_Data.viewer->addPointCloud(brushCursor, white, "BrushCloud");
		//		ui.qvtkWidget->update();

		//		//ui.label->setText(std::to_string(ui_Data.Selected_points->size()).c_str());
		//	}
		//}
		if (event.getButton() == event.MiddleButton && event.getType() == event.MouseButtonPress)
		{

			//QMessageBox::warning(NULL, "Warning!", "right");
		}
		if (event.getButton() == event.RightButton && event.getType() == event.MouseButtonRelease)
		{
			//ui.label->setText(std::to_string(ui_Data.Selected_points->size()).c_str());
		}
		if (event.getButton() == event.LeftButton && ui_Data.BrushMode)
		{
			visualization::Camera cameraParameters;
			ui_Data.viewer->getCameraParameters(cameraParameters);
			//ui.label->setText(std::to_string(0).c_str());
		}
		if (event.getButton() == event.MouseScrollDown || event.getButton() == event.MouseScrollUp)
		{

		}
		else
		{
			//ui_Data.viewer->getRenderWindow()->ShowCursor();
		}
	}
}

void PCLAuxilary::KeyBoard_Selector(const pcl::visualization::KeyboardEvent& event) {
	if (event.isCtrlPressed())
		ui_Data.keyBoard_ctrl = true;
	else if (event.isAltPressed())
		ui_Data.keyBoard_alt = true;

	if (event.keyUp()) {
		ui_Data.keyBoard_ctrl = false;
		ui_Data.keyBoard_alt = false;
	}
	if (event.getKeySym() == "x" && event.keyDown()) {
		ui_Data.AreaSelectMode = !ui_Data.AreaSelectMode;
	}

	if (event.getKeySym() == "n" && event.keyDown() && ui_Data.BrushMode) {
		ui_Data.brush_radius <= 0.5 ? ui_Data.brush_radius : ui_Data.brush_radius -= 0.5;

		ui.label->setText(std::to_string(ui_Data.brush_radius).c_str());
		ui.qvtkWidget->update();
	}
	if (event.getKeySym() == "m" && event.keyDown() && ui_Data.BrushMode) {
		ui_Data.brush_radius += 0.5;
		ui.label->setText(std::to_string(ui_Data.brush_radius).c_str());
		ui.qvtkWidget->update();
	}

	if (event.getKeySym() == "b" && event.keyDown()) {
		ui_Data.BrushMode = !ui_Data.BrushMode;
		if (!ui_Data.BrushMode)
		{
			ui_Data.viewer->removePointCloud("BrushCursorCloud");
		}
	}

	if (event.getKeySym() == "z" && event.keyDown())
	{
		ui_Data.circleSelectMode = true;
		//ui.label->setText(std::to_string(1).c_str());
	}
	else if (event.getKeySym() == "z" && event.keyUp() && ui_Data.selectDone)
	{
		ui_Data.circleSelectMode = false;
		//ui.label->setText(std::to_string(0).c_str());
		for (int i = 0; i < ui_Data.circle_selectID.size(); i++)
		{
			search::KdTree<PointXYZRGB>::Ptr tree(new search::KdTree<PointXYZRGB>);
			std::vector<int> foundPointID;
			std::vector<float> foundPointSquaredDistance;
			tree->setInputCloud(ui_Data.VTKWindow_Cloud);
			tree->radiusSearch(ui_Data.VTKWindow_Cloud->points.at(ui_Data.circle_selectID[i]), ui_Data.brush_radius, foundPointID, foundPointSquaredDistance);

			for (int i = 0; i < foundPointID.size(); i++)
			{
				if (std::find(PCLAuxilary::indices.begin(), PCLAuxilary::indices.end(), foundPointID[i])
					== PCLAuxilary::indices.end()) {
					PCLAuxilary::indices.push_back(foundPointID[i]);
					ui_Data.Selected_points->push_back(ui_Data.VTKWindow_Cloud->points.at(foundPointID[i]));
				}
			}
			visualization::PointCloudColorHandlerCustom<PointXYZRGB> red(ui_Data.Selected_points, 255, 0, 0);
			ui_Data.viewer->removePointCloud("chosenCloud");
			ui_Data.viewer->addPointCloud(ui_Data.Selected_points, red, "chosenCloud");
			ui.qvtkWidget->update();
		}
		ui_Data.circle_selectID.clear();
	}

	if (event.getKeySym() == "u" && event.keyDown())
	{
		visualization::Camera cameraParameters;
		ui_Data.viewer->getCameraParameters(cameraParameters);
		ui_Data.cameraView %= 7;
		switch (ui_Data.cameraView)
		{
		case 0:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + 50,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		case 1:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + 50,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				1, 0, 0, 0);
			break;
		case 2:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + 50,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, -1, 0, 0);
			break;
		case 3:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] + 50,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				-1, 0, 0, 0);
			break;
		case 4:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0] + 50, cameraParameters.focal[1], cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		case 5:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0] - 50, cameraParameters.focal[1], cameraParameters.focal[2],
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		case 6:
			ui_Data.viewer->setCameraPosition(
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2] - 50,
				cameraParameters.focal[0], cameraParameters.focal[1], cameraParameters.focal[2],
				0, 1, 0, 0);
			break;
		}
		ui_Data.cameraView++;
	}
}

void PCLAuxilary::LayerBoundingBox(PointCloud<PointXYZRGB>::Ptr layerCloud, int i) {
	//特徵提取
	MomentOfInertiaEstimation<PointXYZRGB> feature_extractor;
	feature_extractor.setInputCloud(layerCloud);
	feature_extractor.compute();
	//慣性矩
	std::vector<float> moment_of_inertia;
	//偏心率
	std::vector<float> eccentricity;
	float major_value, middle_value, minor_value;
	Eigen::Vector3f major_vector, middle_vector, minor_vector;
	Eigen::Vector3f mass_center;

	feature_extractor.getMomentOfInertia(moment_of_inertia);
	feature_extractor.getEccentricity(eccentricity);
	PointXYZRGB min_point_OBB, max_point_OBB, position_OBB;
	Eigen::Matrix3f rotational_matrix_OBB;
	feature_extractor.getOBB(min_point_OBB, max_point_OBB, position_OBB, rotational_matrix_OBB);
	feature_extractor.getEigenValues(major_value, middle_value, minor_value);
	feature_extractor.getEigenVectors(major_vector, middle_vector, minor_vector);
	feature_extractor.getMassCenter(mass_center);

	Eigen::Vector3f position(position_OBB.x, position_OBB.y, position_OBB.z);
	Eigen::Quaternionf quat(rotational_matrix_OBB);
	std::string OBB_ID = "OBB" + std::to_string(i);
	ui_Data.viewer->addCube(position, quat, max_point_OBB.x - min_point_OBB.x, max_point_OBB.x - min_point_OBB.x,
		max_point_OBB.x - min_point_OBB.x, OBB_ID);
	ui_Data.viewer->setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION,
		pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, OBB_ID);
}