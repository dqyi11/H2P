set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(h2p-harrts-demo
               birrtstar_config.h
               birrtstar_config.cpp
               birrtstar_window.h
               birrtstar_window.cpp
               birrtstar_demo.cpp
               )

include_directories(
                   )

target_link_libraries(h2p-harrts-demo
                      h2p-harrts-viz
                      h2p-spatial-relations-window
                      ${QT_LIBRARIES}
                      ${OpenCV_LIBRARIES}
                     )
install(TARGETS h2p-harrts-demo DESTINATION bin)
