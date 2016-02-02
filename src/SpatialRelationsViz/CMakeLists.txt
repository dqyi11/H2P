
set(LIB h2p-spatial-relations-viz)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${LIB} SHARED
            spatial_relations_viz.h
            spatial_relations_viz.cc)

include_directories(
                   )

target_link_libraries(${LIB} 
                      h2p-world-decomp
                      h2p-spatial-relations
                      ${QT_LIBRARIES} 
                      ${OpenCV_LIBRARIES}
                     )

install(TARGETS ${LIB}
        LIBRARY
        DESTINATION lib
        ARCHIVE
        DESTINATION lib)

