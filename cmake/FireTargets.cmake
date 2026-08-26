include_guard(GLOBAL)

include("${CMAKE_CURRENT_LIST_DIR}/ProjectSettings.cmake")

function(fire_add_library targetName)
    set(options AUTOMOC STATIC)
    set(multiValueArgs SOURCES PUBLIC_DEPENDENCIES PRIVATE_DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 1 fireLibrary "${options}" "" "${multiValueArgs}")

    if(fireLibrary_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireLibrary_UNPARSED_ARGUMENTS}"
        )
    endif()

    if(NOT fireLibrary_SOURCES)
        message(FATAL_ERROR "No sources specified for library ${targetName}")
    endif()

    if(fireLibrary_STATIC)
        add_library("${targetName}" STATIC ${fireLibrary_SOURCES})
    else()
        add_library("${targetName}" ${fireLibrary_SOURCES})
    endif()
    target_include_directories("${targetName}" PUBLIC "${PROJECT_SOURCE_DIR}/src")

    if(fireLibrary_PUBLIC_DEPENDENCIES)
        target_link_libraries("${targetName}" PUBLIC ${fireLibrary_PUBLIC_DEPENDENCIES})
    endif()
    if(fireLibrary_PRIVATE_DEPENDENCIES)
        target_link_libraries("${targetName}" PRIVATE ${fireLibrary_PRIVATE_DEPENDENCIES})
    endif()
    if(fireLibrary_AUTOMOC)
        set_target_properties("${targetName}" PROPERTIES AUTOMOC TRUE)
    endif()

    fire_apply_project_settings("${targetName}" PUBLIC)
endfunction()

function(fire_add_executable targetName)
    set(options AUTOMOC INCLUDE_SOURCE_ROOT)
    set(multiValueArgs SOURCES DEPENDENCIES)
    cmake_parse_arguments(PARSE_ARGV 1 fireExecutable "${options}" "" "${multiValueArgs}")

    if(fireExecutable_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireExecutable_UNPARSED_ARGUMENTS}"
        )
    endif()

    if(NOT fireExecutable_SOURCES)
        message(FATAL_ERROR "No sources specified for executable ${targetName}")
    endif()

    add_executable("${targetName}" ${fireExecutable_SOURCES})

    if(fireExecutable_INCLUDE_SOURCE_ROOT)
        target_include_directories("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/src")
    endif()
    if(fireExecutable_DEPENDENCIES)
        target_link_libraries("${targetName}" PRIVATE ${fireExecutable_DEPENDENCIES})
    endif()
    if(fireExecutable_AUTOMOC)
        set_target_properties("${targetName}" PROPERTIES AUTOMOC TRUE)
    endif()

    fire_apply_project_settings("${targetName}" PRIVATE)
endfunction()

function(fire_add_test targetName)
    if(NOT BUILD_TESTING)
        return()
    endif()

    set(options AUTOMOC INCLUDE_SOURCE_ROOT)
    set(multiValueArgs SOURCES DEPENDENCIES ENVIRONMENT)
    cmake_parse_arguments(PARSE_ARGV 1 fireTest "${options}" "" "${multiValueArgs}")

    if(fireTest_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Unsupported arguments for ${targetName}: ${fireTest_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(executableArguments
        SOURCES ${fireTest_SOURCES}
        DEPENDENCIES ${fireTest_DEPENDENCIES}
    )
    if(fireTest_AUTOMOC)
        list(APPEND executableArguments AUTOMOC)
    endif()
    if(fireTest_INCLUDE_SOURCE_ROOT)
        list(APPEND executableArguments INCLUDE_SOURCE_ROOT)
    endif()

    fire_add_executable("${targetName}" ${executableArguments})
    target_sources("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/tests/tests_common.h")
    target_include_directories("${targetName}" PRIVATE "${PROJECT_SOURCE_DIR}/tests")
    add_test(NAME "${targetName}" COMMAND "${targetName}")
    set_tests_properties("${targetName}" PROPERTIES WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

    if(fireTest_ENVIRONMENT)
        set_tests_properties("${targetName}" PROPERTIES ENVIRONMENT "${fireTest_ENVIRONMENT}")
    endif()
endfunction()
