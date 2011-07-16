#ifndef EXPERTOPTIONSWIDGET_H
#define EXPERTOPTIONSWIDGET_H

#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtGui/QButtonGroup>

#include "xobjects/mainobject.h"

class ExpertOptionsWidget : public QWidget
{
Q_OBJECT
public:
	explicit ExpertOptionsWidget(MainObject *mOb, QWidget *parent = 0);

	MainObject *mainObject;

	// Advanced Options
	QPlainTextEdit *txtExtraArgs;
	QPlainTextEdit *txtExtraEnv;
	QLineEdit *txtRuntime;

	XGroupVBox *groupBoxWriteLog;
	QComboBox *comboLogLevels;

	QStringList get_args();
	QStringList get_env();
	QString		get_runtime();


	// preview
	QPlainTextEdit *txtPreviewOutput;
	QPushButton *buttonCommandPreview;
	QPushButton *buttonCommandHelp;
	QPushButton *buttonCommandVersion;
	QButtonGroup *buttonGroup;


signals:

public slots:

	void on_command_help();
	void on_command_version();
	void preview();

	void save_settings();
	void load_settings();
};

#endif // EXPERTOPTIONSWIDGET_H