#set(CMAKE_DARWIN_ARCHITECTURES "arm64")

#
# Standard CMake libraries
#
#include(CheckLibraryExists)
#include(CheckIncludeFiles)
#include(CheckIncludeFile)
#include(CheckSymbolExists)
#include(CheckCSourceCompiles)
#include(CheckCSourceRuns)
#include(CheckCCompilerFlag)
#include(CheckCXXCompilerFlag)
#include(CheckOBJCSourceCompiles)
#include(CheckTypeSize)
#include(CheckStructHasMember)
#include(CMakeDependentOption)
#include(FindPkgConfig)
#include(GNUInstallDirs)

# Detect known platforms.
#BB_Detect_Platform()

set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
#if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#	set(ARCH_64 TRUE)
	#
	# Default to the Large memory model on 64-bit. It is still possible for
	# 64-bit platforms to use the Medium model (e.g., for lower memory utilization).
	#
#	set(AGAR_DEFAULT_MEMORY_MODEL "L")
#else()
#	set(ARCH_64 FALSE)
	#
	# Default to the Medium memory model on 32-bit. It is still possible for
	# 32-bit platforms to use the Large model (e.g., for Deep Color support).
	#
#	set(AGAR_DEFAULT_MEMORY_MODEL "M")
#endif()

#find_package(SDL2 REQUIRED)

include_directories("/opt/homebrew/include")
include_directories("/opt/homebrew/include/SDL2")
include_directories("/usr/local/include")
include_directories("/usr/local/include/agar")
#include_directories(${SDL2_INCLUDE_DIRS})

link_directories("/opt/homebrew/lib")
link_directories("/usr/local/lib")

add_definitions(
	"-DDARWIN"
	"-D_THREAD_SAFE"
	"-D_USE_OPENGL_FRAMEWORK"
	"-DTARGET_API_MAC_CARBON"
	"-DTARGET_API_MAC_OSX"
	"-fpascal-strings")

add_executable(ovcc
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
	vccgui.c
	hd6309asmP0-00-7F.asm
	hd6309asmP0-80-FF.asm
	hd6309asmP1-00-FF.asm
	hd6309asmP2-00-FF.asm
	hd6309asmReg2Reg.asm
	hd6309asm_h.asm
	hd6309cea.asm
	hd6309pshpul.asm
	hd6309saveflags.asm
	hd6309tfrexg.asm
	ovcc.ico)

target_include_directories(ovcc PUBLIC EXPORT ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ovcc SDL2)
#target_link_libraries(ovcc ${AGAR_LIBRARIES})

TARGET_LINK_LIBRARIES(ovcc ag_core ag_gui ag_math ag_net ag_sg ag_sk ag_vg ag_core pthread)
#TARGET_LINK_LIBRARIES(ovcc pthread)

