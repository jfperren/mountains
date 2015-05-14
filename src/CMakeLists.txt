FILE(GLOB_RECURSE APP app/*)
FILE(GLOB_RECURSE BUFFERS buffers/*)
FILE(GLOB_RECURSE CAMERA camera/*)
FILE(GLOB_RECURSE NOISE noise/*)
FILE(GLOB_RECURSE SCENE scene/*)
FILE(GLOB_RECURSE TEXTURES textures/*)
FILE(GLOB_RECURSE TW tw/*)

source_group(app FILES ${APP})
source_group(buffers FILES ${BUFFERS})
source_group(camera FILES ${CAMERA})
source_group(noise FILES ${NOISE})
source_group(scene FILES ${SCENE})
source_group(textures FILES ${TEXTURES})
source_group(tw FILES ${TW})

add_executable(Mountains ${APP} ${BUFFERS} ${CAMERA} ${NOISE} ${SCENE} ${TEXTURES} ${TW})

target_link_libraries(Mountains ${COMMON_LIBS})
target_deploy_file(Mountains ../external/AntTweakBar/lib/x64/AntTweakBar64.dll)

