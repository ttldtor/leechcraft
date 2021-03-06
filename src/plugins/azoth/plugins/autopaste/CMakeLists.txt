CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (leechcraft_azoth_autopaste)
INCLUDE (InitLCPlugin OPTIONAL)

SET (QT_USE_QTNETWORK TRUE)
INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (${AZOTH_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}/azoth
	)
SET (AUTOPASTE_SRCS
	autopaste.cpp
	bpasteservice.cpp
	codepadservice.cpp
	pastedialog.cpp
	pasteservicebase.cpp
	pasteservicefactory.cpp
	xmlsettingsmanager.cpp
	)
SET (AUTOPASTE_HEADERS
	autopaste.h
	bpasteservice.h
	codepadservice.h
	pastedialog.h
	pasteservicebase.h
	pasteservicefactory.h
	xmlsettingsmanager.h
	)
SET (AUTOPASTE_FORMS
	pastedialog.ui
	)
SET (AUTOPASTE_RESOURCES azothautopasteresources.qrc)
CreateTrs ("azoth_autopaste" "en;es;ru_RU;uk_UA" AUTOPASTE_COMPILED_TRANSLATIONS)
IF (NOT LC_NO_MOC)
	QT4_WRAP_CPP (AUTOPASTE_MOC_SRCS ${AUTOPASTE_HEADERS})
ENDIF (NOT LC_NO_MOC)
QT4_WRAP_UI (AUTOPASTE_UIS_H ${AUTOPASTE_FORMS})
QT4_ADD_RESOURCES (AUTOPASTE_RCCS ${AUTOPASTE_RESOURCES})

ADD_LIBRARY (leechcraft_azoth_autopaste SHARED
	${AUTOPASTE_COMPILED_TRANSLATIONS}
	${AUTOPASTE_SRCS}
	${AUTOPASTE_MOC_SRCS}
	${AUTOPASTE_RCCS}
	${AUTOPASTE_UIS_H}
	)
TARGET_LINK_LIBRARIES (leechcraft_azoth_autopaste
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	)
INSTALL (TARGETS leechcraft_azoth_autopaste DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES azothautopastesettings.xml DESTINATION ${LC_SETTINGS_DEST})
INSTALL (FILES ${AUTOPASTE_COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
