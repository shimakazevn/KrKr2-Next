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

# Enable CRC32 hardware instructions for Apple Clang on iOS.
# All arm64 iOS devices (A7+, iOS 7+) support this.
# Without -march=armv8-a+crc, Apple Clang does not enable CRC32 intrinsics/inline-asm
# by default, causing build failures in 7zip (7zCrc.c) and zlib (crc32.c).
set(VCPKG_C_FLAGS "-march=armv8-a+crc")
set(VCPKG_CXX_FLAGS "-march=armv8-a+crc")
