#include "QT_ToolBarManager.h"

QToolButton* QT_ToolBarManager::AddToolButton(const char* iconRoot,QString toolButtonName) {
	QToolButton *qt_toolButton=new QToolButton(NULL);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(iconRoot), QSize(), QIcon::Normal, QIcon::Off);
	qt_toolButton->setObjectName(toolButtonName);
	qt_toolButton->setIcon(icon1);
	tool_bar_->addWidget(qt_toolButton);
	tool_button_manager_.push_back(qt_toolButton);

	return qt_toolButton;
}