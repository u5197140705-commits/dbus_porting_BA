cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
enable_language(CXX)

include(CMakeTest.txt)
include(CMakeDefinitions.txt)

project(DBAL)

add_executable(MarshalingTest 
  ${CMAKE_CURRENT_SOURCE_DIR}/test_Marshaling.cpp
)

add_executable(InstaceTest 
  ${CMAKE_CURRENT_SOURCE_DIR}/test_Instances.cpp
)

add_executable(CrossQueueTest 
  ${CMAKE_CURRENT_SOURCE_DIR}/test_CrossQueue.cpp
)

add_executable(IoConnectTest 
  ${CMAKE_CURRENT_SOURCE_DIR}/../BshDBus2AppLayer_instances.c
  ${CMAKE_CURRENT_SOURCE_DIR}/dummys/dbus/DBal/DBal_cfg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/test_IoConnectionHandling.cpp
)

add_executable(ApplayerTest 
  ${CMAKE_CURRENT_SOURCE_DIR}/../BshDBus2AppLayer_instances.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../BshDBus2AppLayer_crossQueue.c
  ${CMAKE_CURRENT_SOURCE_DIR}/test_AppLayer.cpp
)

target_link_libraries(MarshalingTest
  gtest
  gtest_main
  gmock
)

target_link_libraries(InstaceTest
  gtest
  gtest_main
  gmock
)

target_link_libraries(IoConnectTest
  gtest
  gtest_main
  gmock
)

target_link_libraries(CrossQueueTest
  gtest
  gtest_main
  gmock
)

target_link_libraries(ApplayerTest
  gtest
  gtest_main
  gmock
)
