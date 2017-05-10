set(CTEST_SOURCE_DIRECTORY "$ENV{APPVEYOR_BUILD_FOLDER}")
set(CTEST_BINARY_DIRECTORY "C:/projects/DREAM3D-Release-appveyor")
# We need to set this otherwise we get 255 as our return code!
set(CTEST_COMMAND ctest)
include(${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake)
set(CTEST_SITE "AppVeyor")
set(_prefix "")
if (DEFINED ENV{APPVEYOR_PULL_REQUEST_NUMBER})
  set(_prefix "-pull_$ENV{APPVEYOR_PULL_REQUEST_NUMBER}")
endif()
set(CTEST_BUILD_NAME "Release-VS2015x64${_prefix}-$ENV{APPVEYOR_BUILD_NUMBER}")
set(CTEST_CMAKE_GENERATOR "Visual Studio 14 2015 Win64")
set(CTEST_CONFIGURATION_TYPE "Release")

set(DREAM3D_SDK_ROOT "C:/DREAM3D_SDK/")
set(DREAM3D_SDK_FILE "C:/DREAM3D_SDK/DREAM3D_SDK.cmake")
FILE(WRITE "${DREAM3D_SDK_FILE}" "# DREAM3D_SDK File")

#-----------------------

set(BUILD_SHARED_LIBS ON CACHE BOOL "")
set(DREAM3D_DATA_DIR $ENV{APPVEYOR_BUILD_FOLDER}/DREAM3D_Data CACHE PATH "")
set(Qt5Core_DIR "${DREAM3D_SDK_ROOT}/Qt5.6.2/5.6/msvc2015_64/lib/cmake/Qt5Core" CACHE PATH "")
set(Qt5_DIR "${DREAM3D_SDK_ROOT}/Qt5.6.2/5.6/msvc2015_64/lib/cmake/Qt5" CACHE PATH "")
set(QWT_INSTALL "${DREAM3D_SDK_ROOT}/Qwt-6.1.3" CACHE PATH "")
set(EIGEN_INSTALL "${DREAM3D_SDK_ROOT}/Eigen-3.2.9" CACHE PATH "")
set(DOXYGEN_EXECUTABLE "${DREAM3D_SDK_ROOT}/doxygen-1.8.11-windows.x64.bin/doxygen.exe" CACHE FILEPATH "")
set(HDF5_INSTALL "${DREAM3D_SDK_ROOT}/hdf5-1.8.16" CACHE PATH "")
set(HDF5_DIR "${DREAM3D_SDK_ROOT}/hdf5-1.8.16/cmake" CACHE PATH "")
set(BOOST_ROOT "${DREAM3D_SDK_ROOT}/boost-1.60.0" CACHE PATH "")
set(BOOST_INCLUDE_DIR "${DREAM3D_SDK_ROOT}/boost-1.60.0/include/boost-1_60" CACHE PATH "")
set(DREAM3D_USE_MULTITHREADED_ALGOS ON CACHE BOOL "")
set(TBB_ROOT "${DREAM3D_SDK_ROOT}/tbb44_20160526oss" CACHE PATH "")
set(TBB_ARCH_TYPE "intel64" CACHE STRING "")
set(ITK_DIR "${DREAM3D_SDK_ROOT}/ITK-4.11.0")

#-----------------------
  file(WRITE ${CTEST_BINARY_DIRECTORY}/CMakeCache.txt "
    SITE:STRING=${CTEST_SITE}
    BUILDNAME:STRING=${CTEST_BUILD_NAME}
    # CTEST_USE_LAUNCHERS:BOOL=${CTEST_USE_LAUNCHERS}
    DART_TESTING_TIMEOUT:STRING=${CTEST_TEST_TIMEOUT}
    ${data_options}
    ${cache_build_type}
    ${cache_make_program}
    ${dashboard_cache}
    #DREAM3D_SDK:PATH=${DREAM3D_SDK_ROOT}
    BUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    DREAM3D_DATA_DIR:PATH=${DREAM3D_DATA_DIR}
    DREAM3D_DATA_ROOT:PATH=${DREAM3D_DATA_DIR}
    Qt5_DIR:PATH=${Qt5_DIR}
    EIGEN_INSTALL:PATH=${EIGEN_INSTALL}
    QWT_INSTALL:PATH=${QWT_INSTALL}
    HDF5_INSTALL:PATH=${HDF5_INSTALL}
    HDF_DIR:PATH=${HDF5_DIR}
    BOOST_ROOT:PATH=${BOOST_ROOT}
    TBB_ROOT:PATH=${TBB_ROOT}
    DREAM3D_USE_MULTITHREADED_ALGOS:BOOL=${DREAM3D_USE_MULTITHREADED_ALGOS}
    ITK_DIR:PATH=${ITK_DIR}
    DREAM3D_EXTRA_PLUGINS:STRING=${DREAM3D_EXTRA_PLUGINS}
    DREAM3D_BUILD_TEST_FILTERS=${BUILD_TEST_FILTERS}
    DREAM3D_BUILD_DEVHELPER=${BUILD_DEVHELPER}
    SIMPL_DISABLE_MSVC_WARNINGS:BOOL=ON
    ")

  set(cfg_options 
    "-DCMAKE_BUILD_TYPE:STRING=Release" 
    "-DHDF5_DIR:PATH=C:/DREAM3D_SDK/hdf5-1.8.16/cmake")

ctest_start("Continuous")
ctest_configure(OPTIONS "${cfg_options}")
ctest_build()
ctest_test(RETURN_VALUE rv)
ctest_submit()

if(NOT rv EQUAL 0)
  message(FATAL_ERROR "Test failures occurred.")
endif()