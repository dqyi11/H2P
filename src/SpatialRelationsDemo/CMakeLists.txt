set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(h2p-spatial-relations-window SHARED
               spatial_relations_window.h
               spatial_relations_window.cc
               spatial_relations_config.h
               spatial_relations_config.cc
            )

target_link_libraries(h2p-spatial-relations-window
                      h2p-world-decomp
                      h2p-spatial-relations
                      h2p-spatial-relations-viz
                      ${QT_LIBRARIES}
                      ${OpenCV_LIBRARIES}
                     )

add_executable(h2p-spatial-relations-demo
               spatial_relations_demo.cc) 

include_directories(
                   )

target_link_libraries(h2p-spatial-relations-demo 
                      h2p-spatial-relations-window
                     ) 

install(TARGETS h2p-spatial-relations-demo DESTINATION bin)
