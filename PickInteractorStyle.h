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
	// 1: ����ѡ��, ��������ȷ��ѡ��Ŀ��
	// 2: ����ѡ��, ��-�����
	int mode;

	double currentSelectPosition[3];

	vtkSmartPointer<vtkPolyData> Data;
	vtkSmartPointer<vtkDataSetMapper> selectedMapper;
	vtkSmartPointer<vtkActor> selectedActor;

	MainWindow *mainWindow;
};
