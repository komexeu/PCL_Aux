#pragma once

//stl
#include <vector>
//qt
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>

class IQT_ToolBarManager
{
	virtual QToolButton* AddToolButton(const char* iconRoot, QString toolButtonName) = 0;
};

class QT_ToolBarManager :IQT_ToolBarManager
{
public:
	QT_ToolBarManager(QMainWindow *w) :IQT_ToolBarManager(), tool_bar_()
	{
		tool_bar_ = new QToolBar(w);
		tool_bar_->setObjectName("toollBarObj");
		w->addToolBar(tool_bar_);
	}

	QToolButton* AddToolButton(const char* iconRoot, QString toolButtonName);
private:
	std::vector<QToolButton*> tool_button_manager_;
	QToolBar *tool_bar_;
};