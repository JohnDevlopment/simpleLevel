# defines the BackgroundFunctions namespace
namespace eval BackgroundFunctions {
	namespace ensemble create -command ::Level_ConfBackground -map {color setColor load loadImage offset offsetImage speeds setSpeeds}
	
	puts {BackgroundFunctions defined}
	
	# proc are defined on the C side
}
