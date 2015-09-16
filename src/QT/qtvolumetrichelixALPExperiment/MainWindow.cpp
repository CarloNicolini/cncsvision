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

#ifdef COMPILE_GLFW
#include <GLFW/glfw3.h>
#endif
#ifndef BSTR2STR
#define BSTR2STR(X) std::string(const_cast<char *>(_com_util::ConvertBSTRToString( X )))
#endif
#ifndef STR2BSTR
#define STR2BSTR(X) _com_util::ConvertStringToBSTR(const_cast<const char*>(X.c_str()))
#endif

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent ):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initializeTabSceneQConnections();
    this->initializeTabCameraCalibrationQConnections();
    this->initializeTabProjectorQConnections();
    this->initializeTabExperimentQConnections();
    this->initializeTabMotorQConnections();

    this->resize(minimumSizeHint());
    cerr << "[MAINWINDOW] QtConnections done" << endl;
    cerr << "[MAINWINDOW] User interface successfully setup" << endl;

    // Initialization of spinboxes eccetera
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue(100);
    this->ui->spinBoxProjectorNSlices->setValue(PROJECTOR_SLICES_NUMBER);

    // Initialization of OpenGL Widget scene
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),ui->doubleSpinboxCameraZNear->value(),ui->doubleSpinboxCameraZFar->value());
    this->update();

    this->loadSettings();
#ifdef COMPILE_GLFW
    if (!glfwInit())
    {
        throw std::runtime_error("[GLFW] Can't initialize a valid GLFW context here");
        exit(EXIT_FAILURE);
    }
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);
    monitorResolutionX = mode->width;
    monitorResolutionY = mode->height;
    glfwGetMonitorPhysicalSize(monitor, &monitorWidthMM, &monitorHeightMM);
#endif
}

/**
 * @brief MainWindow::saveSettings
 */
void MainWindow::saveSettings()
{
    QSettings settings("CNCSVision","QtVolumetrixALPExperiment");

    // Scene tab
    settings.beginGroup("Scene");
    settings.setValue("Offset_X",this->ui->doubleSpinBoxOffsetX->value());
    settings.setValue("Offset_Y",this->ui->doubleSpinBoxOffsetY->value());
    settings.setValue("Offset_Z",this->ui->doubleSpinBoxOffsetZ->value());
    settings.setValue("Object_Size",this->ui->doubleSpinBoxObjectSize->value());
    // Suface
    settings.setValue("Thickness",this->ui->doubleSpinBoxSurfaceThickness->value());
    // Helicoid position
    settings.setValue("Tx_MM",this->ui->doubleSpinboxHelicoidCxMm->value());
    settings.setValue("Ty_MM",this->ui->doubleSpinboxHelicoidCyMm->value());
    settings.setValue("Tz_MM",this->ui->doubleSpinboxHelicoidCzMm->value());
    settings.endGroup();

    // Stimulus settings
    settings.beginGroup("Stimulus");
    settings.setValue("N_spheres",this->ui->spinBoxStimulusNSpheres->value());
    settings.setValue("Min_Radius",this->ui->spinBoxStimulusSpheresRadiusMin->value());
    settings.setValue("Max_Radius",this->ui->spinBoxStimulusSpheresRadiusMax->value());
    settings.endGroup();

    // Camera settings
    settings.beginGroup("Camera");
    settings.setValue("Eye_Z",this->ui->doubleSpinBoxCameraEyeZ->value());
    settings.setValue("FOV",this->ui->doubleSpinBoxCameraFOV->value());
    settings.setValue("Z_near",this->ui->doubleSpinboxCameraZNear->value());
    settings.setValue("Z_far",this->ui->doubleSpinboxCameraZFar->value());
    settings.endGroup();
    // Projector
    settings.beginGroup("Projector");
    settings.setValue("N_slices",this->ui->spinBoxProjectorNSlices->value());
    settings.setValue("Bit_Depth",this->ui->spinBoxProjectorBitDepth->value());
    settings.setValue("us_per_frame",this->ui->spinBoxProjectorMicrosecondsPerFrame->value());
    settings.endGroup();

    // Experiment settigns
    settings.beginGroup("Monitor");
    settings.setValue("Resolution_X",this->ui->monitorResolutionXSpinBox->value());
    settings.setValue("Resolution_Y",this->ui->monitorResolutionYSpinBox->value());
    settings.setValue("Physical_Size_Horizontal_MM",this->ui->monitorWidthMMDoubleSpinBox->value());
    settings.setValue("Physical_Size_Vertical_MM",this->ui->monitorHeightMMDoubleSpinBox->value());
    settings.endGroup();

}

/**
 * @brief MainWindow::loadSettings
 */
void MainWindow::loadSettings()
{

    QSettings settings("CNCSVision","QtVolumetrixALPExperiment");
    QFile settingsFile(settings.fileName());
    if (!settingsFile.exists())
        return;
    /*
    foreach (const QString &childKey, settings.allKeys())
        qDebug() << childKey << " " << settings.value(childKey);
    */
    // Scene
    this->ui->doubleSpinBoxOffsetX->setValue(settings.value("Scene/Offset_X").toDouble());
    this->ui->doubleSpinBoxOffsetY->setValue(settings.value("Scene/Offset_Y").toDouble());
    this->ui->doubleSpinBoxOffsetZ->setValue(settings.value("Scene/Offset_Z").toDouble());
    this->ui->doubleSpinBoxSurfaceThickness->setValue(settings.value("Scene/Thickness").toDouble());
    // Helicoid position
    this->ui->doubleSpinboxHelicoidCxMm->setValue(settings.value("Scene/Tx_MM").toDouble());
    this->ui->doubleSpinboxHelicoidCyMm->setValue(settings.value("Scene/Ty_MM").toDouble());
    this->ui->doubleSpinboxHelicoidCzMm->setValue(settings.value("Scene/Tz_MM").toDouble());

    // Stimulus
    this->ui->spinBoxStimulusNSpheres->setValue(settings.value("Stimulus/N_spheres").toInt());
    this->ui->spinBoxStimulusSpheresRadiusMin->setValue(settings.value("Stimulus/Min_Radius").toInt());
    this->ui->spinBoxStimulusSpheresRadiusMax->setValue(settings.value("Stimulus/Max_Radius").toInt());

    // Camera
    this->ui->doubleSpinBoxCameraEyeZ->setValue(settings.value("Camera/Eye_Z").toDouble());
    this->ui->doubleSpinBoxCameraFOV->setValue(settings.value("Camera/FOV").toDouble());
    this->ui->doubleSpinboxCameraZNear->setValue(settings.value("Camera/Z_near").toDouble());
    this->ui->doubleSpinboxCameraZFar->setValue(settings.value("Camera/Z_far").toDouble());

    // Projector
    this->ui->spinBoxProjectorNSlices->setValue(settings.value("Projector/N_Slices").toInt());
    this->ui->spinBoxProjectorBitDepth->setValue(settings.value("Projector/Bit_Depth").toInt());
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue(settings.value("Projector/us_per_frame").toInt());

    // Monitor
    this->ui->monitorResolutionXSpinBox->setValue(settings.value("Monitor/Resolution_X").toInt());
    this->ui->monitorResolutionYSpinBox->setValue(settings.value("Monitor/Resolution_Y").toInt());
    this->ui->monitorWidthMMDoubleSpinBox->setValue(settings.value("Monitor/Physical_Size_Horizontal_MM").toDouble());
    this->ui->monitorHeightMMDoubleSpinBox->setValue(settings.value("Monitor/Physical_Size_Vertical_MM").toInt());
#ifdef COMPILE_GLFW
    this->ui->monitorResolutionXSpinBox->setValue(this->monitorResolutionX);
    this->ui->monitorResolutionYSpinBox->setValue(this->monitorResolutionY);
    this->ui->monitorWidthMMDoubleSpinBox->setValue(this->monitorWidthMM);
    this->ui->monitorHeightMMDoubleSpinBox->setValue(this->monitorHeightMM);
#endif
}

/**
 * @brief MainWindow::initializeTabCameraCalibrationQConnections
 */
void MainWindow::initializeTabCameraCalibrationQConnections()
{
    QObject::connect(this->ui->doubleSpinBoxCameraFOV,SIGNAL(valueChanged(double)),this,SLOT(onCameraFOVChanged(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZNear,SIGNAL(valueChanged(double)),this,SLOT(onCameraZNearChanged(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZFar,SIGNAL(valueChanged(double)),this,SLOT(onCameraZFarChanged(double)));
    QObject::connect(this->ui->pushButtonCalibrate,SIGNAL(clicked()),this,SLOT(onPushButtonCalibrateClicked()));
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
    QObject::connect(this->ui->pushButtonProjectorChangeSequence,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorSequenceChanged()));
    QObject::connect(this->ui->pushButtonRemoveSequence,SIGNAL(clicked()),this,SLOT(onPushButtonProjectorRemoveSequencePressed()));

    // Connect microsecPerFrame spinbox to slot that changes the microseconds per round given the number of slices
    QObject::connect(this->ui->spinBoxProjectorMicrosecondsPerFrame,SIGNAL(valueChanged(int)),this,SLOT(onSpinBoxProjectorMicrosecondsPerFrameChanged(int)));
    // Connect microsecPerFrame spinbox to slot that changes the microseconds per round given the number of slices
    QObject::connect(this->ui->spinBoxProjectorMicrosecondsPerRound,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxProjectorMicrosecondsPerRoundChanged(int)));

    // Connections for LED current contorol
    QObject::connect(this->ui->spinBoxProjectorLEDcurrent,SIGNAL(valueChanged(int)),this,SLOT(onspinBoxProjectorLEDcurrentChanged(int)));
    QObject::connect(this->ui->doubleSpinBoxProjectorLEDpercentage,SIGNAL(valueChanged(double)),this,SLOT(onSpinBoxProjectorLEDpercentageChanged(double)));

    // When microseconds per frame are changed, then microseconds per round is changed
    QObject::connect(this->ui->spinBoxProjectorMicrosecondsPerRound,SIGNAL(valueChanged(int)),this,SLOT(onSpinboxProjectorMicrosecondsPerRoundChanged(int)));
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

    // Camera eyeZ changed
    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(eyeZChanged(double)),this->ui->doubleSpinBoxCameraEyeZ,SLOT(setValue(double)));
    QObject::connect(this->ui->doubleSpinBoxCameraEyeZ,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(onEyeZChanged(double)));

    // Camera zNear changed
    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(zNearChanged(double)),this->ui->doubleSpinboxCameraZNear,SLOT(setValue(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZNear,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(onZNearChanged(double)));

    // Camera zFar changed
    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(zFarChanged(double)),this->ui->doubleSpinboxCameraZFar,SLOT(setValue(double)));
    QObject::connect(this->ui->doubleSpinboxCameraZFar,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(onZFarChanged(double)));

    // Stimulus tab connections
    QObject::connect(this->ui->pushButtonStimulusRandomizeSpheres,SIGNAL(clicked()),this,SLOT(onPushButtonRandomizeSpheresPressed()));
    QObject::connect(this->ui->pushButtonGenerateFrames,SIGNAL(clicked()),this,SLOT(onPushButtonGenerateFramesPressed()));

    // Enable/disable offscreen rendering
    QObject::connect(this->ui->checkBoxUseOffscreenRendering,SIGNAL(clicked(bool)),this->ui->volumetricGLWidget,SLOT(setOffscreenRendering(bool)));
    QObject::connect(this->ui->pushButtonLoadBinVox,SIGNAL(clicked(bool)),this,SLOT(onPushButtonLoadBinVoxPressed()));

    QObject::connect(this->ui->volumetricGLWidget,SIGNAL(binVoxLoaded(QString)),this->ui->labelTextureInfo,SLOT(setText(QString)));
    // Connection for filtered parametric surface thickness
    QObject::connect(this->ui->doubleSpinBoxSurfaceThickness,SIGNAL(valueChanged(double)),this->ui->volumetricGLWidget,SLOT(onSurfaceThicknessChanged(double)));
}

/**
 * @brief MainWindow::initializeTabMotorQConnections
 */
void MainWindow::initializeTabMotorQConnections()
{
    QObject::connect(this->ui->pushButtonMotorStart,SIGNAL(clicked(bool)),this,SLOT(onPushButtonMotorStartClicked()));
    QObject::connect(this->ui->pushButtonMotorStop,SIGNAL(clicked(bool)),this,SLOT(onPushButtonMotorStopClicked()));
    QObject::connect(this->ui->pushButtonMotorInitialize,SIGNAL(clicked(bool)),this,SLOT(onPushButtonMotorInitializeClicked()));
    QObject::connect(this->ui->doubleSpinBoxFlickerRateHz,SIGNAL(valueChanged(double)),this,SLOT(onSpinBoxFlickerFrequencyChanged(double)));
}

/**
 * @brief MainWindow::initializeTabExperimentQConnections
 */
void MainWindow::initializeTabExperimentQConnections()
{
    // Experiment connections
    QObject::connect(this->ui->pushButtonStartExperiment,SIGNAL(clicked()),this,SLOT(onPushButtonExperimentStartClicked()));

    // Disable/enable debrujin window
    this->ui->groupBoxDeBrujinRandomization->setEnabled(false);
    QObject::connect(this->ui->buttonGroupRandomizationMethod,SIGNAL(buttonClicked(int)),this,SLOT(onRandomizationMethodChanged(int)));
}

/**
 * @brief MainWindow::onPushButtonMotorInitializeClicked
 */
void MainWindow::onPushButtonMotorInitializeClicked()
{
#if defined (SMI_SUPPORT) && (WIN32)
    CoInitialize(NULL);
    /*
    if(!AfxOleInit())
    {
        throw std::runtime_error("OLE initialization failed.  Make sure that the OLE libraries are the correct version.");
    }
    AfxEnableControlContainer();
    */
    HRESULT hr = CommInterface.CreateInstance(__uuidof(INTEGMOTORINTERFACELib::SMIHost));
    if(FAILED(hr))
    {
        QMessageBox::warning(this,"Error","Cannot create an instance of \"SMIHost\" class!");
        return;
    }

    cerr << "[MainWindow] Starting motor" << endl;
    long portResult=-1;
    try
    {
        CommInterface->BaudRate = 9600;
        portResult = CommInterface->OpenPort("Com4");
    }
    catch (_com_error e)
    {
        throw std::exception("Error opening COM4 port");
    }
    // Detect RS232
    cerr << "[Smart Motor] Detecting RS232...";
    UINT x= 10;//GetDlgItemInt(IDC_MAXADDRESS);
    long MaxAddress=10;
    long flags=0;
    long result = CommInterface->DetectRS232(MaxAddress,flags);
    switch(result)
    {
    case CER_SOMEADDRESSED:
        cerr << "Some Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
        break;

    case CER_BIGADDRESS:
        cerr << "Some Motors have big addresses!" <<" IntegMotorInterface Error!" << endl;
        break;

    case CER_DUPLICATEADDR:
        cerr << "Some Motors have duplicate addresses!"<<" IntegMotorInterface Error!"<< endl;
        break;

    case CER_NOTADDRESSED:
        cerr << "Motors are not addressed!"<<" IntegMotorInterface Error!"<< endl;
        break;

    case CER_COMMERROR:
        cerr << "Communication Error!"<<" IntegMotorInterface Error!"<< endl;
        break;

    case CER_NOMOTOR:
        cerr << "No Motor Found!"<<" IntegMotorInterface Error!" << endl;
        break;
    default:
        cerr << CommInterface->NoOfMotors << endl;
        //cerr << CommInterface->DefaultMotor = long(1);//GetDlgItemInt(IDC_CURRENTMOTOR);
    }

    // Establish chain
    cerr << "[Smart Motor] Addressing motor chain...";
    try
    {
        // Establish chain
        CommInterface->AddressMotorChain();
        CommInterface->DefaultMotor = 1;
    }
    catch (_com_error e )
    {
        //AfxMessageBox(e.Description());
        throw std::runtime_error(BSTR2STR(e.Description()));
    }
    cerr << "DONE" << endl;
#endif
}

/**
 * @brief MainWindow::onPushButtonMotorStartClicked
 */
void MainWindow::onPushButtonMotorStartClicked()
{
#if defined (SMI_SUPPORT) && (WIN32)
    cerr << "[MainWindow] Starting motor" << endl;
    long Version = 0;
    Version = CommInterface->EngineVersion;
    if ( this->ui->spinBoxMotorSpeed->isEnabled() )
    {
        int speed = this->ui->spinBoxMotorSpeed->value();
        this->startRotation(speed);
    }
    else
    {
        QMessageBox::warning(this,"!!! WARNING !!!","This speed is not safe for the system");
    }
#endif
}

/**
 * @brief MainWindow::onPushButtonMotorStopClicked
 */
void MainWindow::onPushButtonMotorStopClicked()
{
#if defined (SMI_SUPPORT) && (WIN32)
    cerr << "[MainWindow] Stopping motor" << endl;
    this->startRotation(0);
#endif
}

/**
* @brief MainWindow::onSpinBoxFlickerFrequencyChanged
**/
void MainWindow::onSpinBoxFlickerFrequencyChanged(double flickerFrequency)
{
    QComboBox *c = this->ui->comboBoxMotorUnitsToRevMin;
    // The persistence of the stimulus on the eye in microseconds
    double flickerPersistence_microSec = 1E6/flickerFrequency;
    double nSlices = this->ui->spinBoxProjectorNSlices->value();
    double motorUnitsToRevMin = 65536;//c->itemData(c->currentIndex()).toInt();
    cerr << c->itemData(c->currentIndex()).toDouble() << endl;
    double microSecPerFrame = flickerPersistence_microSec/nSlices;
    if ( std::fmod(microSecPerFrame,1.0) == 0.0)
    {
        //cerr << "consistent frame per microsecond" << endl;
        this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue((int)microSecPerFrame);
        //this->onPushButtonProjectorRemoveSequencePressed();
        //this->onPushButtonUploadSequenceClicked();
    }
    else
    {
        //cerr << "inconsistent frame per microsecond" << endl;
    }

    double motorRate = (1E6/(nSlices*std::floor(microSecPerFrame))) * motorUnitsToRevMin;
    if ( motorRate > this->ui->spinBoxMotorSpeed->maximum() )
    {
        this->ui->spinBoxMotorSpeed->setValue((int)motorRate);
        this->ui->spinBoxMotorSpeed->setEnabled(false);
    }
    else
    {
        //if ( std::fmod(motorRate,1.0) == 0)
        if (true)
        {
            this->ui->spinBoxMotorSpeed->setEnabled(true);
            this->ui->spinBoxMotorSpeed->setValue((int)motorRate);
            //this->onPushButtonMotorStartClicked();
        }
        else
        {
            this->ui->spinBoxMotorSpeed->setEnabled(false);
        }
        this->ui->spinBoxMotorRevMin->setValue(motorRate/8000);
    }
}

void MainWindow::startRotation(int speed)
{
#if defined (SMI_SUPPORT) && (WIN32)
    try
    {
        string speedstring("VT=");
        speedstring+=util::stringify<int>(speed);
        CommInterface->ClearBuffer();
        CommInterface->WriteCommand("EIGN(W,0)");
        CommInterface->WriteCommand("O=0");
        CommInterface->WriteCommand("ZS");
        CommInterface->WriteCommand("EL=-1");
        // Set the PID settings
        CommInterface->WriteCommand("KP=560");
        CommInterface->WriteCommand("KI=5");
        CommInterface->WriteCommand("KD=4500");

        CommInterface->WriteCommand("KL=32767");
        CommInterface->WriteCommand("KS=3");
        CommInterface->WriteCommand("KV=1000");
        CommInterface->WriteCommand("KA=0");
        CommInterface->WriteCommand("KG=0");

        // Activate the buffered PID settings
        CommInterface->WriteCommand("F");
        CommInterface->WriteCommand("MV");
        CommInterface->WriteCommand("ADT=1");
        CommInterface->WriteCommand(STR2BSTR(speedstring));
        CommInterface->WriteCommand("G");
    }
    catch (_com_error e )
    {
        throw std::runtime_error(BSTR2STR(e.Description()));
    }
#endif
}
/**
 * @brief MainWindow::onRandomizationMethodChanged
 * @param randomVal
 */
void MainWindow::onRandomizationMethodChanged(int randomVal)
{
    if (randomVal == -6)
    {
        this->ui->groupBoxDeBrujinRandomization->setEnabled(true);
    }
    else
    {
        this->ui->groupBoxDeBrujinRandomization->setEnabled(false);
    }
}

/**
 * @brief MainWindow::onPushButtonLoadBinVoxPressed
 */
void MainWindow::onPushButtonLoadBinVoxPressed()
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
 * @brief MainWindow::onCameraFOVChanged
 * @param val
 */
void MainWindow::onCameraFOVChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(val,ui->doubleSpinboxCameraZNear->value(),ui->doubleSpinboxCameraZFar->value());
}

/**
 * @brief MainWindow::onCameraZNearChanged
 * @param val
 */
void MainWindow::onCameraZNearChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),val,ui->doubleSpinboxCameraZFar->value());
}

/**
 * @brief MainWindow::onCameraZFarChanged
 * @param val
 */
void MainWindow::onCameraZFarChanged(double val)
{
    this->ui->volumetricGLWidget->setCameraParameters(ui->doubleSpinBoxCameraFOV->value(),ui->doubleSpinboxCameraZNear->value(),val);
}

/**
* @brief onSpinBoxProjectorMicrosecondsPerFrameChanged
**/

void MainWindow::onSpinBoxProjectorMicrosecondsPerFrameChanged(int value)
{
    this->ui->spinBoxProjectorMicrosecondsPerRound->setValue( std::ceil( (double)value*this->ui->spinBoxProjectorNSlices->value()) );
    double flickerRate = 1E6/((double)(value*this->ui->spinBoxProjectorNSlices->value()));
    this->ui->doubleSpinBoxFlickerRateHz->setValue(flickerRate);
}

/**
* @brief onSpinboxMicrosecondsPerRoundChanged
**/
void MainWindow::onSpinboxProjectorMicrosecondsPerRoundChanged(int value)
{
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue( std::ceil( (double)value/this->ui->spinBoxProjectorNSlices->value()) );
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    cerr << "[MAINWINDOW] Destructor" << endl;
    saveSettings();
    delete this->ui;
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

    this->ui->volumetricGLWidget->updateGL();
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
    double flickerPersistenceMicroSec = this->ui->doubleSpinBoxFlickerRateHz->value()*1E-6;
    double motorUnitsToRevMin = this->ui->comboBoxMotorUnitsToRevMin->itemData(
                this->ui->comboBoxMotorUnitsToRevMin->currentIndex()).toInt();
    int usPerFrame = flickerPersistenceMicroSec/value;
    this->ui->spinBoxProjectorMicrosecondsPerFrame->setValue(usPerFrame);
    double motorRate = (1E6/((double)value*usPerFrame))*motorUnitsToRevMin;
    this->ui->spinBoxMotorSpeed->setValue(motorRate);
    this->ui->spinBoxProjectorMicrosecondsPerRound->setValue(value*ui->spinBoxProjectorMicrosecondsPerFrame->value());
}

/**
 * @brief MainWindow::onPushButtonRandomizeSpheresPressed
 */
void MainWindow::onPushButtonRandomizeSpheresPressed()
{
    bool useRandomDots = this->ui->randomDotsCheckBox->isChecked();
    this->ui->volumetricGLWidget->randomizeSpheres(useRandomDots, ui->spinBoxStimulusNSpheres->value(),ui->spinBoxStimulusSpheresRadiusMin->value(),ui->spinBoxStimulusSpheresRadiusMax->value());
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
    this->ui->statusbar->showMessage("Uploading sequence...",500);
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
    this->ui->statusbar->showMessage("Sequence uploaded",5000);
}

/**
* @brief onPushButtonProjectorSequenceChanged
**/
void MainWindow::onPushButtonProjectorSequenceChanged()
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
* @brief onPushButtonProjectorRemoveSequencePressed
**/
void MainWindow::onPushButtonProjectorRemoveSequencePressed()
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
        if ( this->ui->volumetricGLWidget->width() != PROJECTOR_RESOLUTION_WIDTH &&
             this->ui->volumetricGLWidget->height() != PROJECTOR_RESOLUTION_HEIGHT )
            throw std::runtime_error(std::string("OpenGL widget must have the same resolution of the projector, 1024x768"));
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
        alp.m_AlpSeqId.clear();
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error in ALP projector",QString(e.what()));
    }
#endif
    this->ui->listWidgetSequences->clear();
    this->ui->spinBoxProjectorLEDcurrent->setEnabled(false);
    this->ui->doubleSpinBoxProjectorLEDpercentage->setEnabled(false);
}

/**
* @brief onSpinBoxLEDpercentageChanged
**/
void MainWindow::onSpinBoxProjectorLEDpercentageChanged(double percentage)
{
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        if (!alp.m_bAlpLEDInit)
        {
            alp.initLED();
            alp.setLED(ui->spinBoxProjectorLEDcurrent->value(),static_cast<long int>(std::ceil(percentage)));
        }
        else
            alp.setLED(ui->spinBoxProjectorLEDcurrent->value(),static_cast<long int>(std::ceil(percentage)));
    }
#endif
}

/**
* @brief onspinBoxProjectorLEDcurrentChanged
**/
void MainWindow::onspinBoxProjectorLEDcurrentChanged(int current)
{
#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
    {
        if (!alp.m_bAlpLEDInit)
        {
            alp.initLED();
            alp.setLED(current,ui->doubleSpinBoxProjectorLEDpercentage->value());
        }
        else
            alp.setLED(current,ui->doubleSpinBoxProjectorLEDpercentage->value());
    }
#endif
}

/**
 * @brief MainWindow::onPushButtonProjectorInitializeClicked
 */
void MainWindow::onPushButtonProjectorInitializeClicked()
{
    this->ui->spinBoxProjectorLEDcurrent->setEnabled(true);
    this->ui->doubleSpinBoxProjectorLEDpercentage->setEnabled(true);

#ifdef ALP_SUPPORT
    if (alp.m_bAlpInit)
        return;
    int nSlices = this->ui->spinBoxProjectorNSlices->value();
    unsigned char *data = this->ui->volumetricGLWidget->allFrames.data();
    try
    {
        alp.init();
        alp.initLED();
        alp.setLED(this->ui->spinBoxProjectorLEDcurrent->value(),ui->doubleSpinBoxProjectorLEDpercentage->value());
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
 * @brief MainWindow::onPushButtonCalibrateClicked
 */
void MainWindow::onPushButtonCalibrateClicked()
{
    QString points2Dfilename = QFileDialog::getOpenFileName(this,"Select 2D points file",QDir::currentPath(),"*.txt");
    QString points3Dfilename = QFileDialog::getOpenFileName(this,"Select 3D points file",QDir::currentPath(),"*.txt");

    this->ui->volumetricGLWidget->computeCameraCalibrationMatrices(points2Dfilename,points3Dfilename);
    //this->ui->volumetricGLWidget->computeCameraCalibrationMatrices("/home/carlo/Desktop/3D-Display/Calibrations/2D_points.txt" , "/home/carlo/Desktop/3D-Display/Calibrations/3D_points.txt");
}

/**
 * @brief MainWindow::onDoubleSpinBoxSurfaceFirstOrderCoefficientChanged
 * @param val
 */
void MainWindow::onDoubleSpinBoxSurfaceFirstOrderCoefficientChanged(double val)
{
    //this->ui->volumetricGLWidget->curvature = val;
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

/**
 * @brief MainWindow::triggerFramesGeneration
 * @param curvature
 */
void MainWindow::triggerFramesGeneration(double curvature)
{
    this->onPushButtonProjectorStopProjectionClicked();
    this->onDoubleSpinBoxSurfaceFirstOrderCoefficientChanged(curvature);
    this->onPushButtonGenerateFramesPressed();
    this->onPushButtonUploadSequenceClicked();
    this->onPushButtonProjectorSequenceChanged();
    this->onPushButtonProjectorStartProjectionClicked();
}

/**
 * @brief MainWindow::onPushButtonExperimentStartClicked
 * @param clicked
 */
void MainWindow::onPushButtonExperimentStartClicked()
{
    this->ui->plainTextExperimentInfo->clear();
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString timeString = time.toString();

    QString subjectName = QInputDialog::getText(this, "Enter Subject name","Subject name:", QLineEdit::Normal);
    if ( subjectName.isEmpty() )
    {
        QMessageBox::warning(this,"Warning","Please set a valid (non-empty) subject name");
        return;
    }

    // Then must initialize the ALP
    try
    {
        this->ui->checkBoxUseOffscreenRendering->setChecked(true);
        this->ui->volumetricGLWidget->setOffscreenRendering(true);
        this->onPushButtonProjectorInitializeClicked();
        this->ui->spinBoxProjectorLEDcurrent->setValue(this->ui->spinBoxProjectorLEDcurrent->maximum());
        this->ui->doubleSpinBoxProjectorLEDpercentage->setValue(100);
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Warning",QString(e.what()));
        this->onPushButtonProjectorReleaseClicked();
        return;
    }

    exp = new ExperimentGLWidget(NULL);
    exp->setCameraParameters(this->ui->volumetricGLWidget->eyeZ,this->ui->volumetricGLWidget->FOV,this->ui->volumetricGLWidget->zNear,this->ui->volumetricGLWidget->zFar);

    exp->initMonitor(this->ui->monitorResolutionXSpinBox->value(),this->ui->monitorResolutionYSpinBox->value(),
                     this->ui->monitorWidthMMDoubleSpinBox->value(),this->ui->monitorHeightMMDoubleSpinBox->value());

    //QString windowTitle = QString("Volumetric Experiment - ") + date.toString("dd.MM.yyyy") + " " + timeString + " - " + subjectName; exp->setWindowTitle(windowTitle);
    // Fetch experiment parameters
    int nRepetitions = this->ui->repetitionsSpinBox->value();
    int randomize = this->ui->buttonGroupRandomizationMethod->checkedId();
    if (randomize == -1)
    {
        QMessageBox::warning(this,"Warning",QString("Please select a randomization method"));
        return;
    }
    exp->bal.init(nRepetitions,-randomize-2);
    // Get the list of factors
    vector<double> factorsDepth;
    try
    {
        factorsDepth = util::str2num<double>(this->ui->factorCurvatureLineEdit->text().toStdString()," ");
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,"Error",e.what());
        return;
    }
    exp->bal.addFactor("Curvature",factorsDepth);

    //Establish connections
    connect(exp,SIGNAL(trialAdvanced(double)),this->ui->volumetricGLWidget,SLOT(onSurfaceCurvatureChanged(double)));
    connect(exp,SIGNAL(trialAdvanced(double)),this,SLOT(triggerFramesGeneration(double)));
    connect(exp,SIGNAL(getExperimentInfo(const QString &)),this->ui->plainTextExperimentInfo,SLOT(insertPlainText(const QString &)));
    connect(exp,SIGNAL(experimentFinished()),this,SLOT(onPushButtonProjectorReleaseClicked()));
    // Run the experiment
    exp->initializeExperiment();
    exp->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Ensure clean close of Experiment Widget
    if (exp)
        exp->close();
    this->onPushButtonProjectorReleaseClicked();
}
