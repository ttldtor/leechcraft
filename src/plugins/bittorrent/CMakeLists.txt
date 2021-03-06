CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (leechcraft_bittorrent)
INCLUDE (InitLCPlugin OPTIONAL)

FIND_PACKAGE (Boost REQUIRED COMPONENTS date_time filesystem system thread)

SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

FIND_PACKAGE (RBTorrent)
IF (NOT RBTorrent_FOUND)
	MESSAGE (ERROR "Rasterbar libtorrent not found, not building BitTorrent")
ENDIF (NOT RBTorrent_FOUND)

SET (QT_USE_QTXML TRUE)
INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIR}
	${RBTorrent_INCLUDE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	)

IF (WIN32)
	INCLUDE_DIRECTORIES (${RBTorrent_INCLUDE_DIR}/../zlib)
ENDIF (WIN32)

SET (SRCS
	torrentplugin.cpp
	tabwidget.cpp
	core.cpp
	addtorrent.cpp
	newtorrentwizard.cpp
	intropage.cpp
	firststep.cpp
	secondstep.cpp
	thirdstep.cpp
	addmultipletorrents.cpp
	pieceswidget.cpp
	xmlsettingsmanager.cpp
	piecesmodel.cpp
	torrentfilesmodel.cpp
	peersmodel.cpp
	filesviewdelegate.cpp
	movetorrentfiles.cpp
	trackerschanger.cpp
	peerstablinker.cpp
	addpeerdialog.cpp
	ipvalidators.cpp
	addwebseeddialog.cpp
	wizardgenerator.cpp
	startupfirstpage.cpp
	fastspeedcontrolwidget.cpp
	banpeersdialog.cpp
	ipfilterdialog.cpp
	livestreammanager.cpp
	livestreamdevice.cpp
	speedselectoraction.cpp
	torrentmaker.cpp
	singletrackerchanger.cpp
	torrenttab.cpp
	torrenttabwidget.cpp
	tabviewproxymodel.cpp
	notifymanager.cpp
	)
SET (HEADERS
	torrentplugin.h
	tabwidget.h
	core.h
	addtorrent.h
	newtorrentwizard.h
	intropage.h
	firststep.h
	secondstep.h
	thirdstep.h
	addmultipletorrents.h
	pieceswidget.h
	xmlsettingsmanager.h
	piecesmodel.h
	torrentfilesmodel.h
	peersmodel.h
	filesviewdelegate.h
	movetorrentfiles.h
	trackerschanger.h
	peerstablinker.h
	addpeerdialog.h
	ipvalidators.h
	addwebseeddialog.h
	wizardgenerator.h
	startupfirstpage.h
	fastspeedcontrolwidget.h
	banpeersdialog.h
	ipfilterdialog.h
	livestreammanager.h
	livestreamdevice.h
	speedselectoraction.h
	torrentmaker.h
	singletrackerchanger.h
	torrenttab.h
	torrenttabwidget.h
	tabviewproxymodel.h
	notifymanager.h
	newtorrentparams.h
	torrentinfo.h
	fileinfo.h
	peerinfo.h
	)
SET (FORMS
    tabwidget.ui
	addtorrent.ui
	addmultipletorrents.ui
	newtorrentfirststep.ui
	newtorrentsecondstep.ui
	newtorrentthirdstep.ui
	movetorrentfiles.ui
	trackerschanger.ui
	addpeerdialog.ui
	addwebseeddialog.ui
	startupfirstpage.ui
	fastspeedcontrolwidget.ui
	banpeersdialog.ui
	ipfilterdialog.ui
	singletrackerchanger.ui
	torrenttab.ui
	torrenttabwidget.ui
	)
SET (RESOURCES bittorrentresources.qrc)
CreateTrs ("bittorrent" "ar;be;de;en;eo;es;fr;it;oc;pl;ru_RU;uk_UA" COMPILED_TRANSLATIONS)
CreateTrsUpTarget ("bittorrent" "en;ru_RU" "${SRCS}" "${FORMS}" "torrentsettings.xml")

IF (APPLE)
	ADD_DEFINITIONS (-DWITH_SHIPPED_GEOIP_H)
ENDIF (APPLE)

OPTION (ENABLE_BITTORRENT_GEOIP "Enable support for GeoIP in BitTorrent (requires building libtorrent with GeoIP support)" OFF)

IF (ENABLE_BITTORRENT_GEOIP)
	ADD_DEFINITIONS (-DENABLE_GEOIP)
ELSEIF (ENABLE_BITTORRENT_GEOIP)
	IF (WIN32)
		ADD_DEFINITIONS (-DTORRENT_DISABLE_GEO_IP)
	ENDIF (WIN32)
ENDIF (ENABLE_BITTORRENT_GEOIP)

IF (UNIX)
	FIND_PROGRAM (LPKGC_CMD pkg-config)
	EXECUTE_PROCESS (COMMAND "${LPKGC_CMD}" libtorrent-rasterbar --cflags
		OUTPUT_VARIABLE RB_LT_CFLAGS
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	EXECUTE_PROCESS (COMMAND "${LPKGC_CMD}" libtorrent-rasterbar --modversion
		OUTPUT_VARIABLE RB_LT_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	MESSAGE (STATUS "BitTorrent: fetched flags for version ${RB_LT_VERSION}: ${RB_LT_CFLAGS}")
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RB_LT_CFLAGS}")

	IF (RB_LT_VERSION VERSION_GREATER 0.15.9999)
		ADD_DEFINITIONS (-DBOOST_ASIO_DYN_LINK)
	ENDIF (RB_LT_VERSION VERSION_GREATER 0.15.9999)
ENDIF (UNIX)

IF (WIN32)
	ADD_DEFINITIONS (-DWIN32)
	ADD_DEFINITIONS (-D_WIN32_WINNT=0x0502)
	ADD_DEFINITIONS (-DWIN32_LEAN_AND_MEAN)
	ADD_DEFINITIONS (-DBOOST_ASIO_DYN_LINK)
	ADD_DEFINITIONS (-DBOOST_ALL_NO_LIB)
ENDIF (WIN32)
#ADD_DEFINITIONS(-DBOOST_DYN_LINK)
ADD_DEFINITIONS(-DTORRENT_LINKING_SHARED)
#ADD_DEFINITIONS(-D_DLL)
IF (NOT LC_NO_MOC)
	QT4_WRAP_CPP (MOC_SRCS ${HEADERS})
ENDIF (NOT LC_NO_MOC)
QT4_WRAP_UI (UIS_H ${FORMS})
QT4_ADD_RESOURCES (RCCS ${RESOURCES})

ADD_LIBRARY (leechcraft_bittorrent SHARED
	${COMPILED_TRANSLATIONS}
	${SRCS}
	${MOC_SRCS}
	${UIS_H}
	${RCCS}
	)

IF (APPLE)
	SET (CRYPTOLIB "crypto")
ENDIF (APPLE)
TARGET_LINK_LIBRARIES (leechcraft_bittorrent
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${QT_LIBRARIES}
	${RBTorrent_LIBRARY}
	${LEECHCRAFT_LIBRARIES}
	${CRYPTOLIB}
)
INSTALL (TARGETS leechcraft_bittorrent DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES ${COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
INSTALL (FILES torrentsettings.xml DESTINATION ${LC_SETTINGS_DEST})
IF (UNIX AND NOT APPLE)
	INSTALL (FILES freedesktop/leechcraft-bittorrent.desktop DESTINATION share/applications)
ENDIF (UNIX AND NOT APPLE)
