set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(TOOLCHAIN_PREFIX x86_64-w64-mingw64)

# Cross compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

# For CUDA cross-compilation
# NVCC will be used as CMAKE_CUDA_COMPILER by default if CUDA language is enabled.
# We need to tell NVCC to use the MinGW g++ as its host compiler.
set(CMAKE_CUDA_HOST_COMPILER ${TOOLCHAIN_PREFIX}-g++)

# Set a default install prefix for Windows builds (optional)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/${TOOLCHAIN_PREFIX}" CACHE PATH "Install path prefix")