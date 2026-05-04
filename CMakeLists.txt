cmake_minimum_required(VERSION 3.28)
project(Orca LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG main
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG master
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(pugixml)



add_executable(
main src/main.cpp 
    "src/examples.cpp"
    "src/App.cpp"
    "src/Window.cpp"
    "src/Logger.cpp"
    "src/StringUtilities.cpp"
    "src/Layout.cpp"
    "src/LayoutParser.cpp"
    "src/LayoutObject.cpp"
    "src/Panel.cpp"
    "src/FileWatcher.cpp")

target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE SFML::Graphics)
target_link_libraries(main PRIVATE glaze::glaze)
target_link_libraries(main PRIVATE pugixml::pugixml)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
