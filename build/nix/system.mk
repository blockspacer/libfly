# Determine information about the host system environment and define system-dependent variables.

ARCH := $(shell uname -m)
SUDO := $(shell which sudo)

# Define installation directories.
INSTALL_BIN_DIR := /usr/local/bin
INSTALL_INC_DIR := /usr/local/include
INSTALL_SRC_DIR := /usr/local/src
INSTALL_LIB_DIR := /usr/local/lib

# Determine host operating system.
ifneq ($(wildcard /etc/debian_version),)
    SYSTEM := LINUX
    VENDOR := DEBIAN
else ifneq ($(wildcard /etc/redhat-release),)
    SYSTEM := LINUX
    VENDOR := REDHAT
else ifeq ($(shell uname -s), Darwin)
    SYSTEM := MACOS
    VENDOR := APPLE
    XCODE := $(shell xcode-select -p)
else
    $(error Could not determine operating system, check system.mk)
endif

# Determine default architecture.
ifeq ($(SYSTEM), LINUX)
    SUPPORTED_ARCH := x64 x86
else ifeq ($(SYSTEM), MACOS)
    SUPPORTED_ARCH := x64
else
    $(error Unrecognized system $(SYSTEM), check system.mk)
endif

ifneq ($(findstring x86_64, $(ARCH)),)
    arch := x64
else ifeq ($(arch), x64)
    $(error Cannot build 64-bit architecture on 32-bit machine)
else
    arch := x86
endif

ifneq ($(arch), $(filter $(SUPPORTED_ARCH), $(arch)))
    $(error Architecture $(arch) not supported, check system.mk)
endif
