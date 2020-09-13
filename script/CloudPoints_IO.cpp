#include "CloudPoints_IO.h"

#include <qdebug.h>
bool CloudPoints_IO::RootSelector() {
	suffix_ = "";
	import_cloud_->clear();
	qDebug() << "TO OPEN!!!!";

	qDebug() << q_file_path_;
	q_file_path_ = QFileDialog::getOpenFileName(nullptr, QObject::tr("Select a root."), "C:/", QObject::tr("All file(*.*)"));
	qDebug() << "SUCCESS OPEN!!!!";
	if (q_file_path_.isEmpty())
		return(false);

	QFileInfo qfi(q_file_path_);
	QByteArray ba = qfi.filePath().toLocal8Bit();
	file_path_ = ba.data();
	ba = qfi.suffix().toLocal8Bit();
	suffix_ = ba.data();
	ba = qfi.fileName().toLocal8Bit();
	file_name_ = ba.data();

	return(true);
}

bool CloudPoints_IO::csv2pointCloud() {
	QFile csvfile(q_file_path_);
	if (csvfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream infile(&csvfile);
		int Line_i = 0;

		while (!infile.atEnd()) {
			QString lineStr = infile.readLine();
			Line_i++;

			if (Line_i == 1)//欄位名稱跳過				
				continue;

			QStringList res = lineStr.split(",");//分割","
			qDebug() << res.size();

			if (res.size()==1)
			{
				 res = lineStr.split(" ");
				qDebug() << res.size();
			}

			if (res.size() == 1)
			{
				 res = lineStr.split("/t");
				qDebug() << res.size();
			}

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
			import_cloud_->push_back(tmp);
		}
		csvfile.close();

		if (Line_i <= 1)
			return (false);
		else
			return(true);
	}
}

bool CloudPoints_IO::CloudImport() {
	if (!CloudPoints_IO::RootSelector())
		return(false);

	if (suffix_ == "csv") {
		if (CloudPoints_IO::csv2pointCloud())
			return(true);
	}
	else if (suffix_ == "pcd") {
		if (io::loadPCDFile<PointXYZRGB>(file_path_, *import_cloud_) != -1)
			return(true);
	}
	else
		return(false);
}

bool CloudPoints_IO::CloudExport(std::vector<PointCloud<PointXYZRGB>::Ptr> childern_cloudData) {
	QString output_root = QFileDialog::getSaveFileName(nullptr, QObject::tr("Select a root."), "C:/", QObject::tr("All file(*.pcd)"));
	qDebug() << output_root;
	if (output_root.isEmpty())
		return(false);

	PointCloud<PointXYZRGBObjectID>::Ptr myObj(new PointCloud < PointXYZRGBObjectID>);
	for (int i = 0; i < childern_cloudData.size(); i++)
	{
		qDebug() << childern_cloudData.size();
		PointCloud<PointXYZRGB>::Ptr tmpcld = childern_cloudData[i]->makeShared();
		qDebug() << childern_cloudData[i]->size();

		for (int j = 0; j < tmpcld->size(); j++)
		{
			qDebug() << tmpcld->size();
			PointXYZRGBObjectID tmp;
			tmp.x = tmpcld->points[j].x;
			tmp.y = tmpcld->points[j].y;
			tmp.z = tmpcld->points[j].z;
			tmp.rgb = tmpcld->points[j].rgb;
			tmp.obj_id = i;
			myObj->push_back(tmp);
		}
	}

	QString pcdFileRoot = output_root;
	io::savePCDFile(pcdFileRoot.toStdString(), *myObj);
	qDebug() << "Success";
	return(true);
}
