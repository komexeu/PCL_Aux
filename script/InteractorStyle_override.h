#pragma once
#include <pcl/visualization/pcl_visualizer.h>

#include <vtkRenderWindowInteractor.h>
#include <qdebug.h>
#include <pcl/visualization/point_picking_event.h>
#include <pcl/visualization/area_picking_event.h>
#include <pcl/visualization/keyboard_event.h>
#include <pcl/visualization/mouse_event.h>
#include <vtkInteractorStyleRubberBandPick.h>

#include <vtkPointPicker.h>
#include <vtkAreaPicker.h>

#define DEFAULT_SELECT_MODE 0
#define AREA_SELECT_MODE 1

class InteractorStyle_override :public pcl::visualization::PCLVisualizerInteractorStyle
{
public:
	static InteractorStyle_override *New();
	vtkTypeMacro(InteractorStyle_override, PCLVisualizerInteractorStyle);

	InteractorStyle_override() { 
		point_picker_ = vtkSmartPointer<vtkPointPicker>::New();
		PCLVisualizerInteractorStyle();
	}

	void
		OnChar() override {
		FindPokedRenderer(Interactor->GetEventPosition()[0], Interactor->GetEventPosition()[1]);

		switch (Interactor->GetKeyCode()) {
		case 'r': case 'R':
		case 'x': case 'X':
		case 'e': case 'E':
		case 'b': case 'B': {
			break;
		}
		default:
		{
			Superclass::OnChar();
			break;
		}
		}
	}

	// Keyboard events
	void
		OnKeyDown() override {
		qDebug() << "HIIII";
		switch (Interactor->GetKeyCode()) {
		case 'x':case'X': {
			qDebug() << "PRESS XXXXXXXXXXXXXXXX";
			CurrentMode = (CurrentMode != AREA_SELECT_MODE) ? AREA_SELECT_MODE : DEFAULT_SELECT_MODE;
			if (CurrentMode == AREA_SELECT_MODE)
			{
				point_picker_ = static_cast<vtkPointPicker*> (Interactor->GetPicker());
				vtkSmartPointer<vtkAreaPicker> area_picker = vtkSmartPointer<vtkAreaPicker>::New();
				Interactor->SetPicker(area_picker);
			}
			else
			{
				Interactor->SetPicker(point_picker_);
			}
			break;
		}
		case 'b':case'B': {
			qDebug() << "PRESS B";

			CurrentMode = DEFAULT_SELECT_MODE;
			//vtkSmartPointer<vtkPointPicker> point_picker = vtkSmartPointer<vtkPointPicker>::New();
			Interactor->SetPicker(point_picker_);
			qDebug() << "Point Picker";
			break;
		}
		case 'r':case'R': {
			qDebug() << "PRESS R";
			break;
		}
		default:
		{
			Superclass::OnKeyDown();
			break;
		}
		}

		pcl::visualization::KeyboardEvent event(true, Interactor->GetKeySym(),
			Interactor->GetKeyCode(), Interactor->GetAltKey(), Interactor->GetControlKey(), Interactor->GetShiftKey());
		qDebug() << event.getKeySym().c_str();
		keyboard_signal_(event);
	}

	void
		OnKeyUp() override {
		qDebug() << "KEY UP.";

		Superclass::OnKeyUp();
	}

	// mouse button events
	void
		OnMouseMove() override {
		//qDebug() << "mouse MOVE.";
		Superclass::OnMouseMove();
	}

	void
		OnLeftButtonDown() override {
		qDebug() << "Pressed left mouse button DOWN.";
		Superclass::OnLeftButtonDown();
	}

	void
		OnLeftButtonUp() override {
		qDebug() << "Pressed left mouse button UP.";
		Superclass::OnLeftButtonUp();
	}

	void
		OnMiddleButtonDown() override {
		qDebug() << "Pressed left mouse MIDDLE button DOWN.";
		Superclass::OnMiddleButtonDown();
	}

	void
		OnMiddleButtonUp() override { Superclass::OnMiddleButtonUp(); }
	void
		OnRightButtonDown() override { Superclass::OnRightButtonDown(); }
	void
		OnRightButtonUp() override { Superclass::OnRightButtonUp(); }
	void
		OnMouseWheelForward() override { Superclass::OnMouseWheelForward(); }
	void
		OnMouseWheelBackward() override { Superclass::OnMouseWheelBackward(); }
	
};

vtkStandardNewMacro(InteractorStyle_override);