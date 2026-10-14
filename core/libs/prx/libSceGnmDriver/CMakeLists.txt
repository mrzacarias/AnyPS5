add_library(libSceGnmDriver SHARED EXCLUDE_FROM_ALL ${CMAKE_CURRENT_SOURCE_DIR}/Export.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/InitState.cpp)
target_include_directories(libSceGnmDriver PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(libSceGnmDriver PRIVATE libSceAgcDriver libc libkernel)
configure_windows_unwind(libSceGnmDriver)
