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

add_library(harddisk MODULE harddisk.c cc3vhd.c cloud9.c ../CoCo/iniman.c ../CoCo/fileops.c)

target_include_directories(harddisk PUBLIC EXPORT ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(harddisk SDL2)

TARGET_LINK_LIBRARIES(harddisk ag_core ag_gui pthread)
