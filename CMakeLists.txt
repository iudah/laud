cmake_minimum_required(VERSION 3.23)

project(laud)

add_library(laud-c_flags INTERFACE)
target_compile_features(laud-c_flags INTERFACE c_std_17)
target_compile_options(
  laud-c_flags
  INTERFACE
    "$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
    "$<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-fPIC;-ggdb3;-O0;-fno-omit-frame-pointer;-fsanitize=address>>"
)
target_link_options(
  laud-c_flags
  INTERFACE
  # "$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
  # "$<BUILD_INTERFACE:-Wall;-Wl,--exclude-libs>"
  "$<BUILD_INTERFACE:$<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-fPIC;-ggdb3;-O0;-fno-omit-frame-pointer;-fsanitize=address>>"
)

target_compile_definitions(
  laud-c_flags INTERFACE "$<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:IU_MEM_LK=1>")

add_executable(laud main.cpp)
add_subdirectory(c)
target_link_libraries(laud laud-c laud-c_flags)

install(TARGETS laud-c DESTINATION bin)
install(FILES laud_nn.h laud.h DESTINATION include/${PROJECT_NAME})
