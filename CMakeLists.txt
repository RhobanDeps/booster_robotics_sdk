cmake_policy(SET CMP0048 NEW)
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

cmake_minimum_required(VERSION 3.15...3.27)

option(BUILD_PYTHON_BINDING "Build Python Binding" off)


set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

# Importing libraries. booster_robotics_sdk is the entry point you can link against, other
# libraries and include will be added as dependencies
add_library(booster_robotics_sdk STATIC IMPORTED)
set_target_properties(booster_robotics_sdk PROPERTIES
  IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}/libbooster_robotics_sdk.a
)

add_library(libfastcdr SHARED IMPORTED)
set_target_properties(libfastcdr PROPERTIES
  IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}/libfastcdr.so
)

add_library(libfastrtps SHARED IMPORTED)
set_target_properties(libfastrtps PROPERTIES
  IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}/libfastrtps.so
)

add_library(libfoonathan_memory-0.7.3 STATIC IMPORTED)
set_target_properties(libfoonathan_memory-0.7.3 PROPERTIES
  IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}/libfoonathan_memory-0.7.3.a
)

target_link_libraries(booster_robotics_sdk INTERFACE libfastcdr libfastrtps libfoonathan_memory-0.7.3)

target_include_directories(booster_robotics_sdk PUBLIC INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/third_party/include
)

add_executable(b1_loco_example_client example/high_level/b1_loco_example_client.cpp)
target_link_libraries(b1_loco_example_client booster_robotics_sdk)

# add_executable(b1_arm_sdk_example_client example/high_level/b1_arm_sdk_example.cpp)
# add_executable(b1_7dof_arm_sdk_example_client example/high_level/b1_7dof_arm_sdk_example.cpp)
# add_executable(b1_low_level_publisher example/low_level/low_level_publisher.cpp)
# add_executable(b1_low_level_subscriber example/low_level/low_level_subscriber.cpp)
# add_executable(low_level_hand_data_subscriber example/low_level/low_level_hand_data_subscriber.cpp)
# add_executable(b1_low_sdk_example example/low_level/b1_low_sdk_example.cpp)
# add_executable(b1_7dof_arm_low_sdk_example example/low_level/b1_7dof_arm_low_sdk_example.cpp)
# add_executable(odometer_example example/low_level/odometer_example.cpp)
# include_directories(BEFORE $(PROJECT_SOURCE_DIR)/include)

if(BUILD_PYTHON_BINDING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
    message(STATUS "Python3 executable: ${Python3_EXECUTABLE}")

    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()['purelib'])"
        OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    include_directories(${Python3_INCLUDE_DIRS})
    find_package(pybind11 CONFIG REQUIRED)

    python3_add_library(booster_robotics_sdk_python MODULE python/binding.cpp WITH_SOABI)

    target_compile_definitions(booster_robotics_sdk_python PRIVATE VERSION_INFO=${PROJECT_VERSION})

    find_program(PYBIND11_STUBGEN_EXECUTABLE pybind11-stubgen)
    if(NOT PYBIND11_STUBGEN_EXECUTABLE)
        message(FATAL_ERROR "pybind11-stubgen not found")
    endif()

    add_custom_command(
        TARGET booster_robotics_sdk_python
        POST_BUILD
        COMMAND PYTHONPATH=${CMAKE_SOURCE_DIR}/build:/${PYTHONPATH} pybind11-stubgen -o ${CMAKE_SOURCE_DIR}/build booster_robotics_sdk_python
    )

    install(TARGETS booster_robotics_sdk_python LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES})
    install(FILES ${CMAKE_SOURCE_DIR}/build/booster_robotics_sdk_python.pyi DESTINATION ${PYTHON_SITE_PACKAGES})    
endif()

