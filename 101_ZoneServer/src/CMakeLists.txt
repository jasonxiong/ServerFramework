file(GLOB_RECURSE SRC_FILES Utils/* Kernel/* Modules/*)

set (CUR_INC_BASE ${CMAKE_CURRENT_SOURCE_DIR}/BaseLib)
cget_dir(CUR_INC_KERNEL  ${CMAKE_CURRENT_SOURCE_DIR}/Kernel)
cget_dir(CUR_INC_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/Modules)
cget_dir(CUR_INC_UTILS   ${CMAKE_CURRENT_SOURCE_DIR}/Utils)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_executable(${ZONE_SVR_EXE} ${SRC_FILES})
    include_directories(${CUR_INC_BASE} ${CUR_INC_KERNEL} ${CUR_INC_MODULES} ${CUR_INC_UTILS})
    target_link_libraries(${ZONE_SVR_EXE} ${ALL_LAS} ${PUGIXML_LIB})
else()
    add_executable(${ZONE_SVR_EXE} ${SRC_FILES})
    include_directories(${CUR_INC_BASE} ${CUR_INC_KERNEL} ${CUR_INC_MODULES} ${CUR_INC_UTILS})
    target_link_libraries(${ZONE_SVR_EXE} ${ALL_LAS} ${PUGIXML_LIB})
endif()

install(TARGETS ${ZONE_SVR_EXE}  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Make)


