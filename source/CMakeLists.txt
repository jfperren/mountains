#--- The exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SHADERS "*.glsl")
file(GLOB_RECURSE TEXTURES "*.tga")

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})

target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})

target_deploy_file(${EXERCISENAME} ../external/AntTweakBar/lib/x64/AntTweakBar64.dll)

target_deploy_file(${EXERCISENAME} ${TEXTURES})
