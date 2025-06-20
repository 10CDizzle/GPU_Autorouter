ARG CUDA_VERSION="12.1.0"
ARG UBUNTU_VERSION="22.04"
FROM nvidia/cuda:${CUDA_VERSION}-devel-ubuntu${UBUNTU_VERSION}

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install essential build tools, CMake, Git, Ninja, and Qt6
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    g++ \
    g++-mingw-w64-x86-64 \
    binutils-mingw-w64-x86-64 \
    mingw-w64-x86-64-dev \
    g++-multilib \
    gcc-multilib \
    cmake \
    git \
    ninja-build \
    # Qt6 development libraries
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-tools-dev \
    libqt6svg6-dev \
    # Dependencies for running Qt GUI applications via X11 forwarding
    libgl1-mesa-dev \
    libxkbcommon-x11-0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-randr0 \
    libxcb-render-util0 \
    libxcb-xinerama0 \
    libxcb-xfixes0 \
    libegl1-mesa \
    libfontconfig1 \
    libfreetype6 \
    libopengl0 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user 'vscode' with sudo access
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}
RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && apt-get update && apt-get install -y sudo \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

# Switch to the non-root user
USER ${USERNAME}