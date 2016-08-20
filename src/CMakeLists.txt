# Add both shared and static libraries
add_library(ttc_c SHARED ttc_c.c ttc_c_util.c tensor_util.c)
add_library(ttc_c_static STATIC ttc_c.c ttc_c_util.c tensor_util.c)

target_link_libraries(ttc_c dl)
target_link_libraries(ttc_c_static dl)

set_target_properties(ttc_c_static PROPERTIES OUTPUT_NAME ttc_c)

# Configure installation
install(TARGETS ttc_c ttc_c_static
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
