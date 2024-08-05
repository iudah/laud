add_library(mem_lk SHARED IMPORTED)
add_library(backtrace STATIC IMPORTED)
add_library(Ubject SHARED IMPORTED)

set_property(TARGET mem_lk PROPERTY IMPORTED_LOCATION
                                    ${CMAKE_INSTALL_PREFIX}/bin/libmem_lk.so)
set_property(
  TARGET backtrace PROPERTY IMPORTED_LOCATION
                            ${CMAKE_INSTALL_PREFIX}/bin/libbacktrace.a)
set_property(TARGET Ubject PROPERTY IMPORTED_LOCATION
                                    ${CMAKE_INSTALL_PREFIX}/bin/libUbject.so)

add_library(laud-c SHARED)
include(sources.cmake)
add_dependencies(laud-c Ubject)

target_link_libraries(
  laud-c
  PUBLIC Ubject mem_lk
  PRIVATE laud-c_flags)

target_include_directories(laud-c
                           PRIVATE ${CMAKE_INSTALL_PREFIX}/include/mem_lk)
target_include_directories(laud-c
                           PRIVATE ${CMAKE_INSTALL_PREFIX}/include/Ubject)

include(headers.cmake)
