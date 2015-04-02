#--- The exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
add_executable(${EXERCISENAME} cube.h trackball.h main.cpp _grid/grid.h)
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
target_deploy_file(${EXERCISENAME} cube_vshader.glsl)
target_deploy_file(${EXERCISENAME} cube_fshader.glsl)
target_deploy_file(${EXERCISENAME} _grid/grid_vshader.glsl)
target_deploy_file(${EXERCISENAME} _grid/grid_fshader.glsl)
target_deploy_file(${EXERCISENAME} _grid/grid_texture.tga)
