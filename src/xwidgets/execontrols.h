// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// FGx FlightGear Launcher // execontrols.h
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// (c) 2010-2012
// Yves Sablonier, Pete Morgan
// Geoff McLane
// GNU GPLv2, see main.cpp and shipped licence.txt for further information

#ifndef EXECONTROLS_H
#define EXECONTROLS_H

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QStatusBar>
#include <QProcess>

class ExeControls : public QGroupBox
{
Q_OBJECT
public:
	explicit ExeControls(QString title, QWidget *parent = 0);
	
	QToolButton *buttonStart;
	QToolButton *buttonStop;

signals:
	void start();
	void stop();


public slots:
	void on_start_clicked();
	void on_stop_clicked();

	void set_running(bool running);
};

#endif // EXECONTROLS_H
