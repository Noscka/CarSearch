cmake_minimum_required(VERSION 3.8)

project ("CarSearch" VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)
add_subdirectory(External)

qt_standard_project_setup()

file(GLOB_RECURSE ProjectFiles CONFIGURE_DEPENDS "CustomWidgets/*" "Headers/*" "MainWindow/*")

qt_add_executable(${PROJECT_NAME} main.cpp ${ProjectFiles})

target_link_libraries(${PROJECT_NAME} PRIVATE NosLib htmlparser nlohmann_json::nlohmann_json Qt6::Widgets Qt6::Core Qt6::Gui)
target_include_directories(${PROJECT_NAME} PRIVATE htmlparser Qt6::Widgets Qt6::Core Qt6::Gui)

add_compile_definitions(CPPHTTPLIB_OPENSSL_SUPPORT)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    WIN32_EXECUTABLE ON
#    MACOSX_BUNDLE ON
#)


set(OUTDIR ${PROJECT_SOURCE_DIR}/Out/)

message(STATUS "POST BUILD OUT PATH: ${OUTDIR}")

add_custom_command(
   TARGET ${PROJECT_NAME} POST_BUILD 
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTDIR}
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${OUTDIR}
)

#if (CMAKE_IMPORT_LIBRARY_SUFFIX)
#  add_custom_command(
#    TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy -t ${OUTDIR} "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
#  COMMAND_EXPAND_LISTS
#  )
#
#  add_custom_command(
#    TARGET ${PROJECT_NAME} POST_BUILD
#    COMMAND "windeployqt" "${OUTDIR}/CarSearch.exe"
#  )
#endif ()