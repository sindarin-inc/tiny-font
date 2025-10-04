.PHONY: build clean menuconfig save-defconfig

.DEFAULT_GOAL := build

TARGET ?= ibmf
SDKCONFIG_DEFAULTS = sdkconfig.defaults.$(TARGET)
IDF_PROFILE = -B build-$(TARGET) -DSDKCONFIG_DEFAULTS="$(SDKCONFIG_DEFAULTS)"

build:
	idf.py $(IDF_PROFILE) build

menuconfig:
	idf.py $(IDF_PROFILE) menuconfig

save-defconfig:
	idf.py $(IDF_PROFILE) save-defconfig

clean:
	rm -rf build*
