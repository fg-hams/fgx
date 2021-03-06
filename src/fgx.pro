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
	panes/renderingwidget.h \
	panes/coresettingswidget.h \
	xwidgets/xgroupboxes.h \
	xobjects/mainobject.h \
	launcher/launcherwindow.h \
	utilities/statusbar.h \
	xwidgets/xtreewidgetitem.h \
	utilities/utilities.h \
	utilities/messagebox.h \
	airports/importairportsdialog.h \
	airports/airportsdata.h \
	aircraft/aircraftdata.h \
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
	mpmap/xwebpage.h \
	panes/expertoptionswidget.h \
	xobjects/xsettingsmodel.h \
	xwidgets/fgxdebugwidget.h \
	launcher/headerwidget.h \
	xwidgets/xcommandpreviewidget.h \
	xobjects/xopt.h \
	xwidgets/xdebugtreewidget.h \
	map/openlayerwidget.h \
	aircraft/xaero.h \
	airports/metarwidget.h \ 
	utilities/fileDialog.h \
	app_config.h \
	utilities/dirDialog.h \
	launcher/menuwidget.h \
    utilities/workThread.h \
    utilities/loadAptDat.h \
    utilities/simgear/SGGeodesy.h \
    utilities/simgear/constants.h \
    utilities/simgear/compiler.h
SOURCES += main.cpp \
	xobjects/xsettings.cpp \
	settings/settingsdialog.cpp \
	network/networkwidget.cpp \
	network/mptelnet.cpp \
	aircraft/aircraftwidget.cpp \
	airports/airportswidget.cpp \
	xwidgets/execontrols.cpp \
	panes/timeweatherwidget.cpp \
	panes/renderingwidget.cpp \
	panes/coresettingswidget.cpp \
	xwidgets/xgroupboxes.cpp \
	xobjects/mainobject.cpp \
	launcher/launcherwindow.cpp \
	utilities/statusbar.cpp \
	utilities/utilities.cpp \
	utilities/messagebox.cpp \
	airports/importairportsdialog.cpp \
	airports/airportsdata.cpp \
	aircraft/aircraftdata.cpp \
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
	mpmap/xwebpage.cpp \
	panes/expertoptionswidget.cpp \
	xobjects/xsettingsmodel.cpp \
	xwidgets/fgxdebugwidget.cpp \
	launcher/headerwidget.cpp \
	xwidgets/xcommandpreviewidget.cpp \
	xobjects/xopt.cpp \
	xwidgets/xdebugtreewidget.cpp \
	map/openlayerwidget.cpp \
	aircraft/xaero.cpp \
	airports/metarwidget.cpp \ 
	utilities/fileDialog.cpp \
	utilities/dirDialog.cpp \
	launcher/menuwidget.cpp \
    utilities/workThread.cpp \
    utilities/loadAptDat.cpp \
    utilities/simgear/SGGeodesy.cpp
RESOURCES += resources/ycons.qrc \
	resources/artwork.qrc \
	resources/fonts.qrc \
	resources/openlayers.qrc \
	resources/default.qrc \
	resources/images.qrc
CONFIG += qt thread
