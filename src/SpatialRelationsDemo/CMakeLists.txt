set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(h2p-spatial-relations-demo
               spatial_relations_window.h
               spatial_relations_window.cc
               spatial_relations_config.h
               spatial_relations_config.cc
               spatial_relations_demo.cc) 

include_directories(
                   )

target_link_libraries(h2p-spatial-relations-demo 
                      h2p-spatial-relations
                      h2p-spatial-relations-viz
                      ${QT_LIBRARIES}
                     ) 

install(TARGETS h2p-spatial-relations-demo DESTINATION bin)
