#ifndef FLIGHTSMODEL_H
#define FLIGHTSMODEL_H

#include <QStandardItemModel>

#include <QTimer>
#include <QDateTime>

// @todo:  qt5 this could be QJson..
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>


#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "flights/flightpositions.h"
class FlightPositions;

#include "marble/GeoDataCoordinates.h"
using namespace Marble;


class FlightsModel : public QStandardItemModel
{
Q_OBJECT

public:
    explicit FlightsModel(QObject *parent=0);


    enum COLS{
        // The yet to test atomic geoff.cf.Id..
        C_FLIGHT_ID = 0,
        C_CALLSIGN,       
        C_ALTITUDE,
        C_HEADING,
        C_SPEED,
        C_AERO,
        C_LAT,
        C_LON,
        C_TS,
        C_CURRENT
    };

    enum ROLES{
        SORT_ROLE = Qt::UserRole + 1
    };

    int lag;

    QTimer *timer;

    QNetworkAccessManager *netMan;

    // flightPositions
    QHash<QString, FlightPositions*> flightPositions;


signals:
    void update_done();

public slots:

    void on_dns(const QHostInfo &host);
    void fetch_server();
    void on_server_finished(QNetworkReply *reply);

    static QColor get_altitude_color(QString altitude, int alpha);

};

#endif // FLIGHTSMODEL_H
