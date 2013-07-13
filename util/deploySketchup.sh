#!/bin/bash

# deploySketchup.sh
# 
# Script for setting up and deploying the Sunglass Sketchup plugin 
# Became to hard to keep track of everything so time to automate

##### Variables ####


#SRC_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/SRC'
#SERVER_BUILD_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Package/Server_Build'
#SERVER_DEPLOY_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/src'

SRC_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/SRC'
README_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Build/Readme'

CLIENT_BUILD_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Build/Client_Build'
CLIENT_DEPLOY_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Build/Client_Deploy'


SERVER_BUILD_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Build/Server_Build'
SERVER_DEPLOY_DIR=$HOME'/Documents/GIT/SunglassPlugins/SG-SketchupHook/real_src/Build/Server_Deploy'

##################
##### Client #####
##################

#clean out directories
rm -rfv "$CLIENT_BUILD_DIR/"*
rm -rfv "$CLIENT_DEPLOY_DIR/"*

mkdir "$CLIENT_BUILD_DIR/sg"

#copy to client build dir

cp "$SRC_DIR/sunglass.rb" "$CLIENT_BUILD_DIR/sunglass.rb"
cp "$SRC_DIR/sg/sg.rb" "$CLIENT_BUILD_DIR/sg/sg.rb"
cp "$SRC_DIR/sg/sunglass.rb" "$CLIENT_BUILD_DIR/sg/sunglass.rb"
cp -r "$SRC_DIR/sg/bin" "$CLIENT_BUILD_DIR/sg/bin"

#switch to load html from server
mv "$CLIENT_BUILD_DIR/sg/sunglass.rb" "$CLIENT_BUILD_DIR/sg/sunglass_src.rb"
sed 's/#@dialog.set_url/@dialog.set_url/g' "$CLIENT_BUILD_DIR/sg/sunglass_src.rb" > "$CLIENT_BUILD_DIR/sg/sunglass.rb"

#scramble ruby
Scrambler "$CLIENT_BUILD_DIR/sg/sunglass.rb"
rm "$CLIENT_BUILD_DIR/sg/sunglass.rb"
rm "$CLIENT_BUILD_DIR/sg/sunglass_src.rb"

#compress into rbz
cd "$CLIENT_BUILD_DIR"
zip -r "SunglassForSketchup.zip" *
mv "$CLIENT_BUILD_DIR/SunglassForSketchup.zip" "$CLIENT_DEPLOY_DIR/SunglassForSketchup.rbz"

#package into zip with readme
cp "$README_DIR/Readme.txt" "$CLIENT_DEPLOY_DIR/Readme.txt"
cd "$CLIENT_DEPLOY_DIR"
zip -r "SunglassForSketchup.zip" *

#copy into deploy directory
#cp "$CLIENT_BUILD_DIR/SungassForSketchup.zip" "$CLIENT_DEPLOY_DIR/SungassForSketchup.zip"

##################
##### Server #####
##################

echo "clean out server directories..."
rm -rf "$SERVER_BUILD_DIR/sg"
rm -rf "$SERVER_DEPLOY_DIR/sg"

echo "copy to server build dir..."
cp -r "$SRC_DIR/sg" "$SERVER_BUILD_DIR/sg"
rm -rf "$SERVER_BUILD_DIR/sg/bin"

echo "uglify and remove main.js..."
uglifyjs -o "$SERVER_BUILD_DIR/sg/js/main.min.js" "$SERVER_BUILD_DIR/sg/js/main.js"
rm "$SERVER_BUILD_DIR/sg/js/main.js"

echo "change link to main.min.js..."
mv "$SERVER_BUILD_DIR/sg/dialog.html" "$SERVER_BUILD_DIR/sg/dialog_src.html"
sed 's/main.js/main.min.js/g' "$SERVER_BUILD_DIR/sg/dialog_src.html" > "$SERVER_BUILD_DIR/sg/dialog.html"

echo "final copy to deploy dir..."
cp -r "$SERVER_BUILD_DIR/sg" "$SERVER_DEPLOY_DIR/sg"
