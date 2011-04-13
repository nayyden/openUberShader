project(glslubershaderapp)

add_executable(glslubershaderapp main.cpp App.cpp OgreFramework.cpp )

target_link_libraries(glslubershaderapp -lOgreMain -lOIS )