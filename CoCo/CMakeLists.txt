set(CMAKE_FIND_FRAMEWORK LAST)		# so we get homebrew SDL2

include_directories(/opt/homebrew/include)
include_directories(/usr/local/include)

find_package(SDL2 REQUIRED)
find_library(COCOA_LIBRARY Cocoa)

include_directories(${SDL2_INCLUDE_DIRS})

link_directories("/usr/local/lib")	# for AGAR

set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-framework,Cocoa")

set(MACOSX_BUNDLE TRUE)
set(ICON_NAME "ovcc.ico")
set(ICON_PATH "./${ICON_NAME}")
set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
file(COPY ${ICON_PATH} DESTINATION "ovcc.app/Contents/Resources")

add_definitions(
	-DDARWIN
	-D_THREAD_SAFE
	-D_USE_OPENGL_FRAMEWORK
	-DTARGET_API_MAC_CARBON
	-DTARGET_API_MAC_OSX
	-fpascal-strings
	-DCOPY_PASTE)

add_executable(ovcc MACOSX_BUNDLE  ${ICON_PATH}
	_xdebug.c
	AGARInterface.c
	audioSDL.c
	cassette.c
	coco3.c
	config.c
	fileops.c
	hd6309.c
	iniman.c
	iobus.c
	joystickinputSDL.c
	keyboardAGAR.c
	keyboardLayoutAGAR.c
	logger.c
	mc6809.c
	mc6821.c
	pakinterface.c
	quickload.c
	tcc1014graphicsAGAR.c
	tcc1014mmu.c
	tcc1014mmu_common.c
	tcc1014mmu_mm.c
	tcc1014mmu_nomm.c
	tcc1014registers.c
	throttle.c
	vcc.c
	vccgui.c)

target_include_directories(ovcc PUBLIC EXPORT ${CMAKE_CURRENT_SOURCE_DIR})

#set_target_properties(ovcc PROPERTIES MACOSX_BUNDLE_ICONFILE ${ICON_PATH})
set_target_properties(ovcc PROPERTIES
    BUNDLE True
    MACOSX_BUNDLE_ICONFILE ${ICON_PATH}
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.domain.style.identifier.ovcc}
    MACOSX_BUNDLE_BUNDLE_NAME ovcc
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
#    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in
)

target_link_libraries(ovcc ${SDL2_LIBRARIES} ag_core ag_gui objc pthread ${COCOA_LIBRARY})
