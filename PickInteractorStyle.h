#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

#include "mainwindow.h"

class PickInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static PickInteractorStyle* New();

	PickInteractorStyle(void);
	~PickInteractorStyle(void);

	void setPickMode(int mode);

	void setMainWindow(MainWindow *mainWindow);

	//void updateSelectedActor();

	virtual void OnLeftButtonDown();
	virtual void OnMouseMove();

	virtual void OnKeyPress();

public:

	// 0: do nothing
	// 1: 流程选点, 根据流程确定选点目的
	// 2: 测量选点, 点-点距离
	int mode;

	double currentSelectPosition[3];

	vtkSmartPointer<vtkPolyData> Data;
	vtkSmartPointer<vtkDataSetMapper> selectedMapper;
	vtkSmartPointer<vtkActor> selectedActor;

	MainWindow *mainWindow;
};
