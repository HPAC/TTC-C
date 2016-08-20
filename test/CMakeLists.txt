include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add customized make clean targets
set(TTC_DIR "ttc_transpositions")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "$(TTC_DIR)")

# Add executable
add_executable(jit-test jit-test.c test-util.c)
target_link_libraries(jit-test ttc_c)

add_executable(boundary-test boundary-test.c test-util.c)
target_link_libraries(boundary-test ttc_c)

add_executable(avx512-test avx512-test.c test-util.c)
target_link_libraries(avx512-test ttc_c)

add_executable(knc-test knc-test.c test-util.c)
target_link_libraries(knc-test ttc_c)


# Add customized make clean targets
set(TTC_GEN "ttc_transpositions")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "$(TTC_GEN)")

# Build CUDA tests if CUDA is found
find_package(CUDA)
if (CUDA_FOUND)
    add_executable(cuda-test cuda-test.c test-util.c)
    target_link_libraries(cuda-test ttc_c)
endif ()
