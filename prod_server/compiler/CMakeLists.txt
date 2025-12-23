cmake_minimum_required(VERSION 3.6)

project(cmpf CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json
)

FetchContent_makeavailable(nlohmann_json)

include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE HEADERS "src/*.hpp*")



add_executable(
  cmpf 
  src/main.cpp 
  src/setups.cpp
  src/compiler.cpp
  src/compiler_logging.cpp
  src/nodeworks.cpp
  src/pack_proc.cpp
  src/packs.cpp
  src/comp_seq.cpp
  src/pl_gen.cpp
  src/pl_block.cpp
)

target_link_libraries(cmpf PRIVATE nlohmann_json::nlohmann_json)