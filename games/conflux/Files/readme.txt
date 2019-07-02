Use Alt-Enter to toggle full-screen mode.
Two Control-C to terminate.

  Possible arguements are:

  directory=<dir>     eg: directory=e:\mygame
	The directory which holds any game file

  Quick=<n>       eg: Quick=2000000000
	Play movie faster than light for n ticks of the clock

  size=full
  FullScreen
        Not really 'full-screen' but an approximation

  NoRecord
	Prevents a dungeon to autorecord a session, even if the dungeon
	itself tries to force autorecord. It doesn't prevent you from
	manually selecting record. Actually, there is no reason not to
	use this switch.

  record
	Autorecord a session. Files will be stored in the location as
	specified by the above arguments, such as --gamsav.
	This option is usefull for debugging new dungeons or for
        providing a movie so that others can be amazed at your prowess.

  size=humongous
  size=extremelylarge
  size=extralarge
  size=verylarge
  size=large
  size=small
  Play=file         eg: Play=playfile.log
	Plays a recorded game. Might be reffered to as a "movie".
        You can interrupt the 'movie' with two consecutive Control-c.

  VBLMultiplier=n      eg: VBLMultiplier=99
        n should be in the range of 1 to 99.
        Increases the speed of the realtime clock.  Makes the game
        playback faster.  If multiplier is greater than 1 and less
        than 99 then every third frame is displayed.

  Repeat
        Repeats playback of movie indefinitely.



