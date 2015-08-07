FILE(GLOB SRC_FILES *.cpp *.hpp)

link_directories(/usr/lib64/mysql/)

add_executable(BaseLibTest ${SRC_FILES})

target_link_libraries(BaseLibTest ${GTESTLIB_LIB} ${ALL_LAS}
	${MYSQLCLIENT_LIB} ${BOTAN_LIB})

add_test(BaseLibTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BaseLibTest)
