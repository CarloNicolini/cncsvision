// This file is part of CNCSVision, a computer vision related library
// This software is developed under the grant of Italian Institute of Technology
//
// Copyright (C) 2010-2014 Carlo Nicolini <carlo.nicolini@iit.it>
//
//
// CNCSVision is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CNCSVision is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// CNCSVision. If not, see <http://www.gnu.org/licenses/>.

#include <QObject>
#include <QListWidget>
#include <QtGui>
#include <QSlider>
#include "MainWindow.h"
#include "ui_MainWindow.h"

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent ) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //cerr << "[MAINWINDOW] Constructor" << endl;
    ui->setupUi(this);
    cerr << "[MAINWINDOW] User interface successfully setup" << endl;

    this->initializeTabSceneQConnections();
    this->initializeTabCameraCalibrationQConnections();
    this->initializeTabProjectorQConnections();
    cerr << "[MAINWINDOW] QtConnections done" << endl;

    // Initialization of spinboxes eccetera
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue(100);
    this->ui->spinBoxProjectorNSlices->setValue(PROJECTOR_SLICES_NUMBER);

    // Initialization of OpenGL Widget scene
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),ui->doubleSpinboxCameraZNear->value(),ui->doubleSpinboxCameraZFar->value());

    this->update();
}

/**
 * @brief MainWindow::initializeTabCameraCalibrationQConnections
 */
void MainWindow::initializeTabCameraCalibrationQConnections()
{
    QObject::connect(this->ui->doubleSpinBoxCameraFOV,SIGNAL(valueChanged(double)),this,SLOT(onCameraFOVChanged(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZNear,SIGNAL(valueChanged(double)),this,SLOT(onCameraZNearChanged(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZFar,SIGNAL(valueChanged(double)),this,SLOT(onCameraZFarChanged(double)));

    QObject::connect(this->ui->pushButtonCalibrate,SIGNAL(clicked()),this,SLOT(onPushbuttonCalibrateClicked()));
}

/**
 * @brief MainWindow::initializeTabProjectorQConnections
 */
void MainWindow::initializeTabProjectorQConnections()
{
    // Projector tab connections
    QObject::connect(this->ui->pushButtonProjectorStartProjection,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorStartProjectionClicked()));
    QObject::connect(this->ui->pushButtonProjectorStopProjection,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorStopProjectionClicked()));
    QObject::connect(this->ui->pushButtonProjectorInitialize,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorInitializeClicked()));
    QObject::connect(this->ui->pushButtonProjectorRelease,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorReleaseClicked()));
    QObject::connect(this->ui->spinBoxProjectorNSlices,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxProjectorNSlicesChanged(int)));
    QObject::connect(this->ui->pushButtonUploadCurrentSequence,SIGNAL(clicked()),this,SLOT(onPushButtonUploadSequenceClicked()));
    QObject::connect(this->ui->pushButtonProjectorChangeSequence,SIGNAL(clicked()),this,SLOT(onPushButtonSequenceChanged()));
    QObject::connect(this->ui->pushButtonRemoveSequence,SIGNAL(clicked()),this,SLOT(onPushButtonRemoveSequencePressed()));

    // Connect microsecPerFrame spinbox to slot that changes the microseconds per round given the number of slices
    QObject::connect(this->ui->spinBoxProjectorMicrosecondsPerFrame,SIGNAL(valueChanged(int)),this,SLOT(onSpinBoxProjectorMicrosecondsPerFrameChanged(int)));
    // Connect microsecPerFrame spinbox to slot that changes the microseconds per round given the number of slices
    QObject::connect(this->ui->spinBoxMicrosecondsPerRound,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxMicrosecondsPerRoundChanged(int)));

    // Connections for LED current contorol
    QObject::connect(this->ui->spinBoxLEDcurrent,SIGNAL(valueChanged(int)),this,SLOT(onSpinBoxLEDcurrentChanged(int)));
    QObject::connect(this->ui->doubleSpinBoxLEDpercentage,SIGNAL(valueChanged(double)),this,SLOT(onSpinBoxLEDpercentageChanged(double)));

    // When microseconds per frame are changed, then microseconds per round is changed
    QObject::connect(this->ui->spinBoxMicrosecondsPerRound,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxMicrosecondsPerRoundChanged(int)));

    this->repaint();
}

/**
 * @brief MainWindow::initializeTabSceneQConnections
 */
void MainWindow::initializeTabSceneQConnections()
{
    // Scene tab connections
    QObject::connect(this->ui->doubleSpinBoxOffsetX,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetXChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxOffsetY,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetYChanged(double)));
    QObject::connect(this->ui->doubleSpinBoxOffsetZ,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxOffsetZChanged(double)));

    QObject::connect(this->ui->doubleSpinboxHelicoidCxMm,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxHelicoidXChanged(double)));
    QObject::connect(this->ui->doubleSpinboxHelicoidCyMm,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxHelicoidYChanged(double)));
    QObject::connect(this->ui->doubleSpinboxHelicoidCzMm,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxHelicoidZChanged(double)));


    QObject::connect(this->ui->doubleSpinBoxObjectSize,SIGNAL(valueChanged(double)),this,SLOT(onDoubleSpinboxObjectSizeChanged(double)));
    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(eyeZChanged(double)),this->ui->doubleSpinBoxCameraEyeZ,SLOT(setValue(double)));
    QObject::connect(this->ui->doubleSpinBoxCameraEyeZ,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(setEyeZ(double)));
    // Stimulus tab connections
    QObject::connect(this->ui->pushButtonStimulusRandomizeSpheres,SIGNAL(clicked()),this,SLOT(onPushbuttonRandomizeSpheresPressed()));
    QObject::connect(this->ui->pushButtonGenerateFrames,SIGNAL(clicked()),this,SLOT(onPushButtonGenerateFramesPressed()));

    // Enable/disable offscreen rendering
    QObject::connect(this->ui->checkBoxUseOffscreenRendering,SIGNAL(clicked(bool)),this->ui->volumetricGLWidget,SLOT(setOffscreenRendering(bool)));
    QObject::connect(this->ui->pushButtonLoadBinVox,SIGNAL(clicked(bool)),this,SLOT(onPushbuttonLoadBinVoxPressed()));

    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(binVoxLoaded(QString)),this->ui->labelTextureInfo,SLOT(setText(QString)));
    // Connection for filtered parametric surface thickness
    QObject::connect(this->ui->doubleSpinBoxSurfaceThickness,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(onSurfaceThicknessChanged(double)));
    QObject::connect(this->ui->pushButtonSaveParameters,SIGNAL(clicked(bool)),this->ui->volumetricGLWidget,SLOT(saveParameters()));
}

/**
 * @brief MainWindow::onPushbuttonLoadBinVoxPressed
 */
void MainWindow::onPushbuttonLoadBinVoxPressed()
{
    QString binvoxfile = QFileDialog::getOpenFileName(this,"Select 2D points file",QDir::currentPath(),"*.binvox");
    if (binvoxfile.isEmpty())
        return;
    try
    {
        this->ui->volumetricGLWidget->loadBinvox(binvoxfile.toStdString());
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in loading binvox file",QString(e.what()));
    }
}

/**
 * @brief MainWindow::onPushButtonSaveParametersPressed
 */
void MainWindow::onPushButtonSaveParametersPressed()
{
    try
    {
        this->ui->volumetricGLWidget->saveParameters();
    }
    catch ( std::exception &e)
    {
        QMessageBox::warning(this,"Warning I/O error!",e.what());
        return;
    }
}

void MainWindow::onCameraFOVChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(val,ui->doubleSpinboxCameraZNear->value(),ui->doubleSpinboxCameraZFar->value());
}

void MainWindow::onCameraZNearChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),val,ui->doubleSpinboxCameraZFar->value());
}

void MainWindow::onCameraZFarChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),ui->doubleSpinboxCameraZNear->value(),val);
}
/**
* @brief onSpinBoxProjectorMicrosecondsPerFrameChanged
**/

void MainWindow::onSpinBoxProjectorMicrosecondsPerFrameChanged(int value)
{
    this->ui->spinBoxMicrosecondsPerRound->setValue( std::ceil( (double)value*this->ui->spinBoxProjectorNSlices->value()) );
}

/**
* @brief onSpinboxMicrosecondsPerRoundChanged
**/
void MainWindow::onSpinboxMicrosecondsPerRoundChanged(int value)
{
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue( std::ceil( (double)value/this->ui->spinBoxProjectorNSlices->value()) );
}


/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{

}

/**
 * @brief Window::eventFilter
 * @return
 */
bool MainWindow::eventFilter(QObject *, QEvent *)
{
    return false;
}

/**
 * @brief MainWindow::keyPressEvent
 * @param e
 */
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Q )
        close();
    else
        QWidget::keyPressEvent(e);
}

/**
 * @brief MainWindow::onPushButtonGenerateFramesPressed
 */
void MainWindow::onPushButtonGenerateFramesPressed()
{
    if (this->ui->checkBoxUseOffscreenRendering->isChecked())
    {
        QObject::connect(this->ui->volumetricGLWidget,SIGNAL(memoryAllocatedMegaBytes(int)),this->ui->spinBoxMemoryAllocated,SLOT(setValue(int)));
    }
    this->ui->statusbar->showMessage("Generating "+QString::number(ui->volumetricGLWidget->slicesNumber)+ " frames...");
    this->ui->volumetricGLWidget->generateFrames();
    this->ui->statusbar->showMessage("Frames generation done",2000);
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetXChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetXChanged(double value)
{
    this->ui->volumetricGLWidget->setObjectOffset(value,ui->doubleSpinBoxOffsetY->value(),ui->doubleSpinBoxOffsetZ->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetYChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetYChanged(double value)
{
    this->ui->volumetricGLWidget->setObjectOffset(ui->doubleSpinBoxOffsetX->value(),value,ui->doubleSpinBoxOffsetZ->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxOffsetZChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxOffsetZChanged(double value)
{
    this->ui->volumetricGLWidget->setObjectOffset(ui->doubleSpinBoxOffsetX->value(),ui->doubleSpinBoxOffsetY->value(),value);
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxHelicoidXChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxHelicoidXChanged(double value)
{
    this->ui->volumetricGLWidget->setHelicoidOffset(value,ui->doubleSpinboxHelicoidCyMm->value(),ui->doubleSpinboxHelicoidCzMm->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxHelicoidYChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxHelicoidYChanged(double value)
{
    this->ui->volumetricGLWidget->setHelicoidOffset(ui->doubleSpinboxHelicoidCxMm->value(),value,ui->doubleSpinboxHelicoidCzMm->value());
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxHelicoidYChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxHelicoidZChanged(double value)
{
    this->ui->volumetricGLWidget->setHelicoidOffset(ui->doubleSpinboxHelicoidCxMm->value(),ui->doubleSpinboxHelicoidCyMm->value(),value);
    this->ui->volumetricGLWidget->update();
}

/**
 * @brief MainWindow::onDoubleSpinboxObjectSizeChanged
 * @param value
 */
void MainWindow::onDoubleSpinboxObjectSizeChanged(double value)
{
    this->ui->volumetricGLWidget->setObjectScale(value);
}

/**
 * @brief MainWindow::onSpinboxNSlicesChanged
 */
void MainWindow::onSpinboxProjectorNSlicesChanged(int value)
{
    this->ui->volumetricGLWidget->setSlicesNumber(value);
    this->ui->spinBoxMicrosecondsPerRound->setValue(value*ui->spinBoxProjectorMicrosecondsPerFrame->value());
}

/**
 * @brief MainWindow::onPushbuttonRandomizeSpheresPressed
 */
void MainWindow::onPushbuttonRandomizeSpheresPressed()
{
    this->ui->volumetricGLWidget->randomizeSpheres(ui->spinBoxStimulusNSpheres->value(),ui->spinBoxStimulusSpheresRadiusMin->value(),ui->spinBoxStimulusSpheresRadiusMax->value());
}

/**
 * @brief MainWindow::on_spinBoxSpheresRadiusMin_valueChanged
 * @param arg1
 */
void MainWindow::onSpinboxspheresradiusminValuechanged(int arg1)
{
    if (arg1 >= this->ui->spinBoxStimulusSpheresRadiusMax->value() )
    {
        this->ui->spinBoxStimulusSpheresRadiusMax->setValue(arg1+1);
    }

}

/**
 * @brief MainWindow::on_spinBoxSpheresRadiusMax_valueChanged
 * @param arg1
 */
void MainWindow::onSpinboxspheresradiusmaxValuechanged(int arg1)
{
    if (arg1 < this->ui->spinBoxStimulusSpheresRadiusMin->value() )
    {
        this->ui->spinBoxStimulusSpheresRadiusMin->setValue(arg1-1);
    }
}

/**
 * @brief MainWindow::on_checkBoxCameraViewMode_clicked
 * @param checked
 */
void MainWindow::onCheckboxcameraviewmodeClicked(bool checked)
{
    this->ui->volumetricGLWidget->toggleStandardGL(checked);
}


/**
 * @brief MainWindow::on_checkBoxUseCalibratedView_clicked
 * @param checked
 */
void MainWindow::onCheckboxusecalibratedviewClicked(bool checked)
{
    this->ui->volumetricGLWidget->toggleUseCalibratedGLView();
}

void MainWindow::onPushButtonUploadSequenceClicked()
{
    this->ui->statusbar->showMessage("Uploading sequence...");
#ifdef ALP_SUPPORT
    try
    {
        int nSlices = this->ui->spinBoxProjectorNSlices->value();
        unsigned char *data = this->ui->volumetricGLWidget->allFrames.data();
        long int pictureTimeMs = alp.getMinimumPictureTime();
        if (alp.m_bAlpInit)
            this->ui->spinBoxProjectorMicrosecondsPerFrame->setMinimum(pictureTimeMs);
        alp.setPicturesTimeus(this->ui->spinBoxProjectorMicrosecondsPerFrame->value());
        alp.loadSequence(nSlices,data);
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP projector",QString(e.what()));
    }
    this->ui->statusbar->showMessage("Sequence successfully uploaded, sequence ID="+QString::number(alp.m_AlpSeqDisp),5000);
    this->ui->listWidgetSequences->clear();
    for (unsigned int i=0; i<alp.m_AlpSeqId.size(); ++i)
        this->ui->listWidgetSequences->addItem(QString::number(alp.m_AlpSeqId.at(i)));
#endif
}

/**
* @brief onPushButtonSequenceChanged
**/
void MainWindow::onPushButtonSequenceChanged()
{
    if( ui->listWidgetSequences->selectedItems().isEmpty() )
        return;
    int sequenceToDisplay = ui->listWidgetSequences->currentItem()->text().toLong();
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        try
        {
            alp.changeSequence(sequenceToDisplay);
            if (alp.m_bDisp)
            {
                alp.stop();
                alp.start();
            }
        }
        catch (const std::exception &e)
        {
            QMessageBox::warning(this,"Error in ALP projector, changing sequence",QString(e.what()));
        }
    }
#endif
}

/**
* @brief onPushButtonRemoveSequencePressed
**/
void MainWindow::onPushButtonRemoveSequencePressed()
{
    if( ui->listWidgetSequences->selectedItems().isEmpty() )
        return;
    int sequenceToRemove = ui->listWidgetSequences->currentItem()->text().toLong();
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        try
        {
            if (alp.m_bDisp)
                alp.stop();
            alp.removeSequence(sequenceToRemove);
            this->ui->listWidgetSequences->clear();
            for (unsigned int i=0; i<alp.m_AlpSeqId.size(); ++i)
                this->ui->listWidgetSequences->addItem(QString::number(alp.m_AlpSeqId.at(i)));
        }
        catch (const std::exception &e)
        {
            QMessageBox::warning(this,"Error in ALP projector, removing sequence",QString(e.what()));
        }
    }
#endif
}

/**
 * @brief MainWindow::onPushButtonProjectorStartProjectionClicked
 */
void MainWindow::onPushButtonProjectorStartProjectionClicked()
{
#ifdef ALP_SUPPORT
    try
    {
        alp.start();
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP projector, starting sequence",QString(e.what()));
    }
#endif
}

void MainWindow::onPushButtonProjectorReleaseClicked()
{
#ifdef ALP_SUPPORT
    try
    {
        alp.cleanup();
        this->ui->spinBoxLEDcurrent->setEnabled(false);
        this->ui->doubleSpinBoxLEDpercentage->setEnabled(false);
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP projector",QString(e.what()));
    }
#endif
    this->ui->listWidgetSequences->clear();
}

/**
* @brief onSpinBoxLEDpercentageChanged
**/
void MainWindow::onSpinBoxLEDpercentageChanged(double percentage)
{
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        if (!alp.m_bAlpLEDInit)
        {
            alp.initLED();
            alp.setLED(ui->spinBoxLEDcurrent->value(),static_cast<long int>(std::ceil(percentage)));
        }
        else
            alp.setLED(ui->spinBoxLEDcurrent->value(),static_cast<long int>(std::ceil(percentage)));
    }
#endif
}

/**
* @brief onSpinBoxLEDcurrentChanged
**/
void MainWindow::onSpinBoxLEDcurrentChanged(int current)
{
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        if (!alp.m_bAlpLEDInit)
        {
            alp.initLED();
            alp.setLED(current,ui->doubleSpinBoxLEDpercentage->value());
        }
        else
            alp.setLED(current,ui->doubleSpinBoxLEDpercentage->value());
    }
#endif
}

/**
 * @brief MainWindow::onPushButtonProjectorInitializeClicked
 */
void MainWindow::onPushButtonProjectorInitializeClicked()
{
    this->ui->spinBoxLEDcurrent->setEnabled(true);
    this->ui->doubleSpinBoxLEDpercentage->setEnabled(true);

#ifdef ALP_SUPPORT
    int nSlices = this->ui->spinBoxProjectorNSlices->value();
    unsigned char *data = this->ui->volumetricGLWidget->allFrames.data();
    try
    {
        alp.init();
        alp.initLED();
        alp.setLED(this->ui->spinBoxLEDcurrent->value(),ui->doubleSpinBoxLEDpercentage->value());
        alp.inquire();
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP initialization",QString(e.what()));
    }
#endif
}

/**
 * @brief MainWindow::onPushButtonProjectorStopProjectionClicked
 */
void MainWindow::onPushButtonProjectorStopProjectionClicked()
{
    try
    {
#ifdef ALP_SUPPORT
        alp.stop();
#endif
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP projector",QString(e.what()));
    }
}

/**
 * @brief MainWindow::onPushbuttonCalibrateClicked
 */
void MainWindow::onPushbuttonCalibrateClicked()
{
    QString points2Dfilename = QFileDialog::getOpenFileName(this,"Select 2D points file",QDir::currentPath(),"*.txt");
    QString points3Dfilename = QFileDialog::getOpenFileName(this,"Select 3D points file",QDir::currentPath(),"*.txt");

    //  this->ui->volumetricGLWidget->computeCameraCalibrationMatrices(points2Dfilename,points3Dfilename);
    this->ui->volumetricGLWidget->computeCameraCalibrationMatrices("/home/carlo/Desktop/3D-Display/Calibrations/2D_points.txt" , "/home/carlo/Desktop/3D-Display/Calibrations/3D_points.txt");
}

/**
 * @brief MainWindow::onDoubleSpinBoxSurfaceFirstOrderCoefficientChanged
 * @param val
 */
void MainWindow::onDoubleSpinBoxSurfaceFirstOrderCoefficientChanged(double val)
{
    //this->ui->volumetricGLWidget->volume
}

/**
 * @brief MainWindow::onDoubleSpinBoxSurfaceZerothOrderCoefficientChanged
 * @param val
 */
void MainWindow::onDoubleSpinBoxSurfaceZerothOrderCoefficientChanged(double val)
{

}

/**
 * @brief MainWindow::onDoubleSpinBoxSurfaceSecondOrderCoefficientChanged
 * @param val
 */
void MainWindow::onDoubleSpinBoxSurfaceSecondOrderCoefficientChanged(double val)
{

}
