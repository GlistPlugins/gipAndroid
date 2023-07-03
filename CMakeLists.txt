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
)


##### PLUGIN DEPENDENCY LINKING #####
add_subdirectory(${PLUGIN_DIR}/deps/assimp ${PLUGIN_DIR}/build EXCLUDE_FROM_ALL)
list(APPEND PLUGIN_LINKLIBS
		assimp)

