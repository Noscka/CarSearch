cmake_minimum_required(VERSION 3.8)

project ("CarSearch" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME} MainWindow.ui MainWindow.cpp main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

set(OUTDIR ${PROJECT_SOURCE_DIR}/Out/)

message(STATUS "POST BUILD OUT PATH: ${OUTDIR}")

add_custom_command(
   TARGET ${PROJECT_NAME} POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTDIR}
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${OUTDIR}
)

if (CMAKE_IMPORT_LIBRARY_SUFFIX)
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t ${OUTDIR} "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
    COMMAND "windeployqt" "${OUTDIR}/CarSearch.exe"
  COMMAND_EXPAND_LISTS
  )
endif ()