INCLUDE_DIRECTORIES (
	${LAURE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${Boost_INCLUDE_DIR}
	${LEECHCRAFT_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${LastFM_INCLUDE_DIR}
	${VLC_INCLUDE_DIR}
	)

IF (NOT LC_NO_MOC)
	QT4_WRAP_CPP (VLCWRAPPERTEST_MOC "vlcwrappertest.h")
	QT4_WRAP_CPP (VLCWRAPPER_MOC ${LAURE_SOURCE_DIR}/vlcwrapper.h)
ENDIF (NOT LC_NO_MOC)
ADD_EXECUTABLE (lc_laure_vlcwrappertest
	vlcwrappertest.cpp
	${LAURE_SOURCE_DIR}/vlcwrapper.cpp
	${VLCWRAPPERTEST_MOC}
	${MOC_RCCS}
	${VLCWRAPPER_MOC}
)
TARGET_LINK_LIBRARIES (lc_laure_vlcwrappertest
	${QT_LIBRARIES}
	${LEECHCRAFT_LIBRARIES}
	${VLC_LIBRARIES}
)

ADD_TEST (VLCWrapper lc_laure_vlcwrappertest)