# main tcl file
# to be included by simpleLevel
if {! [info exists SIMPLELEVEL]} {
	error "file must be sourced by simpleLevel"
}
unset SIMPLELEVEL

# include global definitions file
source "scripts/globals.tcl"

# processes used by the rest of the program
proc CallProc {name args} {
	global code
	global err
	
	# execute command and trap errors
	uplevel "#0" [subst -nocommands {
	  set code [catch [list $name {*}$args] err]
	}]
	
	# print error message if applicable
	if {$code} {
	  puts stderr $err
	}
	
	# set return value based on error code
	if {! $code} {set retval $err} else {set retval ""}
	
return $retval
}

CallProc level test
CallProc level background

set value [CallProc GetHash rt1-1.dat 5000 10]
puts "Hash value for rt1-1.dat: $value"
