cmake_minimum_required (VERSION 3.10.2)

##### PLUGIN NAME #####
set(pluginname gipAndroid)


##### PLUGIN PATHS #####
set(PLUGIN_DIR ${PLUGINS_DIR}/${pluginname})
set(PLUGIN_LIBS_DIR ${PLUGIN_DIR}/libs)


##### PLUGIN HEADERS #####
list(APPEND PLUGIN_INCLUDES
		${PLUGIN_DIR}/src
		${PLUGIN_DIR}/include
		${PLUGIN_DIR}/include/freetype2
		${PLUGIN_DIR}/libs/openssl/${ANDROID_ABI}/include
		${PLUGIN_DIR}/libs/curl/${ANDROID_ABI}/include
)

##### PLUGIN SOURCES #####
list(APPEND PLUGIN_SRCS
		${PLUGIN_DIR}/src/gAndroidDialog.cpp
)


##### PLUGIN DEPENDENCY LINKING #####
find_library( # Sets the name of the path variable.
		log-lib

		# Specifies the name of the NDK library that
		# you want CMake to locate.
		log)

add_library(fmod SHARED IMPORTED)
set_target_properties(fmod PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/fmod/${ANDROID_ABI}/libfmod.so)

add_library(fmodL SHARED IMPORTED)
set_target_properties(fmodL PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/fmod/${ANDROID_ABI}/libfmodL.so)

add_library(libpng STATIC IMPORTED)
set_target_properties(libpng PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/libpng/${ANDROID_ABI}/libpng.a)

add_library(freetype STATIC IMPORTED)
set_target_properties(freetype PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/freetype/${ANDROID_ABI}/libfreetype.a)

add_library(libcrypto STATIC IMPORTED)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/openssl/${ANDROID_ABI}/lib/libcrypto.a)

add_library(libssl STATIC IMPORTED)
set_target_properties(libssl PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/openssl/${ANDROID_ABI}/lib/libssl.a)

add_library(libcurl STATIC IMPORTED)
set_target_properties(libcurl PROPERTIES IMPORTED_LOCATION
		${ANDROID_DIR}/libs/curl/${ANDROID_ABI}/lib/libcurl.a)

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ANDROID_NATIVE_API_LEVEL android-24)
set(ANDROID_PLATFORM android-24)
add_subdirectory(${PLUGIN_DIR}/deps/assimp ${CMAKE_BINARY_DIR}/assimp EXCLUDE_FROM_ALL)

list(APPEND PLUGIN_LINKLIBS
		fmodL
		fmod
		assimp
		freetype
		libpng
		android
		log
		EGL
		GLESv3
		libssl
		libcrypto
		libcurl
)
