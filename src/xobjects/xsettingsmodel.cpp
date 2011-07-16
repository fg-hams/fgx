

#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>

#include "xobjects/xsettingsmodel.h"




/*! \class XSettingsModel
 * \brief The XSettingsModel is an extended QStandardItemModel, and the main "setting and state" for FGx
 *
 *
 * @author: Peter Morgan
 */



XSettingsModel::XSettingsModel(MainObject *mob, QObject *parent) :
    QStandardItemModel(parent)
{
	mainObject = mob;

	_loading = false;

	setColumnCount(7);

	QStringList headers;
	headers  << "Option" << "Ena" << "Val" << "Default" << "Level" << "Description" << "Class";
	setHorizontalHeaderLabels(headers);

	//========================================================================
	// This list wil be the main issue and debates for a long time probably, said pete
	//========================================================================


	//+++++++
	add_option("fgfs_custom_path", false,"","",0,"","paths");
	add_option("fgroot_custom_path", false,"","",0,"","paths");
	add_option("terrasync_path", false,"","",0,"","paths");


	add_option("extra_args", false,"","",0,"","expert");
	add_option("extra_env", false,"","",0,"","expert");

	add_option("runtime", false,"","",0,"","expert");
	add_option("--log-level=", false,"","",0,"","expert");

	//==


	add_option( "show_mpmap", false, "", "",10,"Follow in MpMap","Map");


	add_option( "--enable-auto-coordination",false, "", "",10,"Enabled Rudder and Airelon","Control");

	add_option( "--full-screen",false, "", "", 1 ,"Enable Full Screen at Startup","Startup");
	add_option( "--disable-splash-screen", false, "", "", 1 ,"Enable Splash Screen at Startup","Startup");


	add_option( "--callsign=", true,"", "fgx001", 1 ,"Your Callsign","Core");



	add_option( "--airport=", false,"", "", 1 ,"Airport","Airport");


	//= Season
	add_option("--season=", false,"","summer",1,"Season", "time");

	//= Time
	add_option("--timeofday=", false,"","realtime",1,"Time of day", "time");

	//= Weather
	add_option("weather",						true, "","none",2,"Weather Flag","weather");
	add_option("--enable-real-weather-fetch",	false, "","",1,"Enable Real Weather","weather");
	add_option("--disable-real-weather-fetch",	false, "","",1,"Disable Real Weather","weather");
	add_option("--metar=",						false,"","",1,"","weather");


	//= MultiPlayer
	add_option( "enable_mp", false, "","",1,"","");
	add_option( "--multiplay=in",  false,"", ",10,localhost,20",2,"Multiplayer In","MultiPlayer");
	add_option( "--multiplay=out", false, "", ",10,localhost,20",2,"Multiplayer In","MultiPlayer");

	//= FGCom Related
	add_option( "--fgcom=",false, "", "fgcom.flightgear.org.uk:16661",3,"FgCom","FgCom");
	add_option( "fgcom_generic_socket", false, "", "",10,"FgCom Socket","FgCom");

	//= Local Servers
	add_option( "--telnet=",false, "", "",3,"Enable Telnet","servers");
	add_option( "--httpd=",false, "", "",3,"Enable Httpd","servers");
	add_option( "--jpg-httpd=",false, "", "",3,"Enable Screenshot","servers");


	//-- Radio
	add_option( "--nav1=",false, "", "",3,"","Radio");
	add_option( "--nav2=",false, "", "",3,"","Radio");
	add_option( "--adf=",false, "", "",3,"","Radio");
	add_option( "--com1=",false, "", "",3,"","Radio");
	add_option( "--com2=",false, "", "",3,"","Radio");

	add_option( "--aircraft=", false,"", "", 1 ,"Aircraft","Aircraft");
	add_option( "use_default_aircraft",true,"","",1,"","Aircraft");
	
	//-- Fuel
	add_option( "use_default_fuel",true,"","",3,"","Aircraft");
	add_option( "--enable-fuel-freeze",false,"","",3,"","Aircraft");
	add_option( "--prop:/consumables/fuels/tank[1]/level-gal=",false, "", "",9,"","Fuel");
	add_option( "--prop:/consumables/fuels/tank[2]/level-gal=",false, "", "",9,"","Fuel");
	add_option( "--prop:/consumables/fuels/tank[3]/level-gal=",false, "", "",9,"","Fuel");
	
	//* Airport Tab
	
	// Coordinates
	add_option( "use_coordinates",false, "", "",3,"","Coordinates");
	add_option( "--vor=",false, "", "",3,"","Coordinates");
	add_option( "--ndb=",false, "", "",3,"","Coordinates");
	add_option( "--fix=",false, "", "",3,"","Coordinates");
	add_option( "--lat=",false, "", "",3,"","Coordinates");
	add_option( "--lon=",false, "", "",3,"","Coordinates");
	add_option( "--offset-distance=",false, "", "",3,"","Coordinates");
	add_option( "--altitude=",false, "", "",3,"","Coordinates");
	add_option( "--heading=",false, "", "",3,"","Coordinates");



}



//==================================================
// == Add an Option
void XSettingsModel::add_option( QString option, bool enabled, QString value, QString preset, int level, QString description, QString area)
{
	Q_UNUSED(value); //= Uses preset intead
	QList<QStandardItem *>items;
	items 	<< new QStandardItem(option)
			<< new QStandardItem(enabled ? "1" : "0")
			<< new QStandardItem(preset)
			<< new QStandardItem(preset)
			<< new QStandardItem(QString::number(level))
			<< new QStandardItem(description)
			<< new QStandardItem(area);
	appendRow(items);

}



//==================================================
// == Set An Option
void XSettingsModel::set_option(QString option, bool enabled, QString value)
{
	qDebug() << "set " << option << _loading;
	if(_loading){
		return;
	}
	//= Find item matching the "option"
	QList<QStandardItem *>items = findItems(option, Qt::MatchExactly,C_OPTION);
	//qDebug() << "opts" << items;

	//TODO handle error if not found

	//= Get/update the "enabled" item in the same row
	QStandardItem *eItem = item(items[0]->row(),C_ENABLED);
	eItem->setText(enabled ? "1" : "0");


	//= Get/update the "value" item in the same row
	QStandardItem *vItem = item(items[0]->row(),C_VALUE);
	vItem->setText(value);

	//= Announce the change
	emit upx(option, enabled,  value);
	emit updated(get_fgfs_list());
}

//==================================================
// == Get Option
QString XSettingsModel::getx(QString option)
{
	QList<QStandardItem *>items = findItems(option, Qt::MatchExactly,C_OPTION);
	//#qDebug() << "opts" << items;

	// Get the item in the same row in the enabled field
	QStandardItem *xItem = item(items[0]->row(),C_VALUE);
	return xItem->text();

}


//==================================================
// == Get Option enabled
bool XSettingsModel::get_ena(QString option)
{
	QList<QStandardItem *>items = findItems(option, Qt::MatchExactly,C_OPTION);
	//#qDebug() << "opts" << items;

	// Get the item in the same row in the enabled field
	QStandardItem *xItem = item(items[0]->row(),C_ENABLED);
	return xItem->text() == "1";

}

XOpt XSettingsModel::get_opt(QString option)
{
	QList<QStandardItem *>items = findItems(option, Qt::MatchExactly,C_OPTION);
	return XOpt(item(items[0]->row(),C_OPTION)->text(),
				item(items[0]->row(),C_ENABLED)->text() == "1",
				item(items[0]->row(),C_VALUE)->text()
				);
}


//==================================================
//= File Functions
//==================================================
QString XSettingsModel::ini_file_path()
{
	return mainObject->settings->data_file("profile.ini");
}


// == Write Ini
void XSettingsModel::write_ini()
{
	//= create ini settings object
	
	QString fileName =
	QFileDialog::getSaveFileName(0,
								 "Save Profiles",
								 ini_file_path(),
								 "Profile files (*.ini)" );

	QSettings settings(fileName, QSettings::IniFormat);

	//= loop rows and save each "option" as an [ini section] with enabled, value as values
	for(int row_idx=0; row_idx < rowCount(); row_idx++){
		settings.beginGroup(item(row_idx, C_OPTION)->text());
			settings.setValue( "enabled", item(row_idx, C_ENABLED)->text());
			settings.setValue( "value", item(row_idx, C_VALUE)->text());
		settings.endGroup();
	}
	qDebug() << "Written ini";
}


// == Read Ini
void XSettingsModel::read_ini()
{
	_loading = true;

	//= Create ini settings object
	QString fileName =
	QFileDialog::getOpenFileName(0,
								 "Load Profiles",
								 ini_file_path(),
								 "Profile files (*.ini)" );
	
	QSettings settings(fileName, QSettings::IniFormat);
	

	for(int row_idx=0; row_idx < rowCount(); row_idx++){
		//= loop rows and load each "option" as an [ini section] with enabled, value as values
		settings.beginGroup(item(row_idx, C_OPTION)->text());
			item(row_idx, C_ENABLED)->setText( settings.value("enabled").toBool() ? "1" : "0" );
			QString val = settings.value("value").toString();
			if(val == ""){
				val = item(row_idx, C_DEFAULT)->text();
			}
			item(row_idx, C_VALUE)->setText(val );

			//= Broadcast changes
			emit upx(item(row_idx, C_OPTION)->text(),
					 item(row_idx, C_ENABLED)->text() == "1",
					 item(row_idx, C_VALUE)->text()
					 );
		settings.endGroup();
	}
	_loading = false;
	qDebug() << "Read ini";
	emit updated(get_fgfs_list());
}











//==========================================================
//= Get Options
//==========================================================
QStringList XSettingsModel::get_fgfs_args()
{
	//= Read --options from tree
	QStringList args;

	for(int row_idx=0; row_idx < rowCount(); row_idx++){

		if(item(row_idx, C_ENABLED)->text() == "1"){
			QString str("");
			QString op = item(row_idx, C_OPTION)->text();
			if(op.startsWith("--")){
				str.append(item(row_idx, C_OPTION)->text()).append(item(row_idx, C_VALUE)->text());
			}
			if(str.length() > 0){
				args << str;
			}


			// Process Unique Items
			if(op == "--fgcom="){
				args << getx("fgcom_generic_socket");
			}

		}
	}


	//= add Extra args
	QString extra = getx("extra_args");
	if (extra.length() > 0) {
		QStringList parts = extra.split("\n");
		if(parts.count() > 0){
			for(int i=0; i < parts.count(); i++){
				QString part = parts.at(i).trimmed();
				if(part.length() > 0){
					args << part;
				}
			}
		}
	}

	//== FgRoot
	args <<  QString("--fg-root=").append(fgroot());


	//args << QString("--fg-scenery=").append(settings->terrasync_sync_data_path()).append(":").append(settings->scenery_path());
	//args << QString("--atlas=socket,out,5,localhost,5505,udp");
	//	args << QString ("--geometry=").append(settings->value("screen_size").toString());

	//

	return args;
}




QStringList XSettingsModel::get_fgfs_list()
{
	//TODO append the commands here
	QStringList args;
	args << fgfs_path();
	args << get_fgfs_args();
	return args;
}
QString XSettingsModel::get_fgfs_command_string()
{
	return get_fgfs_args().join(" ");
}







//===========================================================================
//== fgfs Executable
//===========================================================================
/** \brief Path to fgfs executable
 */
QString XSettingsModel::fgfs_path(){
	if(fgfs_use_default()){
		return fgfs_default_path();
	}
	return getx("fgfs_custom_path");
}

/** \brief true is the default path is being used
 */

bool XSettingsModel::fgfs_use_default(){
	return get_ena("fgfs_custom_path") == false;
}

/** \brief absolute default path for the fgfs executable
 */
QString XSettingsModel::fgfs_default_path(){

	if(mainObject->runningOs() == MainObject::MAC){
		return QDir::currentPath().append("/fgx.app/Contents/MacOS/fgfs");

	}else if(mainObject->runningOs() == MainObject::LINUX){
		return QString("/usr/local/bin/fgfs");


	}else if(mainObject->runningOs() == MainObject::WINDOWS){
		return QString("C:/Program Files/FlightGear/bin/win32/fgfs.exe");
	}

	return QString("UNKNOW OS in default_fgfs_path()");
}



//===========================================================================
//== fgroot  path
//===========================================================================
/** \return The absolute path to FG_ROOT ie /fgdata directory */
QString XSettingsModel::fgroot(){
	if(fgroot_use_default()){
		return fgroot_default_path();
	}
	return getx("fgroot_custom_path");
}
bool XSettingsModel::fgroot_use_default(){
	return get_ena("fgroot_custom_path") == false;
}

/** \brief Path to FG_ROOT with appended path
 *
 * Returns the path to the FG_ROOT. If the default install
 * is selected, then that is returned, otherwise the custom selected fg_data path.
 * The file is appended with the append_path
 * \return The absolute path.
 */

QString XSettingsModel::fgroot(QString append_path){
	return this->fgroot().append(append_path);
}

/** \brief Platform specific default path for the FG_ROOT dir
 *
  * \return The absolute path to FG_ROOT
 */
QString XSettingsModel::fgroot_default_path(){

	if(mainObject->runningOs() == MainObject::MAC){
		return QDir::currentPath().append("/fgx.app/Contents/Resources/fgx-fgdata");

	}else if(mainObject->runningOs() == MainObject::LINUX){
		return QString("/usr/share/games/FlightGear");

	}else if(mainObject->runningOs() == MainObject::WINDOWS){
		return QString("C:/Program Files/FlightGear/data");
	}

	return QString("Your system is not handled for default fgdata path");
}

//===========================================================================
//** Paths Sane
//===========================================================================
/** \brief Checks whether the executablem FG_ROOT paths are sane.
 *
 * \return true if sane
 */
bool XSettingsModel::paths_sane(){
	if(!QFile::exists(fgfs_path())){
		return false;
	}
	if(!QFile::exists(fgroot())){
		return false;
	}
	return true;
}


//===========================================================================
//** Aircraft Path
//===========================================================================
/** \brief The path to the /Aircraft directory
 *
  * \return absolute path.
 */
QString XSettingsModel::aircraft_path(){
	return fgroot().append("/Aircraft");
}

/** \brief Path to the /Aircraft directory with a dir appended.
 *
 * \return absolute path.
 */
QString XSettingsModel::aircraft_path(QString dir){
	return fgroot().append("/Aircraft/").append(dir);
}


//===========================================================================
//** Airports path
//===========================================================================
/** \brief Return the absolute path to the /Airports directory
 *
 * \return If TerraSync is enabled, then returns a terrasync folder, otherwise the default.
 */
QString XSettingsModel::airports_path(){
	QString rpath;
	//= Using terrasync
	if(terrasync_enabled()){
		if(mainObject->runningOs() == MainObject::MAC){
			rpath = QDir::homePath();
			rpath.append("/Documents/TerrasyncScenery");
		}else{
			// Use the terra sync path
			rpath = terrasync_sync_data_path().append("/Airports");
		}
	} else{ // Otherwise return the FG_ROOT airports/
		rpath = fgroot().append("/Scenery/Airports");
	}
	//outLog("*** FGx settings: Airports path: " + rpath + " ***");
	return rpath;
}

//===========================================================================
//** Scenery Path - TODO x/win
//===========================================================================
/** \brief Returns the absolute path to the /Scenery file
 *
 * \return The absolute path.
 */
QString XSettingsModel::scenery_path(){

	return fgroot("/Scenery");
}

//===========================================================================
//** TerraSync
//===========================================================================
/** \brief  Using terrasync for scenery
 *
 * \return true if using terrasync
 */
bool XSettingsModel::terrasync_enabled(){
	return get_ena("terrasync_path");
}
/** \brief terrasync executable
 *
  * \return path to terrasync executable
  * \todo Windows path
 */
QString XSettingsModel::terrasync_exe_path(){
	if (mainObject->runningOs() == MainObject::MAC) {
		//* points to terrasync binary in app bundle
		return QDir::currentPath().append("/fgx.app/Contents/MacOS/terrasync");

	}else if(mainObject->runningOs() == MainObject::LINUX){
		return QString("terrasync");

	}else if(mainObject->runningOs() == MainObject::WINDOWS){
		return QString("TODO/path to terrasync.exe");
	}
	return QString("TODO - terrasync");
}
/** \brief terrasync data path
 *
  * \return path to where terrasync will store files downloaded.
 */
QString XSettingsModel::terrasync_sync_data_path(){
	return getx("terrasync_path");
}

