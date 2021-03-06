# detects the operating system
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set (WINDOWS TRUE)
	set (PLATFORM_DIR win32)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set (LINUX TRUE)
	set (PLATFORM_DIR linux)
else ()
	message (FATAL_ERROR "Current platform ${CMAKE_SYSTEM_NAME} is not supported")
	return ()
endif ()

# check version of virtual studio
if (WINDOWS)
	if (MSVC)
	  if (${MSVC_VERSION} EQUAL 1900 OR ${MSVC_VERSION} GREATER 1900)
	  	message (STATUS "Using windows MSVC compiler version ${MSVC_VERSION}")
	  else ()
	  	message (FATAL_ERROR "invalid version of MSVC: lesser than needed")
	  endif ()
	else ()
	  # not the right compiler, apparently
	  message (FATAL_ERROR "wrong compiler for windows, should be MSVC")
	endif ()
endif ()

# set macro definitions for the platform
function (set_os_compiler_defines target)
	if (WINDOWS)
	  target_compile_definitions (
	  	${target}
	  	  PUBLIC WIN32
	  	  PUBLIC _WIN32
	  	  PUBLIC _WINDOWS
	  	  PUBLIC UNICODE
	  	  PUBLIC _CRT_SECURE_NO_WARNINGS
	  	  PUBLIC _SCL_SECURE_NO_WARNINGS
	  )
	  message (STATUS "Compiler defines: WIN32 _WIN32 _WINDOWS UNICODE _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS")
	elseif (LINUX)
	  target_compile_definitions (${target} PUBLIC LINUX)
	  target_compile_definitions (${target} PUBLIC _GNU_SOURCE)
	  message (STATUS "Compiler defines: LINUX _GNU_SOURCE")
	endif ()
endfunction ()
