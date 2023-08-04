#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CGMapView/cgmapview.h"

#define MAX_VIEWSCALE_VALUE 50000000

#define MIN_VIEWSCALE_VALUE 1000
namespace Ui {
class MainWindow;
}

using namespace INDIGIS_CORE;



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void openMap(QString mapName);
private slots:
    void on_actionOpenMap_triggered();

    void mapZoomIn();

    void mapZoomOut();



    void rotateLeft();

    void rotateRight();

    void rotateNorth();
private:
    CGConnection *createConnection(QString fileName);

    CGPointer<CGDataPipe> openPipe(CGConnection *activeConn);


private:
     Ui::MainWindow *ui;
     CGMapView* mapView;
};

#endif // MAINWINDOW_H
