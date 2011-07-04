

//#include <QtCore/QDebug>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QStringList>


#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QToolButton>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QSplitter>

#include <QtWebKit/QWebFrame>
#include <QtGui/QDesktopServices>

#include "mpmap/mpmapxwidget.h"


/*
QList<int, int> zoomLevelMap

zoomLevelMap[5000] = 1;
zoomLevelMap[1500] = 2;
zoomLevelMap[2000] = 3;
zoomLevelMap[1000] = 4;
zoomLevelMap[500] = 5;
zoomLevelMap[200] = 6;
zoomLevelMap[100] = 7;
zoomLevelMap[100] = 8;
zoomLevelMap[100] = 9;
zoomLevelMap[100] = 10;
zoomLevelMap[100] = 11;

zoomLevelMap[100] = 12;
zoomLevelMap[100] = 13;
zoomLevelMap[100] = 14;

*/
MpMapXWidget::MpMapXWidget(MainObject *mob, QWidget *parent) :
    QWidget(parent)
{

	mainObject  = mob,

    setWindowTitle(tr("FGx Map"));
	setWindowIcon(QIcon(":/icon/mpmap"));


	setProperty("settings_namespace", QVariant("mpmapxwidget__window"));
	mainObject->settings->restoreWindow(this);


	//======================================================
	//= Main Layout and Splitter
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

	QLabel *label_notice = new QLabel();
	label_notice->setText("This widget is currently work in progress. You need to create fgx/src/DEV_MODE.txt for it to work for now");
	label_notice->setStyleSheet("background-color: #eeeedd; color: #000099; padding: 5px;");
	mainLayout->addWidget(label_notice, 0);

	QSplitter *splitter = new QSplitter();
	mainLayout->addWidget(splitter, 200);


	//======================================================
	//= Map
	gMapWidget = new GMapWidget(mainObject);
	splitter->addWidget(gMapWidget);


	//================================================================================
	//= Pilots widget
	//================================================================================
	pilotsWidget = new PilotsWidget(mainObject);
	splitter->addWidget(pilotsWidget);


	//====================================================================

	splitter->setStretchFactor(0, 2);
	splitter->setStretchFactor(1, 1);



}

void MpMapXWidget::initialize(){
	gMapWidget->init_map();
}

//= window close
void MpMapXWidget::closeEvent(QCloseEvent *event){
	mainObject->settings->saveWindow(this);
	event->accept();
}