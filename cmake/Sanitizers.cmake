include_guard(GLOBAL)

option(FIRE_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(FIRE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

function(fire_enable_sanitizers targetName)
    if(NOT TARGET "${targetName}")
        message(FATAL_ERROR "Cannot enable sanitizers for unknown target: ${targetName}")
    endif()

    if(NOT FIRE_ENABLE_ASAN AND NOT FIRE_ENABLE_UBSAN)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        if(FIRE_ENABLE_UBSAN)
            message(FATAL_ERROR "UndefinedBehaviorSanitizer is not supported by the MSVC compiler frontend")
        endif()

        target_compile_options("${targetName}" PRIVATE /fsanitize=address)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
        message(FATAL_ERROR
            "Sanitizers are not supported for C++ compiler ${CMAKE_CXX_COMPILER_ID}"
        )
    endif()

    set(enabledSanitizers)
    if(FIRE_ENABLE_ASAN)
        list(APPEND enabledSanitizers address)
    endif()
    if(FIRE_ENABLE_UBSAN)
        list(APPEND enabledSanitizers undefined)
    endif()
    list(JOIN enabledSanitizers "," sanitizerList)

    target_compile_options("${targetName}" PRIVATE
        "-fsanitize=${sanitizerList}"
        -fno-omit-frame-pointer
    )
    target_link_options("${targetName}" PRIVATE "-fsanitize=${sanitizerList}")

    if(FIRE_ENABLE_UBSAN)
        target_compile_options("${targetName}" PRIVATE -fno-sanitize-recover=undefined)
    endif()
endfunction()
