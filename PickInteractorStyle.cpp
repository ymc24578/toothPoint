#include "PickInteractorStyle.h"

#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellPicker.h>
#include <vtkIdTypeArray.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkCell.h>
#include <vtkMath.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>

#include <QMessageBox>

vtkStandardNewMacro(PickInteractorStyle);

PickInteractorStyle::PickInteractorStyle(void)
{
	selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	selectedActor = vtkSmartPointer<vtkActor>::New();

	mode = 0;
}

PickInteractorStyle::~PickInteractorStyle(void)
{
}

void PickInteractorStyle::setPickMode(int mode)
{
	this->mode = mode;
}

void PickInteractorStyle::setMainWindow(MainWindow *mainWindow)
{
	this->mainWindow = mainWindow;
}

void PickInteractorStyle::OnKeyPress()
{
	//vtkRenderWindowInteractor *rwi = this->Interactor;

	//if(rwi->GetControlKey())
	//{
	//	std::string key = rwi->GetKeySym();
	//	if(key == "z")
	//	{
	//		QMessageBox::information(NULL, QObject::tr("Key Pressed"), QObject::tr("ctrl+z pressed...")); 
	//		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(pointActor[selectPointNum - 1]);
	//	}
	//}

	vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void PickInteractorStyle::OnMouseMove()
{
	vtkInteractorStyleTrackballCamera::OnMouseMove();
}

void PickInteractorStyle::OnLeftButtonDown()
{
	//if(pickIndex > -1)
	if(mode == 1 || mode == 2 || mode == 3)
	{
		// Get the location of the click (in window coordinates)
		int* pos = this->GetInteractor()->GetEventPosition();

		vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
		picker->SetTolerance(0.0005);

		// Pick from this location.
		vtkRenderer *currentRenderer = this->GetDefaultRenderer();
		picker->Pick(pos[0], pos[1], 0, currentRenderer);

		double* worldPosition = picker->GetPickPosition();
		
		int cellId = picker->GetCellId();
		if(cellId != -1)
		{			
			vtkPoints *selectedCellPoints = picker->GetActor()->GetMapper()->GetInput()->GetCell(picker->GetCellId())->GetPoints();
			int selectedCellPointNum = selectedCellPoints->GetNumberOfPoints();
		
			int selectPointId = 0;
			double minDistance = 100000000;
			for (int i = 0; i < selectedCellPointNum; i++)
			{
				double distance = 0;

				double m[3];
				selectedCellPoints->GetPoint(i, m);

				distance = vtkMath::Distance2BetweenPoints(worldPosition, m);
				if(distance < minDistance)
				{
					minDistance = distance;
					selectPointId = i;
				}
			}
			
			//QMessageBox::information(NULL, QObject::tr("Point Index"), QString::number(pickIndex));
			//QMessageBox::information(NULL, QObject::tr("Info"), QString::number(mode));

			selectedCellPoints->GetPoint(selectPointId, currentSelectPosition);
			OperateItem It;
			It.Opetype = 0;
			It.OpePickMode = mode;
			It.OpeNum = this->mainWindow->returnselectOptionTreeIndex();
			It.x = currentSelectPosition[0];
			It.y = currentSelectPosition[1];
			It.z = currentSelectPosition[2];

			this->mainWindow->saveInput(It);

			if(mode == 1)
			{
				this->mainWindow->setPickPointPosition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);
			}
			else if(mode == 2)
			{
				this->mainWindow->setPickPointForMeasureDistancePointToPoint(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);				
			}
			else if (mode == 3)
			{
				this->mainWindow->setSegPickPointPoisition(currentSelectPosition[0], currentSelectPosition[1], currentSelectPosition[2]);		
			}

			vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
			ids->SetNumberOfComponents(1);
			ids->InsertNextValue(picker->GetCellId());

			vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
			selectionNode->SetFieldType(vtkSelectionNode::CELL);
			selectionNode->SetContentType(vtkSelectionNode::INDICES);
			selectionNode->SetSelectionList(ids);

			vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
			selection->AddNode(selectionNode);

			vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
#if VTK_MAJOR_VERSION <= 5
			extractSelection->SetInput(0, this->Data);
			extractSelection->SetInput(1, selection);
#else
			extractSelection->SetInputData(0, this->Data);
			extractSelection->SetInputData(1, selection);
#endif
			extractSelection->Update();

			// In selection
			vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
			selected->ShallowCopy(extractSelection->GetOutput());

#if VTK_MAJOR_VERSION <= 5
			selectedMapper->SetInputConnection(selected->GetProducerPort());
#else
			selectedMapper->SetInputData(selected);
#endif

			selectedActor->SetMapper(selectedMapper);
			selectedActor->GetProperty()->EdgeVisibilityOn();
			selectedActor->GetProperty()->SetEdgeColor(0, 1, 0);
			selectedActor->GetProperty()->SetLineWidth(1);

			this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(selectedActor);
		} // end if
	}

	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}
