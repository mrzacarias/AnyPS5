add_library(libSceGnmDriver SHARED EXCLUDE_FROM_ALL ${CMAKE_CURRENT_SOURCE_DIR}/Export.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/InitState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Flip.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Submit.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Markers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Draws.cpp)
target_include_directories(libSceGnmDriver PRIVATE ${LIBS_INCLUDE_DIR})
target_link_libraries(libSceGnmDriver PRIVATE libSceAgcDriver libc libkernel)
configure_windows_unwind(libSceGnmDriver)
