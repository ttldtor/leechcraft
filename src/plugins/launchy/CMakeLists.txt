CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (leechcraft_launchy)
INCLUDE (InitLCPlugin OPTIONAL)

SET (QT_USE_QTDECLARATIVE TRUE)
INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	)
SET (SRCS
	launchy.cpp
	fsdisplayer.cpp
	item.cpp
	itemsfinder.cpp
	itemssortfilterproxymodel.cpp
	fdodesktopparser.cpp
	favoritesmanager.cpp
	quarkmanager.cpp
	itemimageprovider.cpp
	)
SET (RESOURCES resources.qrc)
QT4_ADD_RESOURCES (RCCS ${RESOURCES})
CreateTrs ("launchy" "en;ru_RU" COMPILED_TRANSLATIONS)

ADD_LIBRARY (leechcraft_launchy SHARED
	${COMPILED_TRANSLATIONS}
	${SRCS}
	${RCCS}
	)
TARGET_LINK_LIBRARIES (leechcraft_launchy
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	)
INSTALL (TARGETS leechcraft_launchy DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES ${COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
INSTALL (DIRECTORY share/qml/launchy DESTINATION ${LC_QML_DEST})
