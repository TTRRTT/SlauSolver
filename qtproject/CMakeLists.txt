cmake_minimum_required(VERSION 3.19)
project(SlauSolver LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(SlauSolver
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    dbmanager.h dbmanager.cpp
    loginwindow.h loginwindow.cpp

)

set(MATRIX_LIB_PATH ${CMAKE_SOURCE_DIR}/../libs/matrix/s21_matrix.a)
add_custom_command(
    OUTPUT ${MATRIX_LIB_PATH}
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../libs/matrix
    COMMENT "Building matrix library..."
    VERBATIM
)
add_custom_target(BuildMatrixLib
    DEPENDS ${MATRIX_LIB_PATH}
)

target_include_directories(SlauSolver PRIVATE
    ${CMAKE_SOURCE_DIR}/../libs/matrix
)

target_link_libraries(SlauSolver
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
        ${MATRIX_LIB_PATH}
)

include(GNUInstallDirs)

install(TARGETS SlauSolver
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET SlauSolver
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
