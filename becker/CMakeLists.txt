include_directories(/opt/homebrew/include)
include_directories(/opt/homebrew/include/SDL2)
include_directories(/usr/local/include)
include_directories(/usr/local/include/agar)
include_directories(../CoCo)

link_directories(/opt/homebrew/lib)
link_directories(/usr/local/lib)

add_definitions(
	-DDARWIN
	-D_THREAD_SAFE
	-D_USE_OPENGL_FRAMEWORK
	-DTARGET_API_MAC_CARBON
	-DTARGET_API_MAC_OSX
	-fpascal-strings)

add_library(becker MODULE becker.c ../CoCo/iniman.c)

target_include_directories(becker PUBLIC EXPORT ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(becker SDL2)

TARGET_LINK_LIBRARIES(becker ag_core ag_gui ag_math ag_net ag_sg ag_sk ag_vg ag_core pthread)
