# Build, push, or run a libfly CI container.

.PHONY: help
.PHONY: build
.PHONY: push
.PHONY: run

# Set the path to the build system.
BUILD_ROOT := $(CURDIR)/../nix

# Import the build system.
include $(BUILD_ROOT)/system.mk
include $(BUILD_ROOT)/config.mk
include $(BUILD_ROOT)/flags.mk

# Docker settings.
USER := trflynn89
REPO := libfly
BASE := $(shell grep -oP "(?<=FROM )(\S+)" Dockerfile)

# Toolchain versions.
CLANG_VERSION := 11
GCC_VERSION := 10
JDK_VERSION := 15

# Form the tag for the container.
TAG := $(subst :,,$(subst .,,$(BASE)))_clang$(CLANG_VERSION)_gcc$(GCC_VERSION)_jdk$(JDK_VERSION)

help:
	$(Q)echo "Build, push, or run a $(REPO) container."
	$(Q)echo
	$(Q)echo "Container will be based on $(BASE), tagged $(TAG), and contain:"
	$(Q)echo
	$(Q)echo "    Clang $(CLANG_VERSION)"
	$(Q)echo "    GCC $(GCC_VERSION)"
	$(Q)echo "    JDK $(JDK_VERSION)"
	$(Q)echo
	$(Q)echo "Usage: make [build] [push] [run] [clean]"

build:
	$(Q)docker build \
		--tag $(USER)/$(REPO):$(TAG) \
		--build-arg CLANG_VERSION=$(CLANG_VERSION) \
		--build-arg GCC_VERSION=$(GCC_VERSION) \
		--build-arg JDK_VERSION=$(JDK_VERSION) \
		.

push: build
	$(Q)docker push $(USER)/$(REPO):$(TAG)

run: build
	$(Q)docker rm $(REPO) > /dev/null 2>&1 || true

	$(Q)docker run -it \
		--name $(REPO) \
		--cap-add SYS_PTRACE \
		$(USER)/$(REPO):$(TAG)

clean:
	$(Q)docker rm $(REPO) > /dev/null 2>&1 || true
	$(Q)docker image rm $(shell docker images -q) > /dev/null 2>&1 || true
