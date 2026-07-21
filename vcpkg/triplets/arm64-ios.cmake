set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME iOS)
set(VCPKG_OSX_DEPLOYMENT_TARGET 15.0)
set(VCPKG_OSX_ARCHITECTURES arm64)

# Fix autotools cross-compilation detection for iOS
# Without this, --host and --build are both aarch64-apple-darwin on Apple Silicon,
# causing configure to think it's not cross-compiling and try to run iOS binaries.
set(VCPKG_MAKE_BUILD_TRIPLET "--host=aarch64-apple-ios")

# Fix 7zip 7zCrc.c build failure on Apple Clang iOS:
# Apple Clang defines __ARM_ACLE which triggers inclusion of arm_acle.h,
# which uses __builtin_arm_crc32b requiring target feature 'crc'.
# Setting __ARM_ACLE=0 prevents this include so 7zip uses pure C CRC code.
set(VCPKG_C_FLAGS "-D__ARM_ACLE=0 -D__ARM_FEATURE_CRC32=0")
set(VCPKG_CXX_FLAGS "-D__ARM_ACLE=0 -D__ARM_FEATURE_CRC32=0")
