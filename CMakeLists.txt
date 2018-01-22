cmake_minimum_required(VERSION 2.6)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
MESSAGE(STATUS "lib output path:"${LIBRARY_OUTPUT_PATH})

SET(CMAKE_INSTALL_PREFIX /home/limingxing/project/branches/hooya/server/Util/install) 

SET(YY_TAF_PATH /usr/local/taf)
MESSAGE(STATUS "taf path:" ${YY_TAF_PATH})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
   ${YY_TAF_PATH}/include
  /usr/local/include/thrift
  /home/tafjce/Comm/Util/hiredis
)

set (HEADER_LIST 
	./include/DbPoolManager.h
	./include/JceUtil.h
	./include/LogUtil.h
	./include/RedisBase.h
	./include/TaskSchedule.h
	./include/TypeDefine.h
	./include/CommHashMap.h
	./include/CommTypeDefine.h
	./include/WordProcess.h
	./include/Word.h
	./include/Hz2Py.h
	./include/LockedSingleton.h
	./include/UrlUtil.h
	./include/MyFun.h
	)

file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#MESSAGE(STATUS "executable output path:"${EXECUTABLE_OUTPUT_PATH})


ADD_LIBRARY(hooyautil STATIC ${CURRENT_SRC})

target_link_libraries(hooyautil 
pthread
#/usr/local/lib/libthrift.a
#/usr/local/lib/libevent.a
#${YY_TAF_PATH}/lib/libtaf.a
#${YY_TAF_PATH}/lib/libhessian.a
#${YY_TAF_PATH}/lib/libparse.a
#${YY_TAF_PATH}/lib/libutil.a
/home/tafjce/Comm/Util/hiredis/libhiredis.a
)

INSTALL (TARGETS hooyautil DESTINATION /home/tafjce/HOOYA/HuyaUtil/)
FOREACH (HEADER_FILE ${HEADER_LIST})
 	STRING( REGEX REPLACE "\\.(.*)/.*\\.h$" "\\1" DIR_NAME ${HEADER_FILE} ) 
	INSTALL (FILES ${HEADER_FILE} DESTINATION  /home/tafjce/HOOYA/HuyaUtil/	)	
ENDFOREACH(HEADER_FILE)
