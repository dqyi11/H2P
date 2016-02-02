
set(LIB h2p-world-decomp)

set(HDR  obstacle.h
         line_subsegment.h
         region.h
         reference_frames.h
         worldmap.h
         string_grammar.h
         cgal_util.h
         world_datatype.h
         img_load_util.h
         )

add_library(${LIB} SHARED
            obstacle.cc
            line_subsegment.cc
            region.cc
            reference_frames.cc
            worldmap.cc
            string_grammar.cc
           )

target_link_libraries(${LIB}
                      ${CGAL_LIBRARIES}
                      ${BOOST_LIBRARIES}
                      ${LIBXML2_LIBRARIES}
                     )

install(TARGETS ${LIB}
        LIBRARY
        DESTINATION lib
        ARCHIVE
        DESTINATION lib)

install(FILES ${HDR}
        DESTINATION include/)
