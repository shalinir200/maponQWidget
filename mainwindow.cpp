#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

#include "CGConnection/cgvectornativeconnection.h"
#include "CGConnection/cgrasternativeconnection.h"
#include "CGPipe/cgvectorpipe.h"
#include "CGPipe/cgstaticrasterpipe.h"
#include "CGPipe/cgdynamicrasterpipe.h"
#include "CGMapView/cgdatarepository.h"
#include "CGGeometry/cggeometrymath.h"

#include "CGFileConnection/cgrasterfileconnection.h"
#include "CGFileConnection/cgvectorfileconnection.h"
#ifdef INCLUDE_S57
#include "CGS52Renderer/cgs52mapview.h"
#else
#include "CGGeoEvents/cgrtmapview.h"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef INCLUDE_S57
    mapView = new CGS52MapView(this);
#else
    mapView = new CGMapView(this);
#endif
    openMap("/home/gis/Downloads/GIS/INDIGIS ToT/ExampleApps/Data/world.ivf");

    this->setCentralWidget(mapView);

    mapView->setBackgroundColor(Qt::white);


    ui->mainToolBar->hide();
    ui->menuBar->hide();

    QToolBar* mapToolBar = new QToolBar("MapDisplay", this);
    mapToolBar->setObjectName("MapDisplay");
    this->addToolBar(Qt::TopToolBarArea,  mapToolBar );

    QAction *actOpenMapFile= new QAction(QIcon("../icons/MapDisplay/OpenMaps.png"),
                                         tr("Open Map"), this);
    actOpenMapFile->setStatusTip(tr("Open Map Files"));
    connect(actOpenMapFile, SIGNAL(triggered()), this, SLOT(on_actionOpenMap_triggered()));
    mapToolBar->addAction(actOpenMapFile);

    mapToolBar->addSeparator();

    QAction * actZoomIn = new QAction(QIcon("../icons/MapInteraction/ZoomIn.png"),
                                tr("Zoom In"), this);
    actZoomIn->setShortcut(tr("Ctrl+Plus"));
    actZoomIn->setStatusTip(tr("Zoom In Mapview"));
    connect(actZoomIn, SIGNAL(triggered()), this, SLOT(mapZoomIn()));
    mapToolBar->addAction(actZoomIn);

    QAction * actZoomOut =
          new QAction(QIcon("../icons/MapInteraction/ZoomOut.png"),
                              tr("Zoom Out"), this);
    actZoomOut->setShortcut(tr("Ctrl+Minus"));
    actZoomOut->setStatusTip(tr("Zoom Out Mapview"));
    connect(actZoomOut, SIGNAL(triggered()), this, SLOT(mapZoomOut()));
    mapToolBar->addAction(actZoomOut);


    mapToolBar->addSeparator();

    QAction * actRotateLeft = new QAction(QIcon("../icons/MapInteraction/RotateLeft.png"),
                                tr("Rotate Left"), this);
    actRotateLeft->setStatusTip(tr("Fit All In Mapview"));
    connect(actRotateLeft, SIGNAL(triggered()), this, SLOT(rotateLeft()));
    mapToolBar->addAction(actRotateLeft);

    QAction * actRotateNorth = new QAction(QIcon("../icons/MapInteraction/RotateNorth.png"),
                                tr("Rotate North"), this);
    actRotateNorth->setStatusTip(tr("Rotate North"));
    connect(actRotateNorth, SIGNAL(triggered()), this, SLOT(rotateNorth()));
    mapToolBar->addAction(actRotateNorth);

    QAction * actRotateRight = new QAction(QIcon("../icons/MapInteraction/RotateRight.png"),
                                tr("Rotate Right"), this);
    actRotateRight->setStatusTip(tr("Rotate Right"));
    connect(actRotateRight, SIGNAL(triggered()), this, SLOT(rotateRight()));
    mapToolBar->addAction(actRotateRight);

    QAction * actfreehandmarking=new QAction(QIcon("../icons/MapDisplay/freehandmarking.png"),tr("freehand marking"),this);
    mapToolBar->addAction(actfreehandmarking);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void  MainWindow::rotateLeft()
{
    mapView->rotate(mapView->rotationAngle()-1);
}

void  MainWindow::rotateRight()
{
    mapView->rotate(mapView->rotationAngle()+1);
}

void  MainWindow::rotateNorth()
{
    mapView->rotate(0);
}

void  MainWindow::mapZoomIn()
{
    double viewS = mapView->viewScale();

    if( viewS <= MIN_VIEWSCALE_VALUE )
    {
        return;
    }

    mapView->zoomIn();
}

void  MainWindow::mapZoomOut()
{
    double viewS = mapView->viewScale();

    if( viewS >= MAX_VIEWSCALE_VALUE )
    {
        return;
    }

    mapView->zoomOut();

}



void MainWindow::on_actionOpenMap_triggered()
{
    QStringList fileNames;

    fileNames = QFileDialog::getOpenFileNames(this,
                                              tr("Open Map File"),
                                              "",
                                              tr("Indigis File Formats(*.ivf *.iof *.irf *.dgn *.shp *.dem *.dt1 *.dt2 *.dt0 *.tiff *.tif *.TIF *.bmp *.jpg *.jpeg );;"
                                                 "Indigis Vector Format(*.ivf);;"
                                                 "Indigis Overlay Format(*.iof);;"
                                                 "Indigis Raster Format(*.irf);;"
                                                 "Microstation design file(*.dgn);;"
                                                 "ESRI Shape file(*.shp);;"
                                                 "Elevation data(*.dem *.dt1 *.dt2 *.dt0);;"
                                                 "GeoTiff file(*.tiff *.tif);;"
                                                 "Bitmap files (*.bmp);;"
                                                 "JPEG files (*.jpg *.jpeg);;")
                                              );


    for ( int i =0 ;i < fileNames.count(); i++)
    {
        QFile file(fileNames.at(i));

        if(file.exists())
        {
            openMap(fileNames.at(i));
        }
    }
}


void MainWindow::openMap(QString mapName)
{
    QFile file(mapName);

    if(!file.exists()) return;

    this->statusBar()->showMessage("Opening maps...Please wait....");

    CGConnection *activeConn = NULL;

    CGPointer <CGDataPipe> pipe;

    activeConn = createConnection( mapName );

    if(activeConn )
        pipe = openPipe( activeConn );

    if (pipe)
        mapView->featureRepository()->addPipe(pipe);

    mapView->refresh();

    this->statusBar()->clearMessage();
}

CGConnection* MainWindow :: createConnection (QString fileName )
{

    QString path , mapName, fileType ;

    QFileInfo fileInfo = QFileInfo(fileName);

    if (!fileInfo.exists()) return NULL;

    CGConnection* conn = NULL;

    path = fileInfo.absoluteDir().absolutePath();

    mapName = fileInfo.fileName();

    fileType =  fileInfo.suffix().toLower();


    if ( fileType == "irf"  )
    {


        conn = new CGRasterNativeConnection(   path + "/" + mapName,
                                               mapName,
                                               "IRF Conn",
                                               CGConnection::Mode_ReadOnly );


    }
    else if ( fileType == "ief")
    {

        conn = new CGRasterNativeConnection(   path + "/" + mapName,
                                               mapName,
                                               "IEF Conn",
                                               CGConnection::Mode_ReadOnly );

    }

    else if ( fileType == "tiff" ||
              fileType == "tif" ||
              fileType == "dem" ||
              fileType == "bmp" ||
              fileType == "jpg" ||
              fileType == "jpeg" ||
              fileType.startsWith("dt") ||
              fileType == "grd" ||
              fileType == "hgt" )
    {

        conn = new CGRasterFileConnection (path + "/" + mapName, mapName );

    }
    else if ( fileType == "ivf")
    {
        conn = new CGVectorNativeConnection(path + "/" + mapName, mapName, "IVF Conn");

    }
    else if ( fileType == "iof")
    {
        conn = new CGVectorNativeConnection(path + "/" + mapName, mapName, "IOF Conn",
                                            CGConnection::Mode_ReadWrite);

    }
    else if ( fileType == "dgn" || fileType == "dvd" || fileType == "dat" ||
              fileType == "shp" || fileType == "dxf" || fileType == "dwg" ||
              fileType == "kml" ||fileType == "gml" || fileType.startsWith("00") || fileType == "mdb" )
    {
        conn = new CGVectorFileConnection(path + "/" + mapName, mapName, "Vector File Conn");
    }

    return conn;


}

CGPointer <CGDataPipe>  MainWindow:: openPipe ( CGConnection* activeConn )
{
    CGPointer <CGDataPipe> pipe = NULL;

    QStringList layers;

    //layers << "DEPARE_AREA";

    QString pipeName = activeConn->name();


    if ( activeConn->connType() == CGConnection::Type_Vector )
    {

        pipe =   new CGVectorPipe( pipeName,
                                   dynamic_cast<CGVectorConnection*>(activeConn),
                                   layers ,
                                   QStringList(),
                                   (activeConn->mode()==CGConnection::Mode_ReadWrite)) ;

    }
    else
    {

        CGRasterConnection *rConn = dynamic_cast<CGRasterConnection*>(activeConn);

        if ( rConn && rConn->getDBType() == CGRasterConnection::Type_RasterFile )

        {
            //pipe = new CGStaticRasterPipe(pipeName, rConn , activeConn->name());

            CGRasterFileConnection *fileConn = dynamic_cast<CGRasterFileConnection*>(rConn);

            if ( fileConn && fileConn->isCacheEnabled() )
            {
                pipe = new CGDynamicRasterPipe(pipeName, fileConn , activeConn->name());
            }
            else
                pipe = new CGStaticRasterPipe(pipeName, rConn , activeConn->name());
        }

        else// IRF, IEF
        {
            pipe = new CGDynamicRasterPipe(pipeName, rConn , activeConn->name());
        }

        //pipe->attachc( activeConn );
    }

    return pipe;
}

