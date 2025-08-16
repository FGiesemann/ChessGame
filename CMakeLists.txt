cmake_minimum_required(VERSION 3.15)
project(ChessGame VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)
option(BUILD_TESTING "Build unittests" ON)

include(cmake/compiler_warnings.cmake)

find_package(chesscore REQUIRED COMPONENTS chesscore)

add_library(${PROJECT_NAME}
    src/cursor.cpp
    src/game.cpp
    src/metadata.cpp
    src/move_matcher.cpp
    src/pgn.cpp
    src/san.cpp
    src/tree.cpp
    src/types.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME} PUBLIC chesscore::chesscore)
add_compiler_warnings(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
install(DIRECTORY include/${PROJECT_NAME}/ DESTINATION include/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
set(${PROJECT_NAME}_VERSION ${PROJECT_VERSION})

include(CMakePackageConfigHelpers)
set(chess_game_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/chesscore")
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    PATH_VARS
        chess_game_INCLUDE_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/api")
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile 
            @ONLY
        )

    add_custom_command(
        OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.html
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMAND_EXPAND_LISTS
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )

    add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.html)

    install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/html DESTINATION share/doc/${PROJECT_NAME}
        PATTERN "*.map" EXCLUDE
        PATTERN "*.md5" EXCLUDE
    )
    endif()
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION share/doc/${PROJECT_NAME})

if (NOT BUILD_TESTING STREQUAL OFF)
    enable_testing()
    find_package(Catch2 3 REQUIRED)
    find_package(chesscore REQUIRED COMPONENTS chesscore chesscore_io)
    add_subdirectory(test)
endif()

# PGN Parser Test Program (DELME)
add_subdirectory(pgn_parser)
