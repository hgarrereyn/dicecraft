
add_executable(dicecraft
    game.cpp
    world.cpp
    chunk.cpp
    player.cpp
    saver.cpp
    lodepng.cpp
)

add_subdirectory(glad)

target_link_libraries(dicecraft glfw)
target_link_libraries(dicecraft glm)
target_link_libraries(dicecraft glad)
set_target_properties(dicecraft PROPERTIES COMPILE_FLAGS -DBUILDER_STATIC_DEFINE)


add_executable(gen
    world_gen.cpp
    world.cpp
    chunk.cpp
    player.cpp
    saver.cpp
    lodepng.cpp
)

target_link_libraries(gen glfw)
target_link_libraries(gen glm)
target_link_libraries(gen glad)
