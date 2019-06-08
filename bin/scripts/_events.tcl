# meant to help parse certain events
#proc parseTrigger {type dictVal} {
#	dict update dictVal x X y Y width WIDTH height HEIGHT {}
#	
#	switch -exact $type {
#	  camera {
#	  	set cmd [list Triggers_CreateEvent camera $X $Y $WIDTH $HEIGHT\
#	  	 [dict get $dictVal speed] [dict get $dictVal increments]]
#	  }
#	  
#	  default {
#	  	set cmd [list puts ...]
#	  }
#	}
#	
#	tailcall {*}$cmd
#}

proc _triggerFromDoc {node} {
	# concatenate Tcl script that creates an event
	set cmd Triggers_CreateEvent
	
	# replace a word denoting the type with an integer
	switch -exact -- [$node getAttribute type] {
	  camera {
	  	lappend cmd 0
	  }
	  
	  cutscene {
	  	lappend cmd 1
	  }
	  
	  dialogue {
	  	lappend cmd 2
	  }
	  
	  input {
	  	lappend cmd 3
	  }
	  
	  default {}
	}
	
	# bounding box
	lappend cmd [$node getAttribute x] [$node getAttribute y] [$node getAttribute width] [$node getAttribute height]
	
	# look for <condition /> elements
	set flags 0
	set err {}
	set condList {}
	set cond {}
	set e {}
	
	if {! [catch "$node getElementsByTagName condition" condList]} {
	  foreach e $condList {
	  	if {[catch "$e getAttribute expr" cond]} continue
	  	
	  	# $cond = some kind of Tcl script
	  	if {[catch $cond err]} {
	  	  puts stderr "Error processing condition: $err"
	  	}
	  	
	  	# OR the result to $flags
	  	set flags [expr "$flags | $err"]
	  }
	}
	
	# if any of the conditions hold true, don't load this event
	if {$flags} return [list expr "1 + 1"]
	
	unset flags cond condList
	
	# arguments to the creation function
	switch -exact -- [$node getAttribute type] {
	  camera {
	  	# locate $e's text node and take its value
	  	foreach e {speed increments} {
	  	  set txtNode [$node selectNodes "$e/text()"]
	  	  lappend cmd [$txtNode nodeValue]
	  	}
	  	unset e txtNode
	  }
	  
	  dialogue {
	  	# locate name's text node and take its value
	  	if {[catch {$node selectNodes name/text()} res]} {
	  	  lappend cmd ...
	  	} else {lappend cmd [$res nodeValue]}
	  	
	  	unset res
	  	
	  	# locate all message nodes
	  	set cnt 0 ;# how many message tags there are
	  	set blk {}
	  	
	  	foreach txtNode [$node selectNodes message/text()] {
	  	  set data [string trimright [$txtNode nodeValue]]
	  	  if {$data == ""} {return -code 1 {A <message /> tag is empty}}
	  	  
	  	  incr cnt
	  	  lappend blk $data
	  	}
	  	
	  	lappend cmd $cnt {*}$blk
	  }
	  
	  default {
	  	return -code 1 "Trigger type \"[lindex $cmd 1]\" is unknown"
	  }
	}
	
return $cmd
}
