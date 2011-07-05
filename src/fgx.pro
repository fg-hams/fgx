# #####################################################################
# Automatically generated by qmake (2.01a) Do. Mrz 3 00:45:52 2011
# #####################################################################
TEMPLATE = app
TARGET = fgx
DEPENDPATH += .
INCLUDEPATH += .
QT += network \
    xml \
    xmlpatterns \
    webkit

# Input
HEADERS += xobjects/xsettings.h \
    settings/settingsdialog.h \
    network/networkwidget.h \
    network/mptelnet.h \
    aircraft/aircraftwidget.h \
    airports/airportswidget.h \
    xwidgets/execontrols.h \
    panes/timeweatherwidget.h \
    panes/advancedoptionswidget.h \
    panes/outputpreviewwidget.h \
    panes/coresettingswidget.h \
    xwidgets/xgroupboxes.h \
    xobjects/mainobject.h \
    launcher/launcherwindow.h \
    utilities/statusbar.h \
    xwidgets/xtreewidgetitem.h \
    utilities/utilities.h \
    utilities/messagebox.h \
    airports/importairportsdialog.h \
    setupwizard/fgrootpage.h \
    setupwizard/fgexepage.h \
    setupwizard/setupwizard.h \
    setupwizard/terrasyncpage.h \
    setupwizard/confirmpage.h \
    airports/airportsdata.h \
    aircraft/aircraftdata.h \
    setupwizard/completedpage.h \
    panes/viewlogwidget.h \
    utilities/zlib/inftrees.h \
    utilities/zlib/inflate.h \
    utilities/zlib/inffixed.h \
    utilities/zlib/inffast.h \
    utilities/zlib/gzguts.h \
    utilities/zlib/fgx_zutil.h \
    utilities/zlib/fgx_zlib.h \
    utilities/zlib/fgx_zconf.h \
    utilities/zlib/deflate.h \
    utilities/zlib/crc32.h \
    utilities/zlib/trees.h \
    utilities/fgx_gzlib.h \
    utilities/helpers.h \
    xobjects/xprocess.h \
    props/telnetslave.h \
    props/propstreewidget.h \
    logs/viewlogswidget.h \
    xobjects/latlng.h \
    props/propeditwidget.h \
    mpmap/mpmapwidget.h \
    mpmap/mpmapxwidget.h \
    mpmap/pilotswidget.h \
    xwidgets/xmessagelabel.h \
    mpmap/gmapwidget.h \
    mpmap/xwebpage.h
SOURCES += main.cpp \
    xobjects/xsettings.cpp \
    settings/settingsdialog.cpp \
    network/networkwidget.cpp \
    network/mptelnet.cpp \
    aircraft/aircraftwidget.cpp \
    airports/airportswidget.cpp \
    xwidgets/execontrols.cpp \
    panes/timeweatherwidget.cpp \
    panes/advancedoptionswidget.cpp \
    panes/outputpreviewwidget.cpp \
    panes/coresettingswidget.cpp \
    xwidgets/xgroupboxes.cpp \
    xobjects/mainobject.cpp \
    launcher/launcherwindow.cpp \
    utilities/statusbar.cpp \
    utilities/utilities.cpp \
    utilities/messagebox.cpp \
    airports/importairportsdialog.cpp \
    setupwizard/setupwizard.cpp \
    setupwizard/fgrootpage.cpp \
    setupwizard/fgexepage.cpp \
    setupwizard/terrasyncpage.cpp \
    setupwizard/confirmpage.cpp \
    airports/airportsdata.cpp \
    aircraft/aircraftdata.cpp \
    setupwizard/completedpage.cpp \
    panes/viewlogwidget.cpp \
    utilities/zlib/uncompr.c \
    utilities/zlib/trees.c \
    utilities/zlib/inftrees.c \
    utilities/zlib/inflate.c \
    utilities/zlib/inffast.c \
    utilities/zlib/infback.c \
    utilities/zlib/gzwrite.c \
    utilities/zlib/gzread.c \
    utilities/zlib/gzlib.c \
    utilities/zlib/gzclose.c \
    utilities/zlib/deflate.c \
    utilities/zlib/crc32.c \
    utilities/zlib/compress.c \
    utilities/zlib/adler32.c \
    utilities/zlib/zutil.c \
    utilities/fgx_gzlib.cpp \
    utilities/helpers.cpp \
    xobjects/xprocess.cpp \
    props/telnetslave.cpp \
    props/propstreewidget.cpp \
    logs/viewlogswidget.cpp \
    xobjects/latlng.cpp \
    props/propeditwidget.cpp \
    mpmap/mpmapwidget.cpp \
    mpmap/mpmapxwidget.cpp \
    mpmap/pilotswidget.cpp \
    xwidgets/xmessagelabel.cpp \
    mpmap/gmapwidget.cpp \
    mpmap/xwebpage.cpp
RESOURCES += resources/icons.qrc \
    resources/artwork.qrc \
	resources/fonts.qrc \
    resources/google_map.qrc
CONFIG += console
