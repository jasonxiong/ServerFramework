file(GLOB SRC_FILES *.hpp *.h *.cpp *.cxx *.c )

add_library(${TDRCONFIG_LIB} STATIC ${SRC_FILES})
