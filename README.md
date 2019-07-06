# TACHYON

Game engine based on Faster Than Light's "Dungeon Master". It can play
both FTL's _Dungeon Master_ and _Chaos Strikes Back_, plus homebrew DM
games including _Conflux III_, _The Black Flame_, _The Towers of
Chaos_, _3D2D1D_, and _5DMaze_. While primarily tested under
GNU/Linux, it is SDL based, and should work in Apple's MacOS, and
Microsoft Windows.

# Wait, this is just CSBwin!

Essentially, yes. TACHYON is based on [Paul R.
Stevens](http://www.dianneandpaul.net/)'s CSBwin. Although Mr. Stevens
no longer wishes to maintain CSBwin, he is due all the credit for the
heroic — verging on crazy — effort of porting the original Atari ST
game to C++. If you wish, you can download the unmodified `CSBwin` at
http://www.dianneandpaul.net/CSBwin/.

Mr. Stevens has always said the source code is "free for anyone to do
with as they like", so — with his consent via e-mail — I have
interpreted that to mean the MIT License. [But, see legal note about
the data files below.]

# So what's different?

Nothing in terms of game play. "TACHYON" is just what I'm calling this
little repository in which I can squash bugs, cleanup code, and add my
wishlist features. The name comes from the theoretical physics and is
a tip of the hat to Faster Than Light, the now defunct publisher of
*Dungeon Master* and its sequel *Chaos Strikes Back*.

# Prerequisites

To play, you only need the binary file, which I hope to make available via github. 

To compile, you will need SDL2 and glib2. For example, in Debian GNU/Linux and Ubuntu you could do this:

    apt install libsdl2-dev libglib2.0-dev

# Compilation

Unix systems should be able to simply run `make`

Usage:

    make                Compile tachyon with optimization
    make debug          Compile with debugging + pointer bounds checking
    make run            Compile and run the debug version, using DM data.
    make clean          Remove temporary files (*.o and core)
    make archive        Create a tar of the source code in the parent directory.
                        (Filename will include version number and today's date.)

# Legal Note about data files

While the code Mr. Stevens wrote may be free software, technically the
data files are almost surely under a restrictive copyright. Sticklers
for getting things right, such as The Debian Project, should package
this program without the `games/dm` or `games/csb` directories.

If FTL still existed, we would be able to ask them to release the
source data files under a permissive license, the same as [other
companies have done](https://archive.org/details/gamesourcecode). I
believe they would do so happily.

Unfortunately, non-existant entities can't grant permission. (Yet
another reason we need to reset copyright duration for computer
software to the original length specified in the U.S. Constitution.)

However, if anyone reading this believes they own the copyright to
Dungeon Master or Chaos Strikes Back, please do contact me.
