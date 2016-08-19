add_library(ttc_c SHARED ttc_c.c ttc_c_util.c tensor_util.c)
target_link_libraries(ttc_c dl)
