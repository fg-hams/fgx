// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// FGx FlightGear Launcher // airportswidget.cpp
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-
// (c) 2010-2012
// Yves Sablonier, Pete Morgan
// Geoff McLane
// GNU GPLv2, see main.cpp and shipped licence.txt for further information


#include <QtCore/QString>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QXmlStreamReader>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>

#include <QDesktopServices>
#include <QMessageBox>
#include <QSizePolicy>
#include <QFont>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>

#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QLineEdit>

#include <QStandardItemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QHeaderView>

#include <QProgressDialog>



#include "airports/airportswidget.h"
#include "airports/airportsdata.h"
#include "airports/importairportsdialog.h"
#include "fgtools/fileviewerwidget.h"
#include "utilities/helpers.h"


AirportsWidget::AirportsWidget(MainObject *mOb, QWidget *parent) :
    QWidget(parent)
{

    mainObject = mOb;

    //= Metar widget
    metarWidget = new MetarWidget(mainObject);
    metarWidget->hide();


    //* Main Layout
    QHBoxLayout *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);
    mainLayout->setSpacing(10);
    int m = 10;
    mainLayout->setContentsMargins(m,m,m,m);


    //= Tab Widget
    //tabWidget = new QTabWidget();
    //mainLayout->addWidget(tabWidget);
    //connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(on_tab_changed()));



    //=================================================================================
    //* Airport Details
    groupBoxAirport = new XGroupVBox("Start at airport");
    //tabWidget->addTab(groupBoxAirport, QIcon(":/icon/XXX"), tr("Start At Aiport"));

    groupBoxAirport->xLayout->setContentsMargins(10,10,10,10);
    groupBoxAirport->xLayout->setSpacing(0);
    mainLayout->addWidget(groupBoxAirport);

    //==================================================================
    //** Airports Filter and Actions Top Bar
    QHBoxLayout *layoutAptTopBar = new QHBoxLayout();
    layoutAptTopBar->setContentsMargins(0,0,0,0);
    layoutAptTopBar->setSpacing(10);
    groupBoxAirport->addLayout(layoutAptTopBar);

    //** Filter Code
    layoutAptTopBar->addWidget(new QLabel(tr("Filter").append(":")));

    //** Filter Buttons - TODO
    buttonGroupFilter = new QButtonGroup(this);
    buttonGroupFilter->setExclusive(true);
    connect(	buttonGroupFilter, SIGNAL(buttonClicked(QAbstractButton*)),
                this, SLOT(on_update_airports_filter())
    );

    //** Aiport Code Filter
    QRadioButton *buttCode = new QRadioButton();
    buttCode->setText("Code");
    buttCode->setProperty("filter_column_idx", QVariant(CA_CODE));
    buttCode->setChecked(true);
    layoutAptTopBar->addWidget(buttCode);
    buttonGroupFilter->addButton(buttCode, 0);

    //** Airport Name Filter
    QRadioButton *buttName = new QRadioButton();
    buttName->setText("Name");
    buttName->setProperty("filter_column_idx", QVariant(CA_NAME));
    layoutAptTopBar->addWidget(buttName);
    buttonGroupFilter->addButton(buttName, 1);

    //** Find Airport Text
    txtAirportsFilter = new QLineEdit();
    layoutAptTopBar->addWidget(txtAirportsFilter);
    txtAirportsFilter->setFixedWidth(100);
    connect(	txtAirportsFilter, SIGNAL(textChanged(QString)),
                this, SLOT(on_update_airports_filter())
    );

    layoutAptTopBar->addStretch(20);


    QToolButton *buttonReloadCache = new QToolButton(this);
    layoutAptTopBar->addWidget(buttonReloadCache);
    buttonReloadCache->setText("Reload");
    buttonReloadCache->setToolTip("Scan directories and reload cache");
    buttonReloadCache->setIcon(QIcon(":/icon/load"));
    buttonReloadCache->setAutoRaise(true);
    buttonReloadCache->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(buttonReloadCache, SIGNAL(clicked()), this, SLOT(on_reload_cache()) );

    QMenu *meniw = new QMenu();
    buttonReloadCache->setMenu(meniw);
    buttonReloadCache->setPopupMode(QToolButton::MenuButtonPopup);

    QAction *actView = meniw->addAction("View `airports.txt` cache file" );
    connect(actView, SIGNAL(triggered()),
            this, SLOT(on_view_airports_cache())
    );
    //==============================================================================
    //== Airports/Runways Splitter
    QSplitter *splitter = new QSplitter(this);
    groupBoxAirport->addWidget(splitter);


    //=== Airport Widget + Layout
    QWidget *airportWidget = new QWidget();
    splitter->addWidget(airportWidget);
    QVBoxLayout *airportsLayout = new QVBoxLayout();
    airportsLayout->setContentsMargins(0,0,0,0);
    airportsLayout->setSpacing(0);
    airportWidget->setLayout(airportsLayout);

    /*  ======== NOTE ============
        # The Airports tree is a treeView
            ## with a model/proxt to enable filtersing
            ## and the columns constants a CA_
        # The Airport Info is a treeWidget
            ## uses CI_ as the column constants
    */

    //==============================================
    //== Airport Models
    model = new QStandardItemModel(this);
    model->setColumnCount(2);
    QStringList headerLabelsList;
    headerLabelsList << tr("Code") << tr("Name") << "Dir";
    model->setHorizontalHeaderLabels(headerLabelsList);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn( CA_NAME );

    //==============================================
    //==  TreeView aiports
    treeAirports = new QTreeView(this);
    airportsLayout->addWidget(treeAirports, 1, 0);
    treeAirports->setModel(proxyModel);

    treeAirports->setUniformRowHeights(true);
    treeAirports->setAlternatingRowColors(true);
    treeAirports->setRootIsDecorated(false);
    treeAirports->setSortingEnabled(true);
    treeAirports->sortByColumn(CA_NAME, Qt::AscendingOrder);
    treeAirports->setSelectionMode(QAbstractItemView::SingleSelection);
    treeAirports->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeAirports->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //* Headers and columns
    treeAirports->header()->setStretchLastSection(true);
    treeAirports->setColumnWidth(CA_CODE, 80);
    treeAirports->setColumnWidth(CA_NAME, 50);
    treeAirports->setColumnHidden(CA_DIR, true); // hide directory column

    connect( treeAirports->selectionModel(),
             SIGNAL( currentChanged(QModelIndex,QModelIndex)),
             this, SLOT( on_airport_tree_selected(QModelIndex, QModelIndex) )
    );

    //================================
    // StatusBar for Airports
    statusBarAirports = new QStatusBar();
    statusBarAirports->setSizeGripEnabled(false);
    airportsLayout->addWidget(statusBarAirports);
    statusBarAirports->showMessage("Idle");

    //= Open Airports Folder
    QToolButton *buttonShowMetar = new QToolButton();
    buttonShowMetar->setAutoRaise(true);
    buttonShowMetar->setIcon(QIcon(":/icon/metar"));
    buttonShowMetar->setText("Metar");
    buttonShowMetar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    statusBarAirports->addPermanentWidget(buttonShowMetar);
    connect(buttonShowMetar, SIGNAL(clicked()), this, SLOT(on_show_metar()));


    //= Label Airports Folder - HIDDEN
    labelAirportsFolder = new QLabel();
    statusBarAirports->addPermanentWidget(labelAirportsFolder);
    labelAirportsFolder->hide();

    //= Open Airports Folder
    buttonOpenAirportsFolder = new QToolButton();
    buttonOpenAirportsFolder->setAutoRaise(true);
    buttonOpenAirportsFolder->setIcon(QIcon(":/icon/folder"));
    statusBarAirports->addPermanentWidget(buttonOpenAirportsFolder);
    connect(buttonOpenAirportsFolder, SIGNAL(clicked()), this, SLOT(on_open_airports_folder()));

    //========================================================
    //** Airport Info Widget
    QWidget *airportInfoWidget = new QWidget();
    splitter->addWidget(airportInfoWidget);
    QVBoxLayout *airportInfoLayout = new QVBoxLayout();
    airportInfoLayout->setContentsMargins(0,0,0,0);
    airportInfoLayout->setSpacing(0);
    airportInfoWidget->setLayout(airportInfoLayout);

    //=========================================================
    //* Airport Info TreeWidget
    treeAptInfo = new QTreeWidget();
    airportInfoLayout->addWidget(treeAptInfo, 3);
    treeAptInfo->setAlternatingRowColors(true);
    treeAptInfo->setRootIsDecorated(true);
    treeAptInfo->setUniformRowHeights(true);
    treeAptInfo->setExpandsOnDoubleClick(false);

    QTreeWidgetItem *headerItem = treeAptInfo->headerItem();
    headerItem->setText(CI_NODE, tr(""));
    headerItem->setText(CI_WIDTH, tr("Width"));
    headerItem->setText(CI_LENGTH, tr("Length"));
    headerItem->setText(CI_LAT, tr("Lat"));
    headerItem->setText(CI_LON, tr("Lon"));
    headerItem->setText(CI_HEADING, tr("Heading"));
    headerItem->setText(CI_RUNWAYS, tr("R"));


    treeAptInfo->setColumnHidden(CI_TYPE,true);
    treeAptInfo->setColumnHidden(CI_LABEL,true);
    treeAptInfo->setColumnHidden(CI_SETTING_KEY,true);
    treeAptInfo->setColumnHidden(CI_WIDTH,true);
    treeAptInfo->setColumnHidden(CI_LENGTH,true);
    treeAptInfo->setColumnHidden(CI_HEADING,true);
    treeAptInfo->setColumnHidden(CI_RUNWAYS, true);
    treeAptInfo->setColumnHidden(CI_LAT, true);
    treeAptInfo->setColumnHidden(CI_LON, true);

    treeAptInfo->setColumnWidth(CI_NODE, 120);
    treeAptInfo->header()->setStretchLastSection(true);

    connect(treeAptInfo, SIGNAL(itemSelectionChanged()),
            this, SLOT(on_airport_info_selection_changed())
    );
    connect(treeAptInfo, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(on_airport_info_double_clicked(QTreeWidgetItem*,int))
    );


    statusBarAirportInfo = new QStatusBar();
    statusBarAirportInfo->setSizeGripEnabled(false);
    airportInfoLayout->addWidget(statusBarAirportInfo);
    statusBarAirportInfo->showMessage("");


    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 2);



    //========================================================================================================
    //** Use Coordinates
    //====================================================================

    /*groupBoxUseCoordinates = new XGroupVBox("Coordinates", this);
    groupBoxUseCoordinates->setMaximumWidth(300);
    tabWidget->addTab(groupBoxUseCoordinates, QIcon(":/icon/XXX"), "Start At Position");

    int space = 5;

    groupBoxUseCoordinates->xLayout->addSpacing(space);

    groupBoxUseCoordinates->addWidget(new QLabel(tr("Latitude (negative is south)")));
    txtLat = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtLat);
    connect(txtLat, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->addWidget(new QLabel(tr("Longtitude (negative is west)")));
    txtLon = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtLon);
    connect(txtLon, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Start at VOR")));
    txtVOR = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtVOR);
    connect(txtVOR, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Start at NDB")));
    txtNDB = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtNDB);
    connect(txtNDB, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Start at FIX")));
    txtFIX = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtFIX);
    connect(txtFIX, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Offset distance")));
    txtOffset = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtOffset);
    connect(txtOffset, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Altitude")));
    txtAltitude = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtAltitude);
    connect(txtAltitude, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Heading")));
    txtHeading = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtHeading);
    connect(txtHeading, SIGNAL(textChanged(QString)), this, SLOT(on_coordinates_changed()));

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Roll")));
    txtRoll = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtRoll);

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Pitch")));
    txtPitch = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtPitch);

    groupBoxUseCoordinates->xLayout->addSpacing(space);
    groupBoxUseCoordinates->addWidget(new QLabel(tr("Airspeed")));
    txtAirspeed = new QLineEdit();
    groupBoxUseCoordinates->addWidget(txtAirspeed);


    groupBoxUseCoordinates->xLayout->addStretch(20);


    connect(buttonGroupFilter, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(on_update_airports_filter())
    );*/




    //============================================================================
    // Map Widget
    //============================================================================
    mapWidget = new OpenLayerWidget(mainObject);
    mapWidget->setMinimumWidth(400);
    mainLayout->addWidget(mapWidget);
    mapWidget->load_map("airport");
    connect(mapWidget, SIGNAL(map_double_clicked(QString,QString, QString)), this, SLOT(on_map_double_clicked(QString, QString, QString)));


    //============================================================================
    //== Main Settings connection
    connect(this, SIGNAL(setx(QString,bool,QString)), mainObject->X, SLOT(set_option(QString,bool,QString)) );
    connect(this, SIGNAL(set_ena(QString,bool)), mainObject->X, SLOT(set_enabled(QString,bool)) );
    connect(mainObject->X, SIGNAL(upx(QString,bool,QString)), this, SLOT(on_upx(QString,bool,QString)));
    connect(mainObject, SIGNAL(on_debug_mode(bool)), this, SLOT(on_debug_mode()));

    on_debug_mode(); // shows hides some stuff

// #ifdef ENABLE_APT_DAT_LOAD // this can no longer be 'safely' turned off
    pAptDat = new loadAptDat(this); // class to do loading
    ploadItem = &pAptDat->loadItem;   // LOADITEM structure
// #endif // #ifdef ENABLE_APT_DAT_LOAD

}

AirportsWidget::~AirportsWidget()
{
    if (pAptDat){
        delete pAptDat; // stop thread is running, and free list
    }
}

//================================================================
//== Map Double Clicked
void AirportsWidget::on_map_double_clicked(QString lat, QString lon, QString heading)
{
    // disable runway/parking - clear tree
    emit set_ena("--runway=", false);
    emit set_ena("--parking-id=", false);
    emit set_ena("--airport=", false);

    /*treeAptInfo->selectionModel()->clear();*/


    //- Set position
    emit setx("--lat=", true, lat);
    emit setx("--lon=", true, lon);
    emit setx("--heading=", true, heading);
    mapWidget->show_aircraft(mainObject->X->getx("--callsign="),
                             lat,
                             lon,
                             heading,
                             "0"
                             );
}


//==============================================
//== Initialize
void AirportsWidget::initialize(){
    static bool first_load_done = false;
    if(first_load_done){
        return;
    }
    QString file = mainObject->X->fgroot("/Airports/apt.dat");
    if (QFile::exists(file)) {
        // pass to thread to load
        first_load_done = true;
    } else {
        file.append(".gz");
        if (QFile::exists(file)) {
            // pass to thread to load
            first_load_done = true;
        }
    }
#ifdef ENABLE_APT_DAT_LOAD
    if (first_load_done) {
        // ploadItem->optionFlag |= lf_noFixName;
        connect(pAptDat,SIGNAL(load_done()),this,SLOT(on_loadaptdat_done()));
        pAptDat->loadOnThread(file);    // start the loading
    }
#endif // #ifdef ENABLE_APT_DAT_LOAD

    if (!QFile::exists(mainObject->data_file("airports.txt"))){
        statusBarAirports->showMessage("No cached data. Click Import");
    }else{
        load_airports_tree();
    }
    first_load_done = true;
}

void AirportsWidget::on_loadaptdat_done()
{
    // first, check if the current apt.dat has been loaded
    QString zf = mainObject->X->fgroot("/Airports/apt.dat");
    if (!QFile::exists(zf)) {
        zf.append(".gz");   // switch to 'gz' file
    }
    if (QFile::exists(zf) && !pAptDat->isFileLoaded(zf) ) {
        connect(pAptDat,SIGNAL(load_done()),this,SLOT(on_loadaptdat_done()));
        if ( pAptDat->loadOnThread(zf) == 0 ) {    // start the thread loading
            outLog("AirportWidget::on_loadaptdat_done() - started load "+zf);
            return;
        }
    }
    // appears it is (or is not valid, in which case nothing will change), so
    // proceed to fill in Airport names
    outLog("AirportWidget::on_loadaptdat_done()");
    int max = model->rowCount();
    int i;
    QString icao, name;
    QStandardItem *item;

    treeAirports->setUpdatesEnabled(false);
    for (i = 0; i < max; i++) {
        icao = model->item(i, CA_CODE)->text();
        item = model->item(i, CA_NAME);
        //name = pAptDat->findNameByICAO(icao,lf_FixName);
        name = pAptDat->findNameByICAO(icao);
        if (name.length())
            item->setText(name);
    }
    treeAirports->setUpdatesEnabled(true);
}


//============================================================================
//== Load Airports Tree
//============================================================================
void AirportsWidget::load_airports_tree(){

    //= Clear existing tree and inhibit updates till end
    model->removeRows(0, model->rowCount());
    treeAirports->setUpdatesEnabled(false);

    //= Airports Cache File
    QFile dataFile(mainObject->data_file(("airports.txt")));
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            treeAirports->setUpdatesEnabled(true);
           return;
    }
    QTextStream in(&dataFile);
    QStringList dupes;  // avoid adding any ICAO twice
    // prepare for dropping the 'path'
    // such a path should be per the current 'user' supplied paths
    QString icao, name, path;
    QString line = in.readLine();
    int cnt;
    //line = in.readLine(); // Why skip this line???
    while(!line.isNull()){

        QStringList cols = line.split("\t");
        cnt = cols.count();

        icao = cols.at(0);  // get the ICAO
        if (dupes.contains(icao)) {
            line = in.readLine();
            continue;   // do NOT add twice
        }
        dupes += icao;
        name = (cnt > 1 ? cols.at(1) : "");
        path = (cnt > 2 ? cols.at(2) : "");

        QStandardItem *itemAirportCode = new QStandardItem();
        itemAirportCode->setText(icao);

        QStandardItem *itemAirportName = new QStandardItem();
        itemAirportName->setText(name);

        QStandardItem *itemAirportDir = new QStandardItem();
        itemAirportDir->setText(path);

        QList<QStandardItem *> items;
        items << itemAirportCode << itemAirportName << itemAirportDir;

        model->appendRow(items);
        line = in.readLine();
    }

    //= Update the status bar with the count
    statusBarAirports->showMessage( QString("%1 airports").arg(model->rowCount()) );

    //= Restore previous airport from settings.. if found
    QList<QStandardItem *> items = model->findItems( mainObject->X->getx("--airport=", "KSFO"), Qt::MatchExactly, CA_CODE);
    if(items.count() > 0){
        // restore selected node
        QModelIndex srcIdx = model->indexFromItem(items[0]);
        QModelIndex proxIdx = proxyModel->mapFromSource(srcIdx);
        treeAirports->selectionModel()->select(proxIdx, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        treeAirports->scrollTo(proxIdx, QAbstractItemView::PositionAtCenter);
        treeAirports->selectionModel()->setCurrentIndex(proxIdx, QItemSelectionModel::Rows);

        //load_info_tree( model->item(srcIdx.row(), CA_DIR)->text(), model->item(srcIdx.row(), CA_CODE)->text() );
        path = model->item(srcIdx.row(), CA_DIR)->text(); // preparing to use USER paths
        labelAirportsFolder->setText(path);
        buttonOpenAirportsFolder->setToolTip(labelAirportsFolder->text());
    }
    treeAirports->setUpdatesEnabled(true);
}


//==============================================================
//== Update Filters
void AirportsWidget::on_update_airports_filter(){

    mainObject->settings->setValue("airports_button_filter", buttonGroupFilter->checkedId());
    int filter_column_id = buttonGroupFilter->checkedButton()->property("filter_column_idx").toInt();
    proxyModel->setFilterKeyColumn( filter_column_id );
    proxyModel->setFilterFixedString( txtAirportsFilter->text() );
    txtAirportsFilter->setFocus();
}


//==============================================================
//== Airport Clicked
void AirportsWidget::on_airport_tree_selected(QModelIndex currentIdx, QModelIndex previousIdx){
    Q_UNUSED(previousIdx);

    txtAirportsFilter->setFocus();

    //= Clear the Info tree
    treeAptInfo->model()->removeRows(0, treeAptInfo->model()->rowCount());
    mapWidget->clear_map();

    //= No selection -eg a filter removing a selected node
    if(!currentIdx.isValid()){
        emit set_ena("--airport=", false);
        emit set_ena("--runway=", false);
        emit set_ena("--parking-id=", false);
        return;
    }

    //== We have an Airport..
    //= Get the airport code forn source model
    QModelIndex srcIndex = proxyModel->mapToSource(currentIdx);
    QString airport_code = model->item(srcIndex.row(), CA_CODE)->text();
    QString airport_dir = model->item(srcIndex.row(), CA_DIR)->text();

    emit setx("--airport=", true, airport_code);
    mainObject->launcherWindow->on_upx("--airport=", true, airport_code); // Show airport in header


    load_info_tree(airport_dir, airport_code);
    mapWidget->zoom_to_airport(airport_code);

}

//==============================================================
// Load the Info Tree
//==============================================================
void AirportsWidget::load_info_tree(QString airport_dir, QString airport_code){

    QString count_label;
    // prepare to use USER current scenery paths
#ifdef ENABLE_NEW_AIRPORT_PATH
    if (airport_code.length() >= 3) {
        QStringList paths = mainObject->X->getSceneryDirs();
        int cnt, i;
        cnt = paths.count();
        if (cnt > 0) {
            QString psep("/");
            QString soup(psep);
            soup.append(airport_code.at(0));
            soup.append(psep);
            soup.append(airport_code.at(1));
            soup.append(psep);
            soup.append(airport_code.at(2));
            //soup.append(psep);
            QString path;
            QDir dir;
            for (i = 0; i < cnt; i++) {
                path = paths.at(i);
                path.append(psep);
                path.append("Airports");
                path.append(soup);
                if (dir.exists(path)) {
                    if (airport_dir != path) {
                        outLog("AirportsWidget::load_info_tree: from ["+airport_dir+"] to ["+path+"]");
                        airport_dir = path;
                    }
                    break;
                }
            }
        }
    }
#endif // ENABLE_NEW_AIRPORT_PATH

    //mapWidget->setUpdatesEnabled(false);

    //== Clear the existing airport
    //mapWidget->clear_airport(airport_code);
    //mapWidget->clear_map();
    //treeAptInfo->setUpdatesEnabled(false);


    //=== Load Tower
    metarWidget->load_metar(airport_code);
    load_tower_node(airport_dir, airport_code);
    labelAirportsFolder->setText( airport_dir );
    buttonOpenAirportsFolder->setToolTip(airport_dir);

    //=== Load Runways
    int runways_count = load_runways_node(airport_dir, airport_code);
    if(runways_count == 0){
        count_label.append(tr("No runway"));
    }else if(runways_count == 1){
        count_label.append(tr("One runway"));
    }else{
        count_label.append(QString::number(runways_count)).append(" runways");
    }

    count_label.append(" / ");

    //==== Load Stands
    int stands_count =  load_parking_node(airport_dir, airport_code);
    if(stands_count == 0){
        count_label.append(tr("No stands"));
    }else if(stands_count == 1){
        count_label.append(tr("One stand"));
    }else{
        count_label.append(QString::number(stands_count)).append(" stands");
    }

    statusBarAirportInfo->showMessage(count_label);



    //==== Restore Runway node from settings
    XOpt optRunway = mainObject->X->get_opt("--runway=");
    if(optRunway.enabled){
        QList<QTreeWidgetItem *> items = treeAptInfo->findItems(
                                                        optRunway.value,
                                                        Qt::MatchExactly | Qt::MatchRecursive,
                                                        CI_NODE);
        if(items.size() > 0){
            treeAptInfo->setCurrentItem(items[0]);
            treeAptInfo->scrollToItem(items[0], QAbstractItemView::EnsureVisible);
            return;
        }
        //return; //= dontbother with stand below as we found runway
    }

    //= Restore Stand node from settings
    XOpt optStand = mainObject->X->get_opt("--parking-id=");
    if(optStand.enabled){
        QList<QTreeWidgetItem *> items = treeAptInfo->findItems(
                                                        optStand.value,
                                                        Qt::MatchExactly | Qt::MatchRecursive,
                                                        CI_NODE);
        if(items.size() > 0){
            treeAptInfo->setCurrentItem(items[0]);
            treeAptInfo->scrollToItem(items[0], QAbstractItemView::EnsureVisible);
            return;
        }
    }


    //= Were using --lat's enabled flag to scheckif positin is enabled terrible hack
    XOpt latopt = mainObject->X->get_opt("--lat=");
    if(latopt.enabled){
        mapWidget->show_aircraft(mainObject->X->getx("--callsign="),
                                 mainObject->X->getx("--lat="),
                                 mainObject->X->getx("--lon="),
                                 mainObject->X->getx("--heading="),
                                 "0"
                                 );
    }



}

//==============================================================
// Load Runways
//==============================================================
int AirportsWidget::load_runways_node(QString airport_dir, QString airport_code){

    //= Create the Runways Node
    QTreeWidgetItem *runwaysParent = new QTreeWidgetItem();
    runwaysParent->setText(0, "Runways" );
    runwaysParent->setIcon(0, QIcon(":/icon/folder"));
    treeAptInfo->addTopLevelItem(runwaysParent);
    treeAptInfo->setItemExpanded(runwaysParent, true);
    treeAptInfo->setFirstItemColumnSpanned(runwaysParent, true);


    //==============================================================
    // Parse the <CODE>.threshold.xml file to get runways
    //==============================================================
    /*
    <?xml version="1.0"?>
    <PropertyList>
      <runway>
        <threshold>
          <lon>0.044298885776989</lon>
          <lat>51.505569223906</lat>
          <rwy>10</rwy>
          <hdg-deg>92.88</hdg-deg>
          <displ-m>95</displ-m>
          <stopw-m>55</stopw-m>
        </threshold>
        <threshold>
          <lon>0.065996952433288</lon>
          <lat>51.5048897753222</lat>
          <rwy>28</rwy>
          <hdg-deg>272.88</hdg-deg>
          <displ-m>71</displ-m>
          <stopw-m>90</stopw-m>
        </threshold>
      </runway>
    </PropertyList>
    */

    //* Get the contents of the file
    QString threshold_file( airport_dir.append("/").append(airport_code).append(".threshold.xml") );
    QFile fileXmlThrehsold(threshold_file);
    fileXmlThrehsold.open(QIODevice::ReadOnly);

    //= Make file contents into a string from bytearray
    QString xmlThresholdString = QString(fileXmlThrehsold.readAll()).toUtf8();

    //= Create domDocument - important dont pass string in  QDomConstrucor(string) as ERRORS.. took hours DONT DO IT
    QDomDocument dom;
    dom.setContent(xmlThresholdString); //* AFTER dom has been created, then set the content from a string from the file


    //==================================
    //= Get <runway> nodes
    QDomNodeList nodeRunways = dom.elementsByTagName("runway");

    if (nodeRunways.count() > 0){
        for(int idxd =0; idxd < nodeRunways.count(); idxd++){

            // loops the <runway> nodes
            QDomNode nodeRunway = nodeRunways.at(idxd);

            //== Add runway parent ie X - Y both ends
            QTreeWidgetItem *rItem = new QTreeWidgetItem(runwaysParent);
            rItem->setIcon(0, QIcon(":/icon/runways"));
            rItem->setText(CI_RUNWAYS, "1");
            rItem->setText(CI_NODE, nodeRunway.childNodes().at(0).firstChildElement("rwy").text().append(
                                    " - ").append(
                                            nodeRunway.childNodes().at(1).firstChildElement("rwy").text()
                                    ));
            //treeAptInfo->setItemExpanded(rItem, true);
            treeAptInfo->setFirstItemColumnSpanned(rItem, true);

            //= Runway threshold 0
            QTreeWidgetItem *tItem0 = new QTreeWidgetItem(rItem);
            tItem0->setIcon(0, QIcon(":/icon/runway"));
            tItem0->setText(CI_NODE,  nodeRunway.childNodes().at(0).firstChildElement("rwy").text());
            tItem0->setText(CI_LAT,  nodeRunway.childNodes().at(0).firstChildElement("lat").text());
            tItem0->setText(CI_LON,  nodeRunway.childNodes().at(0).firstChildElement("lon").text());
            tItem0->setText(CI_HEADING,  nodeRunway.childNodes().at(0).firstChildElement("hdg-deg").text());
            tItem0->setText(CI_TYPE, "runway");
            tItem0->setText(CI_SETTING_KEY, QString(airport_code).append("runway").append(
                                            nodeRunway.childNodes().at(0).firstChildElement("rwy").text()));

            //= Runway  threshold 1
            QTreeWidgetItem *tItem1 = new QTreeWidgetItem(rItem);
            tItem1->setIcon(0, QIcon(":/icon/runway"));
            tItem1->setText(CI_NODE,  nodeRunway.childNodes().at(1).firstChildElement("rwy").text());
            tItem1->setText(CI_LAT,  nodeRunway.childNodes().at(1).firstChildElement("lat").text());
            tItem1->setText(CI_LON,  nodeRunway.childNodes().at(1).firstChildElement("lon").text());
            tItem1->setText(CI_HEADING,  nodeRunway.childNodes().at(1).firstChildElement("hdg-deg").text());
            tItem1->setText(CI_TYPE, "runway");
            tItem1->setText(CI_SETTING_KEY, QString(airport_code).append("runway").append(
                                            nodeRunway.childNodes().at(1).firstChildElement("rwy").text()));

            mapWidget->add_runway(	airport_code,
                                  tItem0->text(CI_NODE),
                                  tItem1->text(CI_NODE),
                                  tItem0->text(CI_LAT), tItem0->text(CI_LON),
                                  tItem1->text(CI_LAT), tItem1->text(CI_LON)
                                  );

        }
    }




    return runwaysParent->childCount();
}

//==============================================================
// Load Stands + Parking
//==============================================================
int AirportsWidget::load_parking_node(QString airport_dir, QString airport_code){

    //* Create the Parkings Node
    QTreeWidgetItem *parkingParent = new QTreeWidgetItem();
    parkingParent->setText(0, "Parking" );
    parkingParent->setIcon(0, QIcon(":/icon/folder"));
    treeAptInfo->addTopLevelItem(parkingParent);
    treeAptInfo->setItemExpanded(parkingParent, true);

    //=======================================================================
    // Parse the <groundnet/parking>.threshold.xml file for Parking Postiton
    //=======================================================================
    /*
    <?xml version="1.0"?>
    <groundnet>
      <frequencies>
        <AWOS>12807</AWOS>
        <CLEARANCE>12197</CLEARANCE>
        <GROUND>12190</GROUND>
        <TOWER>12447</TOWER>
        <APPROACH>13497</APPROACH>
      </frequencies>
      <parkingList>
        <Parking index="0"
                 type="gate"
                 name="EGLL London Heathrow Ramp #11111"
                 number="1"
                 lat="N51 28.192"
                 lon="W00 27.892"
                 heading="56"
                 radius="54"
                 airlineCodes="" />
      .... snipped ....
    */

    //* Files in terrasync are named "groundnet.xml"; in scenery their "parking.xml" -- Why asks pete??
    QString file_path(airport_dir.append("/").append(airport_code));
    file_path.append( mainObject->X->terrasync_enabled() ? ".groundnet.xml" : ".parking.xml");

    //* Check parking file exists
    if(QFile::exists(file_path)){

        //* Open file and read contents to string
        QFile ppfile(file_path);
        ppfile.open(QIODevice::ReadOnly);
        QString xmlString = QString(ppfile.readAll()).toUtf8();

        //* Create domDocument - important - don't pass string in  QDomConstrucor(string) as ERRORS.. took hours DONT DO IT
        QDomDocument dom;
        dom.setContent(xmlString); //* AFTER dom has been created, then set the content from a string from the file



        //===================================
        //= Get Frequencies
        QTreeWidgetItem *towerItem = treeAptInfo->findItems("tower", Qt::MatchExactly, CI_TYPE).at(0);
        QDomNodeList nodeFreqs = dom.elementsByTagName("frequencies");

        for(int idxf =0; idxf < nodeFreqs.at(0).childNodes().count(); idxf++){
            double freq = nodeFreqs.at(0).childNodes().at(idxf).firstChild().nodeValue().toDouble() / 100;
            QTreeWidgetItem *freqItem = new QTreeWidgetItem(towerItem);
            freqItem->setText(CI_NODE,
                              QString("%1 - %2").arg(	nodeFreqs.at(0).childNodes().at(idxf).nodeName()
                                                        ).arg( QString::number(freq, 'f', 2)
                                ));

        }


        //== Parking Positions
        QStringList listParkingPositions;

        //= Get <Parking/> nodes and loop thru them and add to list (removing dupes)
        QDomNodeList parkingNodes = dom.elementsByTagName("Parking");
        if (parkingNodes.count() > 0){
            for(int idxd =0; idxd < parkingNodes.count(); idxd++){

                 QDomNode parkingNode = parkingNodes.at(idxd);
                 QDomNamedNodeMap attribs = parkingNode.attributes();
                 QString stand(attribs.namedItem("name").nodeValue());
                 stand.append(attribs.namedItem("number").nodeValue());

                //= Check it doesnt already exist - pete is confused as to multiple entries
                 if(!listParkingPositions.contains(stand)){
                     if(attribs.namedItem("type").nodeValue() == "gate"){

                        //= Append position to eliminate dupes
                        if(!stand.contains(" ")){
                            listParkingPositions.append(stand);
                            QTreeWidgetItem *pItem = new QTreeWidgetItem(parkingParent);
                            pItem->setIcon(0, QIcon(":/icon/stand"));
                            pItem->setText(CI_NODE, stand);
                            pItem->setText(CI_TYPE, "stand");
                            pItem->setText(CI_LAT, Helpers::hmm_to_decimal(attribs.namedItem("lat").nodeValue()));
                            pItem->setText(CI_LON, Helpers::hmm_to_decimal(attribs.namedItem("lon").nodeValue()));
                            pItem->setText(CI_HEADING, attribs.namedItem("heading").nodeValue());
                            pItem->setText(CI_SETTING_KEY, QString(airport_code).append("stand").append(stand));
                            mapWidget->add_stand( airport_code,
                                                  stand,
                                                  pItem->text(CI_LAT),
                                                  pItem->text(CI_LON)
                                                 );
                        }
                    }
                }
            }

        }


    } //= File Exists

    if(parkingParent->childCount() == 0){
        QTreeWidgetItem *pItem = new QTreeWidgetItem(parkingParent);
        pItem->setText(CI_NODE, "None");
        return 0;
    }
    //* return the count
    return parkingParent->childCount();
}

//==============================================================
// Load Tower Node
//==============================================================
void AirportsWidget::load_tower_node(QString airport_dir, QString airport_code){

    //* Create the Parkings Node
    QTreeWidgetItem *towerParent = new QTreeWidgetItem();
    towerParent->setText(0, "No Tower" );
    towerParent->setText(CI_TYPE, "tower");
    towerParent->setIcon(0, QIcon(":/icon/tower"));
    treeAptInfo->addTopLevelItem(towerParent);
    treeAptInfo->setItemExpanded(towerParent, false);

    //=======================================================================
    // Parse the ICAO.twr.xml file for Tower Postiton
    //=======================================================================
    /*
        <?xml version="1.0"?>
        <PropertyList>
          <tower>
            <twr>
              <lon>4.762789</lon>
              <lat>52.307064</lat>
              <elev-m>60.96</elev-m>
            </twr>
          </tower>
        </PropertyList>
    */

    //= Files in terrasync are named "groundnet.xml"; in scenery their "parking.xml" -- Why asks pete??
    QString file_path(airport_dir.append("/").append(airport_code).append(".twr.xml"));

    //= Check tower file exists
    if(QFile::exists(file_path)){

        //* Open file and read contents to string
        QFile ppfile(file_path);
        ppfile.open(QIODevice::ReadOnly);
        QString xmlString = QString(ppfile.readAll()).toUtf8();

        //= Create domDocument - important - don't pass string in  QDomConstrucor(string) as ERRORS.. took hours DONT DO IT
        QDomDocument dom;
        dom.setContent(xmlString); //* AFTER dom has been created, then set the content from a string from the file

        //= Get <tower> nodes
        QDomNodeList towersNode = dom.elementsByTagName("tower");

        towerParent->setText(CI_LAT, towersNode.at(0).childNodes().at(0).firstChildElement("lat").text());
        towerParent->setText(CI_LON, towersNode.at(0).childNodes().at(0).firstChildElement("lon").text());

        mapWidget->add_tower(airport_code, towerParent->text(CI_LAT), towerParent->text(CI_LON));
        towerParent->setText(0, QString("%1 - Tower").arg(airport_code) );


    } /* File Exists */

    //* return the count
    return;
}


void AirportsWidget::on_reload_cache(){

    ImportAirportsDialog *widget = new ImportAirportsDialog();
    int resp = widget->exec();
    if(resp == QDialog::Rejected){
        return;
    }

    QProgressDialog progress(this);
    AirportsData::import(progress, mainObject);
    load_airports_tree();
}



//====================================================================
// Current Selected Aiport
//====================================================================

QString AirportsWidget::current_airport(){

    if(treeAirports->selectionModel()->selectedIndexes().size() == 0){
        return "";
    }
    QModelIndex index = proxyModel->index(treeAirports->selectionModel()->selectedIndexes().at(0).row(), CA_CODE);
    QStandardItem *item = model->itemFromIndex(proxyModel->mapToSource(index));
    return item->text();
}



QString AirportsWidget::validate(){

    //* Validate Coordinates TODO - other stuff
    /*if(mainObject->X->getx("start_postition") == "1"){
        if(txtLat->text().trimmed().length() == 0){
            txtLat->setFocus();
            return QString("Need Latitude");
        }
        if(txtLon->text().trimmed().length() == 0){
            txtLon->setFocus();
            return QString("Need Longtitude");
        }*/

    //* Validate Airport
    /*}else{
        if (!treeAirports->selectionModel()->hasSelection()){
            return QString(tr("Airport: No Airport selected or check [x] Use Default;"));
        }
    }*/


    return QString("");

}


//==========================================================================
/*void AirportsWidget::on_tab_changed(){
    emit setx("start_postition", true, QString::number(tabWidget->currentIndex()));
}*/


void AirportsWidget::on_coordinates_changed()
{
    //emit setx("--vor=", true, txtVOR->text());
    //emit setx("--ndb=", true, txtNDB->text());
    //emit setx("--fix=", true, txtFIX->text());
    //emit setx("--lat=", true, txtLat->text());
    //emit setx("--lon=", true, txtLon->text());
    //emit setx("--offset-distance=", true, txtOffset->text());
    //emit setx("--altitude=", true, txtAltitude->text());
    //emit setx("--heading=", true, txtHeading->text());

    /*mapWidget->show_aircraft(mainObject->X->getx("--callsign="),
                             mainObject->X->getx("--lat="),
                             mainObject->X->getx("--lon="),
                             mainObject->X->getx("--heading="),
                             "0"
                             //mainObject->X->getx("--altitude=") --> this we will have later
                             );*/
}


void AirportsWidget::on_upx(QString option, bool enabled, QString value)
{
    Q_UNUSED(enabled);
    Q_UNUSED(value);


    //== This is a nasty hack where we have to move the aircraft if a new lat/lon/heading
    QStringList items;
    items << "--lat=" << "--lon=" << "--heading=";

    if(items.contains(option)){

        XOpt lat = mainObject->X->get_opt("--lat=");
        if(lat.enabled){ // Lat enabled is used to hold "custom postion" and not a runway or parking
            mapWidget->show_aircraft(mainObject->X->getx("--callsign="),
                                     mainObject->X->getx("--lat="),
                                     mainObject->X->getx("--lon="),
                                     mainObject->X->getx("--heading="),
                                     "0"
                                     );
        }
    }


}


void AirportsWidget::on_airport_info_selection_changed()
{


    //= No Selection
    if(treeAptInfo->selectionModel()->hasSelection() == false){
        emit set_ena("--runway=", false);
        emit set_ena("--parking-id=", false);
        return;
    }

    QTreeWidgetItem *item = treeAptInfo->currentItem();

    //= Its a runway or stand
    if(item->text(CI_TYPE) == "runway" || item->text(CI_TYPE) == "stand"){

        if(item->text(CI_TYPE) == "runway"){
            emit setx("--runway=", true, item->text(CI_NODE));
            emit set_ena("--parking-id=", false);
            emit setx("--lat=", false, item->text(CI_LAT));
            emit setx("--lon=", false, item->text(CI_LON));
            emit setx("--heading=", false, item->text(CI_HEADING));
            mainObject->launcherWindow->on_upx("--runway=", true, item->text(CI_NODE)); // Show runway in header

        }else{ // its a stand
            emit set_ena("--runway=", false);
            emit setx("--parking-id=", true, item->text(CI_NODE));
            emit setx("--lat=", false, item->text(CI_LAT));
            emit setx("--lon=", false, item->text(CI_LON));
            emit setx("--heading=", false, item->text(CI_HEADING));
            mainObject->launcherWindow->on_upx("--parking-id=", true, item->text(CI_NODE)); // Show parking in header
        }
        mapWidget->show_aircraft(mainObject->X->getx("--callsign="),
                                 item->text(CI_LAT), item->text(CI_LON),
                                 item->text(CI_HEADING),
                                 "0");
    }

}

void AirportsWidget::on_airport_info_double_clicked(QTreeWidgetItem *item, int col_idx)
{
    Q_UNUSED(col_idx);
    QString typ = item->text(CI_TYPE);
    if (typ == "stand" || typ == "runway" || typ == "tower"){
        mapWidget->zoom_to_latlon(item->text(CI_LAT), item->text(CI_LON), 16);
    }
}


void AirportsWidget::on_open_airports_folder()
{
    if(labelAirportsFolder->text().length() == 0){
        return;
    }
    QUrl url( QString("file://%1").arg(labelAirportsFolder->text()) );
    QDesktopServices::openUrl(url);
}


void AirportsWidget::on_show_metar()
{
    metarWidget->show();
}

void AirportsWidget::on_debug_mode()
{
    treeAirports->setColumnHidden(CA_DIR, this->mainObject->debug_mode == false);
}

void AirportsWidget::on_view_airports_cache()
{
    FileViewerWidget *fileViewer = new FileViewerWidget();
    fileViewer->setFile(mainObject->data_file("airports.txt"));
    fileViewer->setWindowState(Qt::WindowMaximized);
    fileViewer->show();

}
