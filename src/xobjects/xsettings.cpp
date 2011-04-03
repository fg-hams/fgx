

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QVariant>

#include <QtGui/QDesktopServices>
#include <QtGui/QWidget>

#include "xsettings.h"


XSettings::XSettings(QObject *parent) :
    QSettings(parent)
{
}

//===========================================================================
//** hAcKer - this to be removed
//===========================================================================
bool XSettings::_dev_mode(){
	//** Hack for petes workstation in dev mode
	return QFile::exists("/home/ffs/fgx/DEV.txt");
}

//===========================================================================
//** fgfs Executable Full Path
//===========================================================================
QString XSettings::fgfs_path(){
	// TODO = return the default windows path
	return this->value("FGFS", default_fgfs_path()).toString();
}

QString XSettings::default_fgfs_path(){

	if(runningOS() == MAC){
		return QDir::currentPath().append("/fgx.app/Contents/MacOS/fgfs");

	}else if(runningOS() == LINUX){
		return QString("fgfs");
	}

	return QString("TODO - default_fgfs_path()");
}

//===========================================================================
//** fg_root()
//===========================================================================
QString XSettings::fg_root(){
	return this->value("FG_ROOT", default_fg_root()).toString();
}

QString XSettings::fg_root(QString append_path){
    return this->fg_root().append(append_path);
}

QString XSettings::default_fg_root(){
	if(runningOS() == MAC){
		return QDir::currentPath().append("/fgx.app/Contents/Resources");

	}else if(runningOS() == LINUX){
		return QString("/usr/share/games/FlightGear");
	}

	return QString("TODO - default_fg_root()");
}

//===========================================================================
//** Paths Sane
//===========================================================================
bool XSettings::paths_sane(){
	if(value("USE_DEFAULT_PATHS").toBool()){
		return true;
	}
	if(!QFile::exists(fgfs_path())){
		return false;
	}
	if(!QFile::exists(fg_root())){
		return false;
	}
	return true;
}


//===========================================================================
//** Aircraft Path
//===========================================================================
QString XSettings::aircraft_path(){
	return fg_root().append("/Aircraft");
}

QString XSettings::aircraft_path(QString dir){
	return fg_root().append("/Aircraft/").append(dir);
}


//===========================================================================
//** Aiprots path
//===========================================================================
QString XSettings::airports_path(){
	if(use_terrasync()){
		return scenery_path().append("/Airports");
	}
	return fg_root().append("/Scenery/Airports");
}


//===========================================================================
//** Scenery Path
//===========================================================================
QString XSettings::scenery_path(){
	if(use_terrasync()){
		return terrasync_path();
	}
	return fg_root("/Scenery");
}

//===========================================================================
//** TerraSync
//===========================================================================
bool XSettings::use_terrasync(){
	return value("use_terrasync").toBool();
}
QString XSettings::terrasync_path(){
	return value("terrasync_path").toString();
}


//===========================================================================
//** temp
//===========================================================================
QString XSettings::temp_dir(){
	return QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation)).absolutePath();
}
QString XSettings::temp_dir(QString append_path){
	return temp_dir().append(append_path);
}


//===========================================================================
//** Save/Restore Window
//===========================================================================
void XSettings::saveWindow(QWidget *widget){
	setValue( _windowName(widget), QVariant(widget->saveGeometry()) );
}
void XSettings::restoreWindow(QWidget *widget){
	widget->restoreGeometry( value(_windowName(widget)).toByteArray() );
}
QString XSettings::_windowName(QWidget *widget){
	QString key_name = "window/";
	key_name.append(widget->property("settings_namespace").toString());
	key_name.append("/geometry");
	return key_name;
}

//===========================================================================
//** OS detection
//===========================================================================
int XSettings::runningOS() {
    #ifdef Q_WS_X11
		return XSettings::LINUX;
    #endif

    #ifdef Q_WS_MAC
		return XSettings::MAC;
    #endif

    #ifdef Q_WS_WIN
		return XSettings::WINDOWS;
    #endif

	return XSettings::UNKNOWN;
}

//===========================================================================
//** FGCom
//===========================================================================
QString XSettings::default_fgcom_no(){
	return QString("fgcom -Sfgcom.flightgear.org.uk");
}
QString XSettings::default_fgcom_port(){
	return QString("16661");
}

//===========================================================================
//** Database
//===========================================================================
QString XSettings::db_file(){
	QString storedir = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absolutePath();
	//** Wtf = expected Qt to create this location automatically mydata/<exe>/<App Name>
	if(!QFile::exists(storedir)){
		QDir *dir = new QDir("");
		dir->mkpath(storedir);
	}
	return storedir.append("/fgx.sqlite.db");
}
