project(glslubershaderapp)

add_executable(glslubershaderapp main.cpp App.cpp OgreFramework.cpp )

if(UNIX)
  target_link_libraries(glslubershaderapp -lOgreMain -lOIS )
endif(UNIX)

if(WIN32)
  target_link_libraries(glslubershaderapp build/OgreMain.dll build/OIS.dll )
endif(WIN32)