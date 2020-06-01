cmake_minimum_required(VERSION 3.15)

find_package(Vulkan REQUIRED)

add_library(vps
STATIC
    include/vps.h
    src/vps.cpp
    src/Context.h
    src/Context.cpp
    src/config.h
    src/Set_pool.h
    src/Set_pool.cpp
    src/Technique.h
    src/Technique.cpp
    src/Image_threshold_to_zero.h
    src/Image_threshold_to_zero.cpp
)

target_include_directories(vps
PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    include
)

target_compile_definitions(vps
PUBLIC
    VPS_ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/asset"
)

target_link_libraries(vps
PUBLIC
    ${Vulkan_LIBRARIES}
    sc
)

set_target_properties(vps
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED O
    CXX_EXTENSIONS ON
)

if(BUILD_TESTING)
    add_executable(vps_cts
        test/main.cpp
        test/image_threshold_to_zero_cts.cpp
    )

    target_link_libraries(vps_cts
    PRIVATE
        vps
    )

    set_target_properties(vps_cts
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )

    enable_testing()
    add_test(vps_cts vps_cts)
endif()