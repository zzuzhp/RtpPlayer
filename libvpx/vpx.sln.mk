MSBUILD_TOOL := msbuild.exe
found_devenv := $(shell which $(MSBUILD_TOOL) >/dev/null 2>&1 && echo yes)
.nodevenv.once:
	@echo "  * $(MSBUILD_TOOL) not found in path."
	@echo "  * "
	@echo "  * You will have to build all configurations manually using the"
	@echo "  * Visual Studio IDE. To allow make to build them automatically,"
	@echo "  * add the Common7/IDE directory of your Visual Studio"
	@echo "  * installation to your path, eg:"
	@echo "  *   C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE"
	@echo "  * "
	@touch $@
CLEAN-OBJS += $(if $(found_devenv),,.nodevenv.once)

BUILD_TARGETS += $(if $(NO_LAUNCH_DEVENV),,Debug_x64)
clean::
	rm -rf "x64"/"Debug"
.PHONY: Debug_x64
ifneq ($(found_devenv),)
Debug_x64: vpx.sln
	$(MSBUILD_TOOL) vpx.sln -m -t:Build \
		-p:Configuration="Debug" -p:Platform="x64"
else
Debug_x64: vpx.sln .nodevenv.once
	@echo "  * Skipping build of Debug|x64 ($(MSBUILD_TOOL) not in path)."
	@echo "  * "
endif

BUILD_TARGETS += $(if $(NO_LAUNCH_DEVENV),,Release_x64)
clean::
	rm -rf "x64"/"Release"
.PHONY: Release_x64
ifneq ($(found_devenv),)
Release_x64: vpx.sln
	$(MSBUILD_TOOL) vpx.sln -m -t:Build \
		-p:Configuration="Release" -p:Platform="x64"
else
Release_x64: vpx.sln .nodevenv.once
	@echo "  * Skipping build of Release|x64 ($(MSBUILD_TOOL) not in path)."
	@echo "  * "
endif

