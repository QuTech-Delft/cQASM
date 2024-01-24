cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(func-gen C CXX)

# If func-gen was already included elsewhere in the project, don't include it again
# There should be only one place for it and one version per project
if(NOT TARGET ${PROJECT_NAME})

# Loads up the appropriate directories for installing stuff
include(GNUInstallDirs)

# Include the generate_tree functions
include(cmake/generate_funcs.cmake)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_std_23
)

# fPIC: otherwise some weirdness happens with pthreads or something when linking statically.
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
        -fPIC
        -Wno-error=deprecated-declarations
        -Wno-error=restrict
        -Wno-error=sign-compare
    )
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Werror -Wfatal-errors
        -fPIC
        -Wno-error=sign-compare
        -Wno-error=unused-private-field
        -Wno-error=unused-but-set-variable
    )
elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /WX
        /D_CRT_NONSTDC_NO_DEPRECATE
        /D_CRT_SECURE_NO_WARNINGS
        /D_UNICODE /DUNICODE
        /diagnostics:column /EHsc /FC /fp:precise /Gd /GS /MP /sdl /utf-8 /Zc:inline
    )
else()
    message(SEND_ERROR "Unknown compiler!")
endif()

# Address sanitizer
if(ASAN_ENABLED)
    if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        string(REGEX REPLACE "/RTC(su|[1su])" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}\n")

        # /MTd is needed to link clang_rt.asan-i386.lib statically
        # Otherwise the path to clang_rt.asan-i386.dll should be provided
        add_compile_options(/fsanitize=address /MTd)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

#=============================================================================#
# Installation                                                                #
#=============================================================================#

# Install the generator tool only if this is the top level project
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
    )
endif()

endif() # NOT TARGET ${PROJECT_NAME}
