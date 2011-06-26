
#include <QDebug>

#include <QtCore/QDir>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtGui/QProgressDialog>

#include <QtXmlPatterns/QXmlQuery>
#include <QtXml/QDomDocument>
#include <QtScript/QScriptEngine>

#include "aircraftimport.h"
#include "utilities/utilities.h"

AircraftImport::AircraftImport(QObject *parent, MainObject *mOb) :
    QObject(parent)
{
	mainObject = mOb;
}


//========================================================
//*** Walk XML - sets
//========================================================

/* This function walks the /Aircraft/ directory
 its find files in a directory maching "-set.xml"
   and therby engquires the xml fo
*/

void AircraftImport::scan_xml_sets(){

	int c = 0;
	int found = 0;
	

	
	//= Cache File
	QString cache_file_name = mainObject->settings->data_file("airports.txt");
	QFile cacheFile( mainObject->settings->data_file("airports.txt") );
	qDebug() << "Cahce file=" << cache_file_name;
	if(!cacheFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		qDebug() << "TODO Open error cachce file=" << cache_file_name;
		return;
	}

	QProgressDialog progress(tr("Scanning aircraft directory ..."), tr("Cancel"), 0, 0);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();


	QTextStream out(&cacheFile);


	//= Get files Entries from Aircaft/ directory
	QDir aircraftDir( mainObject->settings->aircraft_path() );
	aircraftDir.setFilter( QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	qDebug() << "aircraft directory path: " << mainObject->settings->aircraft_path();

	QStringList entries = aircraftDir.entryList();
	progress.setMaximum(entries.size());

	for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){

		// Filter out default dir names, should be a QDir name filter?
		if (*entry != "Instruments" &&  *entry != "Instruments-3d" && *entry != "Generic") {

			progress.setValue(progress.value() + 1);

			//** get the List of *-set.xml files in dir
			QDir dir( mainObject->settings->aircraft_path(*entry) );
			QStringList filters;
			filters << "*-set.xml";
			QStringList list_xml = dir.entryList(filters);

			if(list_xml.count() > 0){ // << Scan MOdels
				QString directory;
				QString description;
				QString author;
				QString fdm;
				QString xml_file;
				QString aero;

				//** Add Path Node
				directory = QString(*entry);
				//** Add Models
				for (int i = 0; i < list_xml.size(); ++i){

					xml_file = QString(list_xml.at(i));
					aero = QString(xml_file);
					aero.chop(8);

					//*=parse the Xml file - f&*& long winded
					QString file_path =  mainObject->settings->aircraft_path(*entry);
					file_path.append("/");
					file_path.append(list_xml.at(i));
					QFile xmlFile( file_path);
					if (xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){

						/* The file content is converted to UTF-8.
							 Some files are Windows, encoding and throw error with QxmlQuery etc
							 Its a hack and don't quite understand whats happening.. said pedro
						*/
						QString xmlString = QString(xmlFile.readAll()).toUtf8();

						QXmlQuery query;
						query.setFocus(xmlString);
						//query.setFocus(&xmlFile); << Because file is not QTF8 using sting instead
						query.setQuery("PropertyList/sim");
						if (query.isValid()){

							QString res;
							query.evaluateTo(&res);
							xmlFile.close();

							QDomDocument dom;
							dom.setContent("" + res + "");
							QDomNodeList nodes = dom.elementsByTagName("sim");

							QDomNode n = nodes.at(0);
							description = n.firstChildElement("description").text();
							author = n.firstChildElement("author").text();
							fdm = n.firstChildElement("flight-model").text();
						} /* !query.isValid() */
					} /*  xmlFile.open() */

					QStringList lines;
					lines << aero << directory << xml_file << description << fdm << author << file_path;
					out << lines.join("\t") << "\n";

					found++;
//					list.clear();
//					list += aero;
//					list += directory;
//					list += xml_file;
//					list += description;
//					list += fdm;
//					list += author;
//					rows += list;

					if(progress.wasCanceled()){
						qDebug() << "Progress cancelled!";
						break; // TODO ?? why..
					}
					c++;
				} 

			} /* list_xml.count() > 0 */
		} /* entry != INstruments etc */
	} /* loop entries.() */	

	cacheFile.close();
	progress.hide();
}


