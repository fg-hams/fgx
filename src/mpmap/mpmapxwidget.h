#ifndef MPMAPXWIDGET_H
#define MPMAPXWIDGET_H

#include <QtCore/QVariant>
#include <QtCore/QString>

#include <QtWebKit/QWebView>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkCookieJar>


#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QActionGroup>
#include <QtGui/QToolButton>

#include "mpmap/gmapwidget.h"
#include "mpmap/pilotswidget.h"

#include "xobjects/mainobject.h"
#include "xobjects/latlng.h"

class MainObject;
class PilotsWidget;
class GMapWidget;


class MpMapXWidget : public QWidget
{
Q_OBJECT
public:
	explicit MpMapXWidget(MainObject *mob, QWidget *parent = 0);

	MainObject *mainObject;

	GMapWidget *gMapWidget;
	PilotsWidget *pilotsWidget;


	void closeEvent(QCloseEvent *event);
signals:

public slots:

	void initialize();


};

#endif // MPMAPXWIDGET_H