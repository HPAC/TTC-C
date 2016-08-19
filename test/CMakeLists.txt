include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jit-test jit-test.c)
target_link_libraries(jit-test ttc_c)

add_executable(boundary-test boundary-test.c)
target_link_libraries(boundary-test ttc_c)


# Add customized make clean targets
set(TTC_GEN "ttc_transpositions")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${TTC_GEN}")
