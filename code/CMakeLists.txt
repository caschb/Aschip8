find_package(SDL2 REQUIRED)


file(GLOB SOURCES "src/*.cpp")
add_executable(aschip8 ${SOURCES})

target_include_directories(aschip8 PRIVATE headers)
target_include_directories(aschip8 PRIVATE ${SDL2_INCLUDE_DIR})

target_link_libraries(aschip8 PRIVATE ${SDL2_LIBRARY})
