#!/usr/bin/env bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]
then 
	brew update
	brew install glfw3 
else
	# Install GLFW $GLFW_VERSION
	wget -O glfw.zip "https://github.com/glfw/glfw/releases/download/$GLFW_VERSION/glfw-$GLFW_VERSION.zip"
	unzip glfw.zip
	cd glfw-"$GLFW_VERSION"
	cmake -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX="$HOME"
	make
	sudo make install
	cd ..
fi
