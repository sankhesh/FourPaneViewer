#include "ui_QtVTKRenderWindows.h"
#include "QtVTKRenderWindows.h"
#include "QtVTKXMLEventSource.h"
#include "QtVTKXMLEventObserver.h"

#include <vtkBoundedPlanePointPlacer.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkDICOMImageReader.h>
#include <vtkDistanceRepresentation.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkDistanceWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkHandleRepresentation.h>
#include <vtkImageData.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageSlabReslice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceImageViewer.h>
#include <vtkResliceImageViewerMeasurements.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkResliceCursor.h>

#include <pqTestUtility.h>

#include <QFileDialog>
#include <QDir>
#include <QPixmap>
#include <QString>
#include <QMessageBox>
#include <QPropertyAnimation>

#include <sstream>

//----------------------------------------------------------------------------
class vtkResliceCursorCallback : public vtkCommand
{
public:
  static vtkResliceCursorCallback *New()
  { return new vtkResliceCursorCallback; }

  void Execute( vtkObject *caller, unsigned long ev,
                void *callData )
  {
    if (ev == vtkResliceCursorWidget::WindowLevelEvent ||
        ev == vtkCommand::WindowLevelEvent ||
        ev == vtkResliceCursorWidget::ResliceThicknessChangedEvent)
    {
      // Render everything
      for (int i = 0; i < 3; i++)
      {
        this->RCW[i]->Render();
      }
      this->IPW[0]->GetInteractor()->GetRenderWindow()->Render();
      return;
    }

    for (int i = 0; i < 3; i++)
    {
      vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
          this->IPW[i]->GetPolyDataAlgorithm());
      ps->SetOrigin(this->RCW[i]->GetResliceCursorRepresentation()->
                                        GetPlaneSource()->GetOrigin());
      ps->SetPoint1(this->RCW[i]->GetResliceCursorRepresentation()->
                                        GetPlaneSource()->GetPoint1());
      ps->SetPoint2(this->RCW[i]->GetResliceCursorRepresentation()->
                                        GetPlaneSource()->GetPoint2());

      // If the reslice plane has modified, update it on the 3D widget
      this->IPW[i]->UpdatePlacement();
    }

    // Render everything
    for (int i = 0; i < 3; i++)
    {
      this->RCW[i]->Render();
    }
    this->IPW[0]->GetInteractor()->GetRenderWindow()->Render();

    if (ev == vtkCommand::MouseMoveEvent)
    {
      // Get the event position from the interactor
      vtkInteractorStyleImage* style =
        dynamic_cast<vtkInteractorStyleImage*> (caller);
      vtkResliceImageViewer* riw = nullptr;
      if (style)
      {
        // Figure out the current viewer
        vtkCornerAnnotation* ca = nullptr;
        for (int i = 0; i < 3; ++i)
        {
          if (RIW[i]->GetInteractorStyle() == style)
          {
            riw = RIW[i];
            ca = CA[i];
            break;
          }
        }

        if (riw)
        {
          // Get event position in display coordinates
          int * eventPos = style->GetInteractor()->GetEventPosition();
          vtkRenderer* curRen =
            style->GetInteractor()->FindPokedRenderer(eventPos[0], eventPos[1]);

          vtkNew<vtkCellPicker> cellPicker;
          cellPicker->SetTolerance(0.005);
          cellPicker->AddPickList(riw->GetImageActor());
          cellPicker->PickFromListOn();
          cellPicker->Pick(eventPos[0], eventPos[1], 0, curRen);
          double* worldPtReslice = cellPicker->GetPickPosition();
          std::cout << "worldPtReslice " << worldPtReslice[0] << " " << worldPtReslice[1] << " " << worldPtReslice[2] << std::endl;

          // Get the (i,j,k) indices of the point in the original data
          double origin[3], spacing[3];
          data->GetOrigin(origin);
          data->GetSpacing(spacing);
          int pt[3];
          int extent[6];
          data->GetExtent(extent);

          int iq[3];
          int iqtemp;
          for (int i = 0; i < 3; i++)
          {
          // compute world to image coords
            iqtemp = vtkMath::Round((worldPtReslice[i]-origin[i])/spacing[i]);

          // we have a valid pick already, just enforce bounds check
            iq[i] = (iqtemp < extent[2*i])?extent[2*i]:((iqtemp > extent[2*i+1])?extent[2*i+1]:iqtemp);

            pt[i] = iq[i];
          }

          short * val = static_cast<short*> (data->GetScalarPointer(pt));

          std::ostringstream annotation;
          annotation << "(" << pt[0] << ", "<< pt[1] << ", " << pt[2] <<
            ") "  << val[0];
          std::cout << "(" << pt[0] << ", "<< pt[1] << ", " << pt[2] <<
            ") "  << val[0] << std::endl;
          ca->SetText(0, annotation.str().c_str());
          riw->Render();
        }

      style->OnMouseMove();
      }
    }

    vtkImagePlaneWidget* ipw =
      dynamic_cast< vtkImagePlaneWidget* >( caller );
    if (ipw)
    {
      double* wl = static_cast<double*>( callData );

      if ( ipw == this->IPW[0] )
      {
        this->IPW[1]->SetWindowLevel(wl[0],wl[1],1);
        this->IPW[2]->SetWindowLevel(wl[0],wl[1],1);
      }
      else if( ipw == this->IPW[1] )
      {
        this->IPW[0]->SetWindowLevel(wl[0],wl[1],1);
        this->IPW[2]->SetWindowLevel(wl[0],wl[1],1);
      }
      else if (ipw == this->IPW[2])
      {
        this->IPW[0]->SetWindowLevel(wl[0],wl[1],1);
        this->IPW[1]->SetWindowLevel(wl[0],wl[1],1);
      }
    }

  }

  vtkResliceCursorCallback() {}
  vtkImagePlaneWidget* IPW[3];
  vtkResliceCursorWidget *RCW[3];
  vtkResliceImageViewer* RIW[3];
  vtkCornerAnnotation* CA[3];
  vtkImageData* data;
};


QtVTKRenderWindows::QtVTKRenderWindows( int vtkNotUsed(argc), char *argv[])
{
  this->ui = new Ui_QtVTKRenderWindows;
  this->ui->setupUi(this);

  QObject::connect(ui->actionRecord, SIGNAL(triggered()), this, SLOT(record()));
  QObject::connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(play()));
  QObject::connect(
    ui->pushButton_Screenshot, SIGNAL(clicked()), this, SLOT(screenshot()));

  this->TestUtility = new pqTestUtility(this);
  this->TestUtility->addEventObserver("xml", new QtVTKXMLEventObserver(this));
  this->TestUtility->addEventSource("xml", new QtVTKXMLEventSource(this));

  vtkSmartPointer< vtkDICOMImageReader > reader =
    vtkSmartPointer< vtkDICOMImageReader >::New();
  reader->SetDirectoryName(argv[1]);
  reader->Update();
  int imageDims[3];
  reader->GetOutput()->GetDimensions(imageDims);

  for (int i = 0; i < 3; i++)
  {
    riw[i] = vtkSmartPointer< vtkResliceImageViewer >::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> window =
      vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    riw[i]->SetRenderWindow(window);
  }

  this->ui->view1->SetRenderWindow(riw[0]->GetRenderWindow());
  riw[0]->SetupInteractor(
      this->ui->view1->GetRenderWindow()->GetInteractor());

  this->ui->view2->SetRenderWindow(riw[1]->GetRenderWindow());
  riw[1]->SetupInteractor(
      this->ui->view2->GetRenderWindow()->GetInteractor());

  this->ui->view3->SetRenderWindow(riw[2]->GetRenderWindow());
  riw[2]->SetupInteractor(
      this->ui->view3->GetRenderWindow()->GetInteractor());

  for (int i = 0; i < 3; i++)
  {
    // make them all share the same reslice cursor object.
    vtkResliceCursorLineRepresentation *rep =
      vtkResliceCursorLineRepresentation::SafeDownCast(
          riw[i]->GetResliceCursorWidget()->GetRepresentation());
    riw[i]->SetResliceCursor(riw[0]->GetResliceCursor());

    rep->GetResliceCursorActor()->
      GetCursorAlgorithm()->SetReslicePlaneNormal(i);

    riw[i]->SetInputData(reader->GetOutput());
    riw[i]->SetSliceOrientation(i);
    riw[i]->SetResliceModeToAxisAligned();
  }

  vtkSmartPointer<vtkCellPicker> picker =
    vtkSmartPointer<vtkCellPicker>::New();
  picker->SetTolerance(0.005);

  vtkSmartPointer<vtkProperty> ipwProp =
    vtkSmartPointer<vtkProperty>::New();

  vtkSmartPointer< vtkRenderer > ren =
    vtkSmartPointer< vtkRenderer >::New();

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> window =
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  this->ui->view4->SetRenderWindow(window);
  window->AddRenderer(ren);
  vtkRenderWindowInteractor *iren = this->ui->view4->GetInteractor();

  for (int i = 0; i < 3; i++)
  {
    planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
    planeWidget[i]->SetInteractor( iren );
    planeWidget[i]->SetPicker(picker);
    planeWidget[i]->RestrictPlaneToVolumeOn();
    double color[3] = {0, 0, 0};
    color[i] = 1;
    planeWidget[i]->GetPlaneProperty()->SetColor(color);

    color[0] /= 4.0;
    color[1] /= 4.0;
    color[2] /= 4.0;
    riw[i]->GetRenderer()->SetBackground( color );

    planeWidget[i]->SetTexturePlaneProperty(ipwProp);
    planeWidget[i]->TextureInterpolateOff();
    planeWidget[i]->SetResliceInterpolateToLinear();
    planeWidget[i]->SetInputConnection(reader->GetOutputPort());
    planeWidget[i]->SetPlaneOrientation(i);
    planeWidget[i]->SetSliceIndex(imageDims[i]/2);
    riw[i]->SetSlice(imageDims[i]/2);
    planeWidget[i]->DisplayTextOn();
    planeWidget[i]->SetDefaultRenderer(ren);
    planeWidget[i]->SetWindowLevel(1358, -27);
    planeWidget[i]->On();
    planeWidget[i]->InteractionOn();
  }

  // HACK: equalize the data spacing in X and Y - otherwise leads to rounding
  // off errors when transforming from world to image IJK coordinates.
  vtkImageData* data = reader->GetOutput();
  double spacing[3];
  data->GetSpacing(spacing);
  if (std::fabs(spacing[0] - spacing[1]) < 1e-5)
  {
    spacing[1] = spacing[0];
    data->SetSpacing(spacing);
  }
  vtkSmartPointer<vtkResliceCursorCallback> cbk =
    vtkSmartPointer<vtkResliceCursorCallback>::New();
  cbk->data = reader->GetOutput();

  for (int i = 0; i < 3; i++)
  {
    cbk->IPW[i] = planeWidget[i];
    cbk->RIW[i] = riw[i];
    cbk->RCW[i] = riw[i]->GetResliceCursorWidget();
    riw[i]->GetResliceCursorWidget()->AddObserver(
        vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk );
    riw[i]->GetResliceCursorWidget()->AddObserver(
        vtkResliceCursorWidget::WindowLevelEvent, cbk );
    riw[i]->GetResliceCursorWidget()->AddObserver(
        vtkResliceCursorWidget::ResliceThicknessChangedEvent, cbk );
    riw[i]->GetResliceCursorWidget()->AddObserver(
        vtkResliceCursorWidget::ResetCursorEvent, cbk );
    riw[i]->GetInteractorStyle()->AddObserver(
        vtkCommand::WindowLevelEvent, cbk );
    riw[i]->GetInteractorStyle()->AddObserver(
        vtkCommand::MouseMoveEvent, cbk);

    // Make them all share the same color map.
    riw[i]->SetLookupTable(riw[0]->GetLookupTable());
    planeWidget[i]->GetColorMap()->SetLookupTable(riw[0]->GetLookupTable());
    //planeWidget[i]->GetColorMap()->SetInput(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap()->GetInput());
    planeWidget[i]->SetColorMap(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());

    vtkSmartPointer<vtkCornerAnnotation> cornerText =
      vtkSmartPointer<vtkCornerAnnotation>::New();
    cornerText->SetText(0, "(X, Y, Z) I");
    cornerText->GetTextProperty()->SetColor(0.6, 0.5, 0.1);
    riw[i]->GetRenderer()->AddViewProp(cornerText);
    cbk->CA[i] = cornerText;
  }

  this->ui->view1->show();
  this->ui->view2->show();
  this->ui->view3->show();

  // Set up action signals and slots
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
  connect(this->ui->resliceModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));
  connect(this->ui->thickModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(thickMode(int)));
  this->ui->thickModeCheckBox->setEnabled(0);

  connect(this->ui->radioButton_Max, SIGNAL(pressed()), this, SLOT(SetBlendModeToMaxIP()));
  connect(this->ui->radioButton_Min, SIGNAL(pressed()), this, SLOT(SetBlendModeToMinIP()));
  connect(this->ui->radioButton_Mean, SIGNAL(pressed()), this, SLOT(SetBlendModeToMeanIP()));
  this->ui->blendModeGroupBox->setEnabled(0);

  connect(this->ui->resetButton, SIGNAL(pressed()), this, SLOT(ResetViews()));
  connect(this->ui->AddDistance1Button, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToView1()));

  this->ResetViews();
};

void QtVTKRenderWindows::slotExit()
{
  qApp->exit();
}

void QtVTKRenderWindows::resliceMode(int mode)
{
  this->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
  this->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

  for (int i = 0; i < 3; i++)
  {
    double ps = riw[i]->GetRenderer()->GetActiveCamera()->GetParallelScale();
    riw[i]->SetResliceMode(mode ? 1 : 0);
    riw[i]->GetRenderer()->ResetCamera();
    riw[i]->GetRenderer()->GetActiveCamera()->SetParallelScale(ps);
    riw[i]->Render();
  }
}

void QtVTKRenderWindows::thickMode(int mode)
{
  for (int i = 0; i < 3; i++)
  {
    riw[i]->SetThickMode(mode ? 1 : 0);
    riw[i]->Render();
  }
}

void QtVTKRenderWindows::SetBlendMode(int m)
{
  for (int i = 0; i < 3; i++)
  {
    vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(
        vtkResliceCursorThickLineRepresentation::SafeDownCast(
          riw[i]->GetResliceCursorWidget()->GetRepresentation())->GetReslice());
    thickSlabReslice->SetBlendMode(m);
    riw[i]->Render();
  }
}

void QtVTKRenderWindows::SetBlendModeToMaxIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

void QtVTKRenderWindows::SetBlendModeToMinIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

void QtVTKRenderWindows::SetBlendModeToMeanIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

void QtVTKRenderWindows::ResetViews()
{
  // Reset the reslice image views
  for (int i = 0; i < 3; i++)
  {
    riw[i]->Reset();
  }

  // Also sync the Image plane widget on the 3D top right view with any
  // changes to the reslice cursor.
  for (int i = 0; i < 3; i++)
  {
    vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
        planeWidget[i]->GetPolyDataAlgorithm());
    ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
    ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());

    // If the reslice plane has modified, update it on the 3D widget
    this->planeWidget[i]->UpdatePlacement();
  }

  // Render in response to changes.
  this->Render();
}

void QtVTKRenderWindows::Render()
{
  for (int i = 0; i < 3; i++)
  {
    riw[i]->Render();
  }
  this->ui->view3->GetRenderWindow()->Render();
}

void QtVTKRenderWindows::AddDistanceMeasurementToView1()
{
  this->AddDistanceMeasurementToView(1);
}

void QtVTKRenderWindows::AddDistanceMeasurementToView(int i)
{
  // remove existing widgets.
  if (this->DistanceWidget[i])
  {
    this->DistanceWidget[i]->SetEnabled(0);
    this->DistanceWidget[i] = NULL;
  }

  // add new widget
  this->DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
  this->DistanceWidget[i]->SetInteractor(
    this->riw[i]->GetResliceCursorWidget()->GetInteractor());

  // Set a priority higher than our reslice cursor widget
  this->DistanceWidget[i]->SetPriority(
    this->riw[i]->GetResliceCursorWidget()->GetPriority() + 0.01);

  vtkSmartPointer< vtkPointHandleRepresentation2D > handleRep =
    vtkSmartPointer< vtkPointHandleRepresentation2D >::New();
  vtkSmartPointer< vtkDistanceRepresentation2D > distanceRep =
    vtkSmartPointer< vtkDistanceRepresentation2D >::New();
  distanceRep->SetHandleRepresentation(handleRep);
  this->DistanceWidget[i]->SetRepresentation(distanceRep);
  distanceRep->InstantiateHandleRepresentation();
  distanceRep->GetPoint1Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
  distanceRep->GetPoint2Representation()->SetPointPlacer(riw[i]->GetPointPlacer());

  // Add the distance to the list of widgets whose visibility is managed based
  // on the reslice plane by the ResliceImageViewerMeasurements class
  this->riw[i]->GetMeasurements()->AddItem(this->DistanceWidget[i]);

  this->DistanceWidget[i]->CreateDefaultRepresentation();
  this->DistanceWidget[i]->EnabledOn();
}

void QtVTKRenderWindows::record()
{
  QString filename = QFileDialog::getSaveFileName (this, "Test File Name",
    QString(), "XML Files (*.xml)");
  if (!filename.isEmpty())
    {
    QApplication::setActiveWindow(this);
    this->TestUtility->recordTests(filename);
    }
}

void QtVTKRenderWindows::play()
{
  QString filename = QFileDialog::getOpenFileName (this, "Test File Name",
    QString(), "XML Files (*.xml)");
  if (!filename.isEmpty())
    {
    this->TestUtility->playTests(filename);
    }
}

void QtVTKRenderWindows::screenshot()
{
  const QDateTime now = QDateTime::currentDateTime();
  const QString timestamp = now.toString(QLatin1String("yyyyMMdd-hhmmsszzz"));
  const QString filename =
    QString::fromLatin1("Screenshot-%1.png").arg(timestamp);
  QPropertyAnimation* fadeInAnimation =
    new QPropertyAnimation(this, "windowOpacity");
  fadeInAnimation->setDuration(50);
  fadeInAnimation->setStartValue(0.0);
  fadeInAnimation->setEndValue(1.0);
  fadeInAnimation->start();
  QApplication::beep();
  QPixmap pm = this->grab();
  if (!pm.save(filename))
    {
      QMessageBox::warning(this,
                           tr("Screenshot error"),
                           tr("Image \"%1\" could not be saved.")
                             .arg(QDir::toNativeSeparators(filename)));
    }
}
