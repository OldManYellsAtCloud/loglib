cmake_minimum_required(VERSION 3.16)

project(loglib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(log REQUIRED)

add_library(loglib SHARED include/loglib.h
    src/loglib.cpp)

target_include_directories(loglib PUBLIC "include")

set_target_properties(loglib PROPERTIES VERSION "1.0")
set_target_properties(loglib PROPERTIES SOVERSION "1")
set_target_properties(loglib PROPERTIES PUBLIC_HEADER include/loglib.h)
set_target_properties(loglib PROPERTIES LINKER_LANGUAGE CXX)


include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "loglibConfig.cmake"
        VERSION 1.0
        COMPATIBILITY AnyNewerVersion)

include(GNUInstallDirs)
install(TARGETS loglib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/loglib
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/loglibConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/loglib)
