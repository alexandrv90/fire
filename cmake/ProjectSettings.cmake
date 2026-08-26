include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/Sanitizers.cmake")

function(fire_apply_project_settings targetName languageScope)
    if(NOT TARGET "${targetName}")
        message(FATAL_ERROR "Cannot configure unknown target: ${targetName}")
    endif()

    if(NOT languageScope MATCHES "^(PRIVATE|PUBLIC|INTERFACE)$")
        message(FATAL_ERROR "Invalid C++ language setting scope: ${languageScope}")
    endif()

    target_compile_features("${targetName}" ${languageScope} cxx_std_20)

    if(MSVC)
        target_compile_options("${targetName}" PRIVATE /W4 /permissive-)
    else()
        target_compile_options("${targetName}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    fire_enable_sanitizers("${targetName}")

    set_target_properties("${targetName}" PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    )
endfunction()
