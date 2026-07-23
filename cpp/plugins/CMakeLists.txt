# Plugins CMakeLists.txt

add_definitions(-DTVP_COMPILING_KRKR2_NEXT)

set(CUBISM_FW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Framework)
set(CUBISM_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cubism/Core)

# Find all sources in Framework/src recursively
file(GLOB_RECURSE CUBISM_FW_SOURCES
    ${CUBISM_FW_DIR}/src/*.cpp
    ${CUBISM_FW_DIR}/src/*.c
)

# Exclude non-OpenGL renderers (already removed from tree, but safety net)
if(CUBISM_FW_SOURCES)
    list(FILTER CUBISM_FW_SOURCES EXCLUDE REGEX ".*/D3D9/.*|.*/D3D11/.*|.*/Vulkan/.*|.*/Metal/.*|.*/Cocos2d/.*")
endif()

if(NOT CUBISM_FW_SOURCES)
    set(DUMMY_CUBISM_SRC ${CMAKE_CURRENT_BINARY_DIR}/cubism_dummy.cpp)
    if(NOT EXISTS ${DUMMY_CUBISM_SRC})
        file(WRITE ${DUMMY_CUBISM_SRC} "/* Dummy CubismFramework source when Framework files are missing */\n")
    endif()
    set(CUBISM_FW_SOURCES ${DUMMY_CUBISM_SRC})
endif()

add_library(CubismFramework STATIC ${CUBISM_FW_SOURCES})
target_include_directories(CubismFramework PUBLIC
    ${CUBISM_FW_DIR}
    ${CUBISM_FW_DIR}/src
    ${CUBISM_CORE_DIR}/include
)

# Ensure vcpkg include directory is ALWAYS in CubismFramework's PUBLIC include paths
if(VCPKG_INSTALLED_DIR AND VCPKG_TARGET_TRIPLET)
    target_include_directories(CubismFramework PUBLIC
        ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include
    )
endif()

target_compile_definitions(CubismFramework PUBLIC CSM_TARGET_ANGLE_ES2)

# ANGLE headers for CubismFramework's OpenGLES2 renderer
find_package(unofficial-angle CONFIG QUIET)
if(unofficial-angle_FOUND)
    target_link_libraries(CubismFramework PRIVATE
        unofficial::angle::libEGL
        unofficial::angle::libGLESv2
    )
    get_target_property(_EGL_INCS unofficial::angle::libEGL INTERFACE_INCLUDE_DIRECTORIES)
    if(_EGL_INCS)
        target_include_directories(CubismFramework PRIVATE ${_EGL_INCS})
    endif()
elseif(ANDROID)
    target_link_libraries(CubismFramework PRIVATE EGL GLESv2)
endif()

if(NOT CUBISM_FW_SOURCES MATCHES "cubism_dummy.cpp")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
        target_compile_options(CubismFramework PRIVATE -include GLES2/gl2.h)
    elseif(MSVC)
        target_compile_options(CubismFramework PRIVATE /FI GLES2/gl2.h)
    endif()
    target_compile_definitions(CubismFramework PRIVATE GL_TEXTURE_MAX_ANISOTROPY_EXT=0x84FE)
endif()

if(ANDROID)
    set(CUBISM_CORE_ARCH ${CMAKE_ANDROID_ARCH_ABI})
    set(CUBISM_CORE_LIB ${CUBISM_CORE_DIR}/lib/android/${CUBISM_CORE_ARCH}/libLive2DCubismCore.a)
elseif(IOS)
    set(CUBISM_CORE_LIB ${CUBISM_CORE_DIR}/lib/ios/Release-iphoneos/libLive2DCubismCore.a)
elseif(APPLE)
    if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
        set(CUBISM_CORE_LIB ${CUBISM_CORE_DIR}/lib/macos/x86_64/libLive2DCubismCore.a)
    else()
        set(CUBISM_CORE_LIB ${CUBISM_CORE_DIR}/lib/macos/arm64/libLive2DCubismCore.a)
    endif()
endif()

if(EXISTS "${CUBISM_CORE_LIB}")
    target_link_libraries(CubismFramework PUBLIC ${CUBISM_CORE_LIB})
endif()

add_library(krkr2plugin STATIC
    krkrlive2d.cpp
    krkrgles.cpp
)

target_include_directories(krkr2plugin PUBLIC
    ${CUBISM_FW_DIR}
    ${CUBISM_FW_DIR}/src
    ${CUBISM_CORE_DIR}/include
    ${KRKR2PLUGIN_PATH}
    ${KRKR2PLUGIN_PATH}/simplebinder
)
target_compile_definitions(krkr2plugin PUBLIC CSM_TARGET_ANGLE_ES2)
target_link_libraries(krkr2plugin PUBLIC CubismFramework krkr2core)

if(TARGET ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME} PUBLIC krkr2plugin)
endif()

add_subdirectory(psdfile)
add_subdirectory(psbfile)
add_subdirectory(layerex_draw)
add_subdirectory(motionplayer)
add_subdirectory(kagparserex)
