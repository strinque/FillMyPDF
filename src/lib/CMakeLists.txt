cmake_minimum_required(VERSION 3.20)
project(FillMyPDF-lib)
set(TARGET_LIB "FillMyPDF-lib" CACHE INTERNAL "")
set(TARGET_NAME "FillMyPDF")

# set required c++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set project source-files
set(SOURCE_FILES
  FillMyPDF.cpp)
set(HEADER_FILES
  FillMyPDF.h)

# compile static library
add_library(${TARGET_LIB} STATIC ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(${TARGET_LIB} PROPERTIES OUTPUT_NAME ${TARGET_NAME})

# list of required third-party libraries
find_package(freetype CONFIG REQUIRED)
find_package(unofficial-brotli CONFIG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# force utf-8 encoding for source-files
add_compile_options($<$<C_COMPILER_ID:MSVC>:/utf-8>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

# generic configuration
string(CONCAT CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded"
  $<$<CONFIG:Debug>:DebugDLL>
  $<$<NOT:$<CONFIG:Debug>>:DLL>)
target_include_directories(${TARGET_LIB}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include>)
target_compile_options(${TARGET_LIB}
  PRIVATE
    /EHsc)
target_link_options(${TARGET_LIB}
  PRIVATE
    $<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>)

# link third-party libraries
target_link_libraries(${TARGET_LIB}
  PRIVATE
    $<$<PLATFORM_ID:Windows>:ws2_32>
    $<$<PLATFORM_ID:Windows>:crypt32>
    freetype
    JPEG::JPEG
    ZLIB::ZLIB
    PNG::PNG
    BZip2::BZip2
    unofficial::brotli::brotlicommon
    unofficial::brotli::brotlidec
    OpenSSL::Crypto
    OpenSSL::SSL
    podofo.lib
    nlohmann_json::nlohmann_json)

# install files
install(TARGETS ${TARGET_LIB} EXPORT ${TARGET_LIB}Config
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
string(CONCAT HEADERS_DESTINATION
  $<$<NOT:$<CONFIG:Debug>>:${CMAKE_INSTALL_PREFIX}/include>
  $<$<CONFIG:Debug>:${CMAKE_INSTALL_PREFIX}/../include>)
install(FILES ${HEADER_FILES} DESTINATION ${HEADERS_DESTINATION})
install(EXPORT ${TARGET_LIB}Config DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)

# organize files for visual-studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group("Headers Files" FILES ${HEADER_FILES})
source_group("Sources Files" FILES ${SOURCE_FILES})