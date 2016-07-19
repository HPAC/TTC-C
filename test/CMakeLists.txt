include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(jit-test jit-test.c)
target_link_libraries(jit-test tensor_tc dl)

# Add customized make clean targets
set(TTC_DIR "ttc_transpositions")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${TTC_DIR}")
