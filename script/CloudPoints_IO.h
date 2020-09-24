#pragma once

//qt
#include <qstring.h>
#include <qfiledialog.h>
#include <qlocale.h>
#include <qtextstream.h>
#include <qobject.h>

//pcl
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/filters/voxel_grid.h>

using namespace pcl;

struct PointXYZRGBObjectID
{
	union
	{
		float coordinate[5];
		struct
		{
			float x;
			float y;
			float z;
		};
	};
	union
	{
		/*struct {
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		uint32_t rgb= ((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b);*/
		float rgb;
	};
	union
	{
		int obj_id;
	};
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
}EIGEN_ALIGN16;
POINT_CLOUD_REGISTER_POINT_STRUCT(PointXYZRGBObjectID,
(float, x, x)
(float, y, y)
(float, z, z)
(float, rgb, rgb)
(int, obj_id, obj_id)
)

class ICloudPoints_IO
{
public:
	virtual bool CloudImport() = 0;
	virtual bool CloudExport(std::vector<PointCloud<PointXYZRGB>::Ptr> childern_cloudData) = 0;
private:

};

class CloudPoints_IO :ICloudPoints_IO
{
public:
	CloudPoints_IO() : ICloudPoints_IO(), import_cloud_()
	{
		import_cloud_.reset(new PointCloud<PointXYZRGB>);
	};

	//�I���פJ
	bool CloudImport();
	//�I���ץX
	bool CloudExport(std::vector<PointCloud<PointXYZRGB>::Ptr> childern_cloudData);

	//�פJ�ɮ��ഫ���I��
	PointCloud<PointXYZRGB>::Ptr import_cloud_;
	std::string file_name_;
protected:
	//�ɮ׸��|
	QString q_file_path_;
	std::string file_path_;
	//���ɦW
	std::string suffix_;
private:
	bool RootSelector();
	bool csv2pointCloud();
};