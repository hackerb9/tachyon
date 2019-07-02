Commandline parameters for linux and Windows.
The linux parameters are always preceeded by '--' and
the Windows parameters have no such prefix.

  Possible arguements are:

  --gamsav  (linux only)
	Usage: --gamsav modules/chaos\ strikes\ back-atari/
	The directory where any saved game files are
	The game also tries to save in this directory
	If not set, then default is to save in your current working directory

  --directory <dir>   eg: --directory ../../mygame
  directory=<dir>     eg: directory=e:\mygame
	The directory which holds any game file

  --module   (linux only)
	Usage: --module /tmp/
	The directory which holds any other game file

  --root-path   (linux only)
	Usage: --root-path /tmp/
	The fall-back directory if the above pathways failed

  --height	 in pixels  (not implemented)

  --width	 in pixels  (not implemented)

  --quick  <n>    eg: --quick 2000000000
  Quick=<n>       eg: Quick=2000000000
	Play movie faster than light for n ticks of the clock

  --dungeon  (linux only)
	Usage: --dungeon dungeon.dat
	Play the game in another dungeon.

  --fullscreen
  FullScreen
        Not really 'full-screen' but an approximation

  --nosound  (linux only)
	Turn off all sound support. Default if no sounddriver has been
	compiled in.

  --sound-pipe   (linux only) (not implemented)
	Usage: --sound-pipe | esdcat -b -m -r 5120
	Pipe sound data to stdout.
	This data can be fed to a properly setup sound device,
	but the quality of the sound will wary.
	The sound data is 8-bit signed, mono and plays at 5120 Hz.
	Note that --sound-pipe does not take any arguments.

  --norecord
  NoRecord
	Prevents a dungeon to autorecord a session, even if the dungeon
	itself tries to force autorecord. It doesn't prevent you from
	manually selecting record. Actually, there is no reason not to
	use this switch.

  --record
	Autorecord a session. Files will be stored in the location as
	specified by the above arguments, such as --gamsav.
	This option is usefull for debugging new dungeons or for
        providing a movie so that others can be amazed at your prowess.

  --timer  (linux only)
	Usage: --timer 50
	Set internal callback-timer in ms.

  --humongous
  --extremelylarge
  --extralarge
  ExtraLarge
	Set screensize to 1280x960 pixels

  --verylarge
  VeryLarge
	Set screensize to 960x720 pixels

  --large
  Large
	Set screensize to 640x480 pixels

  --small
  Small
	Set screensize to 320x240 pixels

  --play <file>     eg: --play playfile.log
  Play=file         eg: Play=playfile.log
	Plays a recorded game. Might be reffered to as a "movie".
        You can interrupt the 'movie' with two consecutive Control-c.

  --version (linux only)
	Prints the version of the program and exits.
	First numbers is the version of the CSB engine. The last number
	(after the 'v') counts extra bug-fixes to the Linux-client only.
 
  --vblmultiplier  n   eg: vblmultiplier 99
  VBLMultiplier=n      eg: VBLMultiplier=99
        n should be in the range of 1 to 99.
        Increases the speed of the realtime clock.  Makes the game
        playback faster.  If multiplier is greater than 1 and less
        than 99 then every third frame is displayed.

  --repeat
  Repeat
        Repeats playback of movie indefinitely.


  FEATURES:
 

  KNOWN BUGS IN LINUX :
	Given the evolution of this program, it might be a good thing
	to list them here. 

	* Fullscreen is not working. 
	* The GTK menu feels slow because it's only updated every --timer
	  settings. Set timer to 10 ms if you want the GTK interface to be
	  faster. 
	* The DISPLAY variable is not honoured. 
	* Sound has always been a problem. We have now implemented an
	  option to pipe sound to stdout, so that you can format the
	  sound and send it to the soundcard by your own means. This
	  is an experimental solution, which in our testing works
	  quite bad.
	 
	If you encounter a bug then catch it on record, send the Record000.txt
	(whatever) and the  saved  game  you used to Paul R. Stevens
	<prsteven@facstaff.wisc.edu>.
	If the bug is not possible to catch on record, then send it to Erik
	Svanberg <svanberg@acc.umu.se>.
