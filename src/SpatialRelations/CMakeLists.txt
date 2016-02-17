
set(LIB h2p-spatial-relations)

set(HDR    spatial_relation_function.h
           inbetween_relation_function.h
           sideoof_relation_function.h
           avoid_relation_function.h
           spatial_relation_mgr.h
           spatial_relation_string_class_mgr.h
           rule.h
   )

add_library(${LIB} SHARED
            inbetween_relation_function.cc
            sideof_relation_function.cc
            avoid_relation_function.cc
            spatial_relation_mgr.cc
            spatial_relation_string_class_mgr.cc
            rule.cc
           )

target_link_libraries(${LIB}
                      h2p-world-decomp
                      ${CGAL_LIBRARIES}
                      ${BOOST_LIBRARIES}
                     ) 


install(TARGETS ${LIB}
        LIBRARY
        DESTINATION lib
        ARCHIVE
        DESTINATION lib)

install(FILES ${HDR}
        DESTINATION include/)
