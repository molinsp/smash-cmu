######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

OPENCV_ROOT=$(PACKAGES_ROOT)/opencv/opencv

all: old


old:

	@echo "Starting OpenCV"

#check if packages/opencv/opencv exists
ifeq ($(wildcard $(OPENCV_ROOT)),)
	@echo "Downloading opencv source"
	@git clone git://code.opencv.org/opencv.git
else
	@echo "opencv source already exists"
endif

#create toolchain.cmake
ifeq ($(wildcard $(OPENCV_ROOT)/Makefile),)
	@echo "Generating OpenCV makefiles"
	@echo "set ( CMAKE_SYSTEM_NAME Linux )" > $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( COMPILER_ROOT $(LOCAL_CROSS) )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_FIND_ROOT_PATH $(INSTALL_ROOT) )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_C_COMPILER $(LOCAL_GCC) )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_CXX_COMPILER $(LOCAL_GPLUSPLUS) )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_INSTALL_PREFIX $(INSTALL_ROOT) )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_LIBRARY_PATH $(INSTALL_ROOT)/lib \$${CMAKE_LIBRARY_PATH} )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( CMAKE_INCLUDE_PATH $(INSTALL_ROOT)/include \$${CMAKE_INCLUDE_PATH} )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_features2d ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_ml ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_flann ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_objdetect ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_photo ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_video ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_contrib OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_ts ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_legacy ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_nonfree OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_stitching OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_videostab OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_opencv_gpu OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_PERF_TESTS OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_TESTS OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_JPEG ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( WITH_GTK OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( BUILD_ZLIB ON )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "set ( WITH_PNG OFF )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "SET ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "SET ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )" >> $(OPENCV_ROOT)/toolchain.cmake
	@echo "SET ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )" >> $(OPENCV_ROOT)/toolchain.cmake
	@cd $(OPENCV_ROOT); cmake -DCMAKE_TOOLCHAIN_FILE=$(OPENCV_ROOT)/toolchain.cmake $(OPENCV_ROOT)
else
	@echo "Makefiles already generated"
endif

ifeq ($(wildcard $(OPENCV_ROOT)/lib/libopencv_core.so),)
	@echo "Building OpenCV"
	@cd $(OPENCV_ROOT); make
else
	@echo "OpenCV already built"
endif

ifeq ($(wildcard $(INSTALL_ROOT)/lib/libopencv_core.so),)
	@echo "Installing OpenCV"
	@cd $(OPENCV_ROOT); make install
else
	@echo "OpenCV already installed"
endif

	@echo "Done in OpenCV"

update: realclean
	cd opencv; git pull

clean:

realclean: clean
