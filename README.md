# AstroneerCameraFix
Fixes annoying camera smoothing applied by the devs that you cannot disable making some players nauseated

How does this work? Loops through GObjects and matches names with certain instances who have properties to the camera/mouse smoothing, turns down the property to 0 esssentially disabling it

Pattern scans for GObjects and namepool data, so I dont have to search for the new addresses everytime the game updates
