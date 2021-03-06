#!/bin/bash

echo rvm_autoupdate_flag=0 >> ~/.rvmrc
if [ "$deploy" = true ]; then
sudo apt-get update
sudo apt-get install -y -qq \
	curl \
	wget \
	git \
	zsync \
	xz-utils \
	libjson-perl \
	libwww-perl \
	lsb-release
fi

case "$TRAVIS_OS_NAME" in
linux)
	sudo apt-get update
	sudo apt-get install -y \
		curl \
		libosmesa6-dev \
		libgl1-mesa-dev \
		libglu1-mesa-dev \
		libsdl1.2-dev \
		libsdl-image1.2-dev \
		libusb-dev \
		libusb-1.0-0-dev \
		libudev-dev \
		zsync \
		xz-utils \
		libjson-perl \
		libwww-perl \
		git \
		autoconf \
		automake \
		build-essential \
		gcc-4.8 \
		g++-4.8 \
		libx11-dev \
		lsb-release

		rm /usr/bin/gcc
		rm /usr/bin/g++
		ln -s /usr/bin/gcc-4.8 /usr/bin/gcc
		ln -s /usr/bin/g++-4.8 /usr/bin/g++
		gcc -v
		export CC=/usr/bin/gcc-4.8
		export CXX=/usr/bin/g++-4.8
	;;

osx)
	curl --get https://www.libsdl.org/projects/SDL_image/release/SDL_image-1.2.12.dmg --output SDL_image.dmg
	curl --get https://www.libsdl.org/release/SDL-1.2.15.dmg --output SDL.dmg
	curl --get https://www.libsdl.org/release/SDL2-2.0.7.dmg --output SDL2.dmg
	curl --get https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.2.dmg --output SDL2_image.dmg
	mkdir -p ~/Library/Frameworks
	
	mountpoint=~/sdltmp
	mkdir "$mountpoint"

	hdiutil attach -mountpoint "$mountpoint" -readonly SDL.dmg
	cp -R "$mountpoint/SDL.framework" ~/Library/Frameworks
	hdiutil detach "$mountpoint"

	hdiutil attach -mountpoint "$mountpoint" -readonly SDL_image.dmg
	cp -R "$mountpoint/SDL_image.framework" ~/Library/Frameworks
	hdiutil detach "$mountpoint"

	hdiutil attach -mountpoint "$mountpoint" -readonly SDL2.dmg
	cp -R "$mountpoint/SDL2.framework" ~/Library/Frameworks
	hdiutil detach "$mountpoint"

	hdiutil attach -mountpoint "$mountpoint" -readonly SDL2_image.dmg
	cp -R "$mountpoint/SDL2_image.framework" ~/Library/Frameworks
	hdiutil detach "$mountpoint"

	rmdir "$mountpoint"
	
	rm -rf src/Unix/MacOSX/SDL.Framework
	rm -rf src/Unix/MacOSX/SDL_image.Framework
	rm -rf src/Unix/MacOSX/SDL2.Framework
	rm -rf src/Unix/MacOSX/SDL2_image.Framework
	ln -s ~/Library/Frameworks/SDL.Framework src/Unix/MacOSX/SDL.Framework
	ln -s ~/Library/Frameworks/SDL_image.Framework src/Unix/MacOSX/SDL_image.Framework
	ln -s ~/Library/Frameworks/SDL2.Framework src/Unix/MacOSX/SDL2.Framework
	ln -s ~/Library/Frameworks/SDL2_image.Framework src/Unix/MacOSX/SDL2_image.Framework
	
	# we must install the macports version of the dependencies,
	# because the brew packages are not universal
	mkdir -p src/Unix/MacOSX/external
	for i in gmp/gmp-6.1.2_3+universal.darwin_16.i386-x86_64.tbz2 \
		mpfr/mpfr-4.0.2_1.darwin_15.x86_64.tbz2 \
		jpeg/jpeg-9d_0+universal.darwin_16.i386-x86_64.tbz2; do
		f=`basename $i`
		curl --get https://tho-otto.de/download/macports/$f --output - | bzip2 -dc | tar -C src/Unix/MacOSX/external --strip-components=3 -xf -
	done
	# replace symlinks; with XCode 11, only the symlinks will be copied
	# to the application folder and can't be signed locally
	cwd=`pwd`
	cd src/Unix/MacOSX/external/lib
	rm -f libjpeg.dylib
	for i in .7 .8 .9; do
		test -f libjpeg${i}.dylib && ln libjpeg${i}.dylib libjpeg.dylib
	done
	rm -f libgmp.dylib
	for i in .10; do
		test -f libgmp{$i}.dylib && ln libgmp{$i}.dylib libgmp.dylib
	done
	rm -f libgmpxx.dylib
	for i in .4; do
		test -f ln libgmpxx${i}.dylib && ln libgmpxx${i}.dylib libgmpxx.dylib
	done
	rm -f libmpfr.dylib
	for i in .4; do
		test -f ln libmpfr${i}.dylib && ln libmpfr${i}.dylib libmpfr.dylib
	done
	cd "$cwd"
	;;

*)
	exit 1
	;;
esac
