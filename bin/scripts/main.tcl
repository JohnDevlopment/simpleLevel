# main script file
package require tdom

# making class objects
namespace import oo::class

# in case of repeated sourcing
foreach e {ScriptSystem SaveScript LevelScript} {
	catch [list $e destroy]
}
unset e

# var linked to C program: CurrentLevel, type: string, permissions: read/write
if {! [info exists CurrentLevel]} {set CurrentLevel "levels/rt1-1.dat"
} elseif {$CurrentLevel eq "NULL"} {set CurrentLevel "levels/rt1-1.dat"}

# call a process and traps and print its error message
proc CallProc {Proc args} {
	uplevel [subst {
	  set err {}
	  set code \[catch \[list $Proc {*}$args\] err\]
	  if {\$code} {puts stderr "Error returned from $Proc: \$err"}
	  unset code err
	}]
	
return
}

# open a file
proc OpenLevelExitFile {} {
	global CurrentLevel
	
	# name of the file
	set dir [file join [file dirname $CurrentLevel] config]
	set fn "[file root [file tail $CurrentLevel]].exits"
	
	# open file
	set id [open [file join $dir $fn] r]
	
	# read each line into a list
	set lFiles {}
	
	while {[gets $id line] > 0} {
	  lappend lFiles [file join levels $line]
	}
	unset line
	
return $lFiles
}

# open events file for level
proc OpenLevelEventFile {} {
	global CurrentLevel
	global Doc
	
	# name of file
	set dir [file join [file dirname $CurrentLevel] config]
	set fn "[file root [file tail $CurrentLevel]].evt"
	
	# open file
	set id [open [file join $dir $fn] r]
	
	# turn file into a document tree
	set Doc [dom parse -channel $id]
	
	# close file
	close $id
	
	# root of tree
	set root [$Doc documentElement]
	
	# if the doctype is wrong, reject the file
	if {[$root nodeName] ne "events"} {
	  after 500 {
	  	$Doc delete
	  	unset Doc
	  }
	  
	  return -code 1 "Incorrect doctype \"[$root nodeName]\""
	}
	
	# look through all <trigger /> nodes
	set temp {_triggerFromDoc $node}
	
	foreach node [$root childNodes] {
	  # turn each trigger node into a commandline for Triggers_CreateEvent
	  if {[catch $temp cmd]} {
	  	puts "Error returned from _triggerFromDoc: $cmd"
	  } else {
	  	CallProc {*}$cmd
	  }
	}
	
return
}

# open script file for level
proc OpenLevelScript {} {
	global CurrentLevel
	
	# name of file -> .dat changed to .scr -> in levels/config
	set dir [file join [file dirname $CurrentLevel] config]
	set fn "[file root [file tail $CurrentLevel]].scr"
	
	# open file
	set id [open [file join $dir $fn] r]
	
	# read each line and form a command string
	set true 1
	set false 0
	
	set background $false
	
	while {[gets $id line] > 0} {
	  set cmd  [lindex $line 0]
	  set args [lreplace $line 0 0]
	  
	  # command based on first word
	  switch -exact $cmd {
	  	sprite {
	  	  set cmd Level_NewSprite
	  	}
	  	
	  	player {
	  	  set cmd Level_PlayerLoc
	  	}
	  	
	  	background {
	  	  set cmd Level_ConfBackground
	  	}
	  	
	  	default {
	  	  set cmd NULL
	  	}
	  }
	  
	  # evaluate string
	  if {$cmd ne "NULL"} {
	  	::CallProc $cmd {*}$args
	  }
	}
	
	# close file	
	close $id
	
return
}

namespace eval LevelNamespace {
	namespace ensemble create -command ::Level -map {index GetIndex}
	
	proc GetIndex {fileStr} {
		set ret -1
		
		# name of the level determines value
		switch [file rootname [file tail $fileStr]] {
		  rt1-1 {
		  	set ret 0
		  }
		  rt1-2 {
		  	set ret 1
		  }
		  default {}
		}
		
	return $ret
	}
}
