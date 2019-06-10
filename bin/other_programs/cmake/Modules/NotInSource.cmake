# prevent in-source builds
function (AssureOutOfSourceBuilds)
	get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
	get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)
	
	# disallow in-source builds
	if ("${srcdir}" STREQUAL "${bindir}")
		message ("----------------------------------------------------------------------------")
		message ("- image_class must not be compiled in the same directory as the source files")
		message ("----------------------------------------------------------------------------")
		message (FATAL_ERROR "Quitting configuration...")
	endif ()
endfunction ()

AssureOutOfSourceBuilds ()
