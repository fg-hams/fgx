// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// FGx FlightGear Launcher // viewlogwidget.cpp
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// (c) 2010-2012
// Yves Sablonier, Pete Morgan
// Geoff McLane
// GNU GPLv2, see main.cpp and shipped licence.txt for further information

#include <QHBoxLayout>

#include <QGroupBox>
#include <QFile>


#include "viewlogwidget.h"

ViewLogWidget::ViewLogWidget(MainObject *mOb, QWidget *parent) :
    QWidget(parent)
{

	mainObject = mOb;
	
	setProperty("settings_namespace", QVariant("log_window"));
	mainObject->settings->restoreWindow(this);

	setWindowIcon(QIcon(":/icon/log"));
	setWindowTitle(tr("View Log"));
	setMinimumWidth(600);
	setMinimumHeight(600);

	QHBoxLayout *mainLayout = new QHBoxLayout();

	setLayout(mainLayout);


	QGroupBox *groupBox = new QGroupBox(tr("Log Output"));
	mainLayout->addWidget(groupBox);

	QHBoxLayout *layoutBox = new QHBoxLayout();
	groupBox->setLayout(layoutBox);

	
	txtLog = new QPlainTextEdit();
	layoutBox->addWidget(txtLog);


	QVBoxLayout *layoutButtons = new QVBoxLayout();
	layoutBox->addLayout(layoutButtons);

	buttonRefresh = new QPushButton();
	buttonRefresh->setText(tr("Refresh"));
	buttonRefresh->setIcon(QIcon(":/icon/refresh"));
	connect(buttonRefresh, SIGNAL(clicked()), this, SLOT(on_load_log_file()));

	layoutButtons->addWidget(buttonRefresh);


	layoutButtons->addStretch(20);

	on_load_log_file();
}


void ViewLogWidget::on_load_log_file(){

	QString file_name = mainObject->log_file_path();
	QFile file(file_name);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		return;
	}
	txtLog->setPlainText( file.readAll());

}

//= window close
void ViewLogWidget::closeEvent(QCloseEvent *event){
	mainObject->settings->saveWindow(this);
	event->accept();
}
