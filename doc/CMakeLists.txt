if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is need to build the documentation.")
endif()

set(_DOXYGEN_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/tensor_doc.config)
set(_DOXYGEN_IMG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/html/img)
set(_DOXYGEN_IMG_DEST ${CMAKE_CURRENT_BINARY_DIR}/html)

add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${_DOXYGEN_CONFIG}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with doxygen."
    VERBATIM)

file(COPY ${_DOXYGEN_IMG_SRC} DESTINATION ${_DOXYGEN_IMG_DEST})
