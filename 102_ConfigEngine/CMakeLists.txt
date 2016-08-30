file(GLOB SRC_FILES *.hpp *.cpp)

add_library(${CONFIG_LAS}  STATIC  ${SRC_FILES})
#add_dependencies(${CONFIG_LAS} ${PROTOCOL_NULL_OUT})
#target_link_libraries(${CONFIG_LAS} ${SERVERLIB_LAS} ${PROTO_LAS})


----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
