IF (NOT USE_QT5)
	SET (QT_USE_QTDECLARATIVE TRUE)
	SET (QT_USE_QTNETWORK TRUE)
	SET (QT_USE_QTSQL TRUE)
	SET (QT_USE_QTXML TRUE)
	INCLUDE (${QT_USE_FILE})
ENDIF ()
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	..)

SET (SRCS
	gui/autoresizemixin.cpp
	gui/clearlineeditaddon.cpp
	gui/findnotification.cpp
	gui/flowlayout.cpp
	gui/pagenotification.cpp
	gui/selectablebrowser.cpp
	gui/unhoverdeletemixin.cpp
	gui/util.cpp
	models/flattenfiltermodel.cpp
	models/flattofoldersproxymodel.cpp
	models/listmodel.cpp
	models/mergemodel.cpp
	models/treeitem.cpp
	qml/colorthemeproxy.cpp
	qml/widthiconprovider.cpp
	qml/themeimageprovider.cpp
	qml/unhidelistmodel.cpp
	qml/unhidelistviewbase.cpp
	sync/syncdeltamanager.cpp
	sync/syncops.cpp
	sys/paths.cpp
	tags/categoryselector.cpp
	tags/tagscompleter.cpp
	tags/tagscompletionmodel.cpp
	tags/tagslineedit.cpp
	tags/tagsfiltermodel.cpp
	xpc/stddatafiltermenucreator.cpp
	xpc/util.cpp
	util.cpp
	dblock.cpp
	customcookiejar.cpp
	backendselector.cpp
	structuresops.cpp
	defaulthookproxy.cpp
	basehookinterconnector.cpp
	fileremoveguard.cpp
	exceptions.cpp
	customnetworkreply.cpp
	resourceloader.cpp
	notificationactionhandler.cpp
	socketerrorstrings.cpp
	sysinfo.cpp
	passutils.cpp
	queuemanager.cpp
	shortcuts/shortcutmanager.cpp
	)
SET (FORMS
	backendselector.ui
	gui/findnotification.ui
	)
IF (USE_QT5)
	qt5_wrap_ui (UIS_H ${FORMS})
ELSE ()
	QT4_WRAP_UI (UIS_H ${FORMS})
ENDIF ()

FOREACH (SRC ${SRCS})
	SET (UTIL_SRCS "${UTIL_SRCS};util/${SRC}")
ENDFOREACH (SRC)
FOREACH (FORM ${FORMS})
	SET (UTIL_FORMS "${UTIL_FORMS};util/${FORM}")
ENDFOREACH (FORM)
SET (UTIL_SRCS ${UTIL_SRCS} PARENT_SCOPE)
SET (UTIL_FORMS ${UTIL_FORMS} PARENT_SCOPE)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_GUI_LIB)

ADD_LIBRARY (lcutil SHARED
	${SRCS}
	${UIS_H}
	)
TARGET_LINK_LIBRARIES (lcutil
	${QT_LIBRARIES}
	xmlsettingsdialog
	)
SET_PROPERTY (TARGET lcutil PROPERTY SOVERSION 0.5.0)
INSTALL (TARGETS lcutil DESTINATION ${LIBDIR})

IF (USE_QT5)
	QT5_USE_MODULES (lcutil Declarative Network Xml Sql Widgets)
ENDIF ()
