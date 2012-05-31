# This is th4e configuration area of this script, which has all the bits users can set. You really shouldn't need to though.
		_autodepencencies=true
		_threads=0 # Leave at zero to automatically make it 2 times your number of cpu cores, or change as desired to suit performance.

     
     
                #!/bin/sh
                LANG=C
                _gittrunk=http://anongit.kde.org/calligra/calligra-latest.tar.gz
                _gitdir=calligra
                _startdir=`pwd`
                _cbversion=1.1.5



#########################################################################################################################
#                                                                                                                       #
#   I really wouldn't recommend changing anything below here, unless you're some kind of git wielding,code-ninja-bot.   #
#                                                                                                                       #
#########################################################################################################################



#Set number of cores to be used
  if  [ "$_threads" == 0 ]; then
    _threads=$((`cat /proc/cpuinfo | grep processor | wc -l`*2))	
  fi

# Be purdy :)     
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo "              _"
            echo "           ,d88P'              ,8     op   ,o."
            echo "        ,o8P                  d8'   ,8P     Y8"
            echo "      ,d8P'                  d8'   ,8F        ="
            echo "     d8P'           _u,     ,8'    dP    _,      ___ _   __           ._."
            echo "    88           ,o8P  P   ,8[    dP     88    d88 888  ]8L,ooo._  od8P8Y"
            echo "   88          ,d8 ,,oop   d8    ,8[    d8P  ,8P _o88[  88P      d8P _ooo."
            echo "   88         d88odP Y8b  ,8[   ,8P   ,=8P  ,88o8P dP  d8F     _88boP  88"
            echo "   Y8b.._._,o=PPP      P=P78boP  8bopP YP    PP^^ d8  ,8P       PP^^    YPY"
            echo "     ^^^^^^^                ^^    ^^             d88"
            echo "                                                d8P"
            echo "                                           /   d8 "
            echo "                                          b _o8P"
            echo "                                          ^^^"
            echo ""
            echo "              Builder version "$_cbversion
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""

# Install dependencies (and remove conflicting distro packages) on supported distros
	     if  [ "$_autodepencencies" == "true" ]; then
		echo "Checking if this is a distro I can automatically get the dependencies for you...";
		echo "You'll be asked for your root password to update needed software and"
		echo ">>> UNINSTALL ANY CURRENT KRITA / CALLIGRA INSTALL YOU HAVE <<<"
		echo "from elsewhere (they conflict with the compiled versions)."
		echo "IF YOU DON'T WANT TO CONTINUE WITH THIS PUSH Ctrl+C TO SKIP INSTALLING DEPENDENCIES NOW."
            echo ""
            echo ""
		if cat /etc/issue | grep Ubuntu > /dev/null ; then
# 		  if cat /etc/issue | grep "Ubuntu 11.10" > /dev/null ; then echo "WARNING: Ubuntu 11.04 / 11.10 (and distros based on them) are currently known to have problems when using a Wacom (no pressure, strange lines drawn). There are workarounds for both issues. Please see: https://bugs.launchpad.net/ubuntu/natty/+source/qt4-x11/+bug/762938 and https://bugs.launchpad.net/ubuntu/+source/xorg-server/+bug/799202 .";
# 		  fi
		      echo "Using Ubuntu settings." && sudo apt-get purge krita* koffice* karbon* && sudo apt-get install cl-fftw3 liblcms2 liblcms2-dev liblcms2-util git cmake xserver-xorg-input-wacom oxygen-icon-theme kde-runtime wget && sudo apt-get build-dep krita
		else
		  if cat /etc/issue | grep Fedora > /dev/null ; then
		      echo "Using Fedora settings." && su -c'yum remove koffice* calligra* && yum-builddep koffice && yum install git wget gcc gcc-c++'
		  else
		    if cat /etc/issue | grep openSUSE > /dev/null ; then
		      echo "Using openSuse settings." && sudo zypper remove koffice* calligra* && sudo zypper install git cmake gcc gcc-c++ libkde4-devel libeigen2-devel libexiv2-devel liblcms2-devel wget
		    else
			if cat /etc/issue | grep "Mint 11" > /dev/null ; then
				echo "Using Linux Mint 11 settings" && sudo apt-get install cmake git kdelibs5-dev zlib1g-dev libpng12-dev libboost-dev liblcms1-dev libeigen2-dev libexiv2-dev pstoedit libfreetype6-dev libglew1.5-dev libfftw3-dev libglib2.0-dev libopenexr-dev libtiff4-dev libjpeg62-dev xserver-xorg-input-wacom oxygen-icon-theme kde-runtime
			else
			  if cat /etc/issue | grep Chakra > /dev/null ; then
			      echo "Using Chakra Linux settings. To install the needed dependencies, just enter the super user password and say yes to the default installer settings (ie Enter and then Y)" && sudo pacman -S base-devel git cmake automoc4 boost eigen opengtl docbook-xml docbook-xsl xf86-input-wacom
			    else echo "Looks like you're not using a distro supported by this script (ie *Ubuntu, OpenSuse, Fedora). That's fine, but you need to make sure the necessary dependencies are installed yourself (usually whatever is needed to build koffice plus git). Once you have done that, please change the first line of this script to _autodepencencies=false and run this script again. NOTE: Using this method you will not get automatic updates to this script or a log file (which can help us find and fix problems for you)." && exit
			    fi
			  fi
		    fi
		  fi
		fi
	     fi
sudo -K # Revoke all sudo rights so we don't accidentally do anything nasty

            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
# Update the self updater (welcome to recursive city...)
echo "Jacking into the Matrix..."
rm ./calligra-creative_build-script.sh*
wget -O calligra-creative_build-script.sh.tar.gz http://krita.org/builder/calligra-creative_build-script.sh.tar.gz

# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi  

tar -xzvf ./calligra-creative_build-script.sh.tar.gz
rm ./calligra-creative_build-script.sh.tar.gz
echo "Red pill taken..."
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""
            echo ""


# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi     
     
                if [ -d ./$_gitdir ]; then
                    (cd ./$_gitdir/$_gitdir-src && git pull)
                else
                 if [ -f ./calligra-latest.tar.gz ]; then
                (echo "Extracting source..." && tar -xzf calligra-latest.tar.gz && echo "Renaming source folder..." && mv ./calligra ./$_gitdir-src && echo "Creating base folder..." && mkdir ./$_gitdir && echo "Moving source into sub-folder..." && mv ./$_gitdir-src ./$_gitdir/ && echo "Initialising repository..." && cd ./$_gitdir/$_gitdir-src && ./initrepo.sh && cd ./.git && git remote set-url origin git://anongit.kde.org/calligra && echo "Making build and install directories..." && mkdir -p $_startdir/$_gitdir/$_gitdir-inst && mkdir -p $_startdir/$_gitdir/$_gitdir-build )
                 else
                (echo "Downloading Calligra source code (may take a while!)" && wget $_gittrunk -O calligra-latest.tar.gz && echo "Extracting source..." && tar -xzf calligra-latest.tar.gz && echo "Renaming source folder..." && mv ./calligra ./$_gitdir-src && echo "Creating base folder..." && mkdir ./$_gitdir && echo "Moving source into sub-folder..." && mv ./$_gitdir-src ./$_gitdir/ && echo "Initialising repository..." && cd ./$_gitdir/$_gitdir-src && ./initrepo.sh && cd ./.git && git remote set-url origin git://anongit.kde.org/calligra && echo "Making build and install directories..." && mkdir -p $_startdir/$_gitdir/$_gitdir-inst && mkdir -p $_startdir/$_gitdir/$_gitdir-build )
                 fi
                fi
# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi
                echo "Setting up environment"
                cd $_startdir/$_gitdir/$_gitdir-inst      #set up your variables so Calligra will find it's libraries when you run it



# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi
     
     
                cd $_startdir/$_gitdir/$_gitdir-src

# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi
     
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo "Now witness the magic of open source cranked up to "$_threads" as you build your very own copy of Calligra..."
     
                calligra_version=`git describe --tags`

                cd ../$_gitdir-build
# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi
     
#                cmake ../$_gitdir-src -DCMAKE_BUILD_TYPE=RELWITHDEBINFO -DCMAKE_INSTALL_PREFIX=../$_gitdir-inst
                cmake ../$_gitdir-src -DCREATIVEONLY=ON -DCMAKE_BUILD_TYPE=RELWITHDEBINFO -DCMAKE_INSTALL_PREFIX=../$_gitdir-inst


# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi
     
                echo "Quick! To the Batmobile!"
                make -j$_threads  #compile with your number of cores times two unless multicore was turned off

# Test for errors and quit if anything is wrong
errorcode=$?; if [[ $errorcode != 0 ]] ; then echo "Oh noes! I've hit a bash error number "$errorcode". Please look just above this text and see if you can see anything that looks like it may be an error message. You can then tell us about it at http://forum.kde.org/krita or on IRC at #krita.";exit $errorcode;fi

                echo "Atomic batteries to power..."
                make install -j$_threads  #install with your number of cores times two unless multicore was turned off
                echo "Turbines to speed..."
                echo ""
                echo ""
                echo ""




# Do they have Calligra paths set in kdeglobals? If not add it...
KDELOCALCONFS=$(kde4-config --localprefix)
if ! grep -Fq '[Directories]' $KDELOCALCONFS'share/config/kdeglobals'; then
 		echo '' >> $KDELOCALCONFS'share/config/kdeglobals'
		echo '[Directories]' >> $KDELOCALCONFS'share/config/kdeglobals'
 		echo 'prefixes='$_startdir'/'$_gitdir'/'$_gitdir'-inst' >> $KDELOCALCONFS'share/config/kdeglobals'
fi





# Create a file to manually reset the enbvironment variables if needed
if [ -f $_startdir/set-calligra-paths.sh ]; then
		echo 'set-calligra-paths.sh exists already. Skipping creating it.'
else
		echo 'Creating set-calligra-paths.sh'
		echo 'export KDEDIRS='$(kde4-config --prefix) >> $_startdir/set-calligra-paths.sh
                echo 'export KDEDIRS='$KDEDIRS':'$_startdir'/'$_gitdir'/'$_gitdir'-inst' >> $_startdir/set-calligra-paths.sh
                echo 'export KDEDIR='$KDEDIRS >> $_startdir/set-calligra-paths.sh
                echo 'export PATH='$PATH':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/bin' >> $_startdir/set-calligra-paths.sh
                echo 'export XDG_DATA_DIRS='$XDG_DATA_DIRS':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/share' >> $_startdir/set-calligra-paths.sh
                echo 'export XDG_DATA_HOME='$XDG_DATA_HOME':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/share' >> $_startdir/set-calligra-paths.sh
		echo 'kbuildsycoca4' >> $_startdir/set-calligra-paths.sh
		chmod +x $_startdir/set-calligra-paths.sh
fi




# Set the environment variables for this session, just in case they're not already set.
		echo 'setting environment variables for current session.'
		export KDEDIRS=$(kde4-config --prefix)
                export KDEDIRS=$KDEDIRS':'$_startdir'/'$_gitdir'/'$_gitdir'-inst'
                export KDEDIR=$KDEDIRS
                export PATH=$PATH':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/bin'
                export XDG_DATA_DIRS=$XDG_DATA_DIRS':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/share'
                export XDG_DATA_HOME=$XDG_DATA_HOME':'$_startdir'/'$_gitdir'/'$_gitdir'-inst/share'

kbuildsycoca4



### Install menu items for Gnome desktop

#Remove old entries
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
echo "Enter your admin password if you would like menu items for your apps to be created";
echo "(or you can just click Ctrl+C when asked for your password from now on if you wish to skip menu entries.)";
sudo rm /usr/share/applications/kde4/krita.desktop
sudo rm /usr/share/applications/kde4/karbon.desktop

#Move compiled menu items to a temp folder for batch processing
mkdir /tmp/calligra-creative-builder
sudo cp $_startdir"/"$_gitdir"/"$_gitdir"-inst/share/applications/kde4/krita.desktop" /tmp/calligra-creative-builder/
sudo cp $_startdir"/"$_gitdir"/"$_gitdir"-inst/share/applications/kde4/karbon.desktop" /tmp/calligra-creative-builder/


# Set menu items application and icon paths
echo "Set those paths, Jeeves!";
find /tmp/calligra-creative-builder -type f -exec sed -i "s,krita %U,$_startdir/$_gitdir/$_gitdir-inst/bin/krita %U,g" {} \;
find /tmp/calligra-creative-builder -type f -exec sed -i "s,karbon %u,$_startdir/$_gitdir/$_gitdir-inst/bin/karbon %U,g" {} \;

find /tmp/calligra-creative-builder -type f -exec sed -i "s,Icon=krita,Icon=$_startdir/$_gitdir/$_gitdir-inst/share/icons/hicolor/32x32/apps/krita.png,g" {} \;
find /tmp/calligra-creative-builder -type f -exec sed -i "s,Icon=karbon,Icon=$_startdir/$_gitdir/$_gitdir-inst/share/icons/hicolor/32x32/apps/karbon.png,g" {} \;
echo "What ho!";


#Move menu items into Gnome menu folder and cleanup
sudo cp /tmp/calligra-creative-builder/* /usr/share/applications/kde4/
sudo rm -Rf /tmp/calligra-creative-builder
sudo -K # Revoke all sudo rights so we don't accidentally do anything nasty


                echo "Let's go!!!!"
                cd ../$_gitdir-inst/bin
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""
                echo ""

                echo 'You have succesessfully built (either now or in the past) the following apps:'
                ls
                echo ""
                echo "Version:"
                echo $calligra_version
                echo "(Format is: Version number - revisions since the version number - git hash)"
		echo "You can update these apps to the latest (unstable) code any time by just running this script again."
                echo ""
                echo "Krita and karbon need to be run from the menu (under Graphics) or directly from the /calligra/calligra-inst/bin folder."
		echo "Wed love to see what you create over at the Krita forums! ( http://forum.kde.org/krita )"


# Release checklist just for me :)
# Check executible bit is set
# Creative only
# 0 threads
