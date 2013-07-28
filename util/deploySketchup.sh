#!/bin/bash

# deploySketchup.sh
# Author: Robin Willis
# http://robincwillis.com
# I like to serve my Sketchup Plugin UI from a remote server. 
# That way I can push updates to the server and they will 
# be reflected across all installations of the plugin.
# However this leads to different versions, 
# like a local one for testing versus a production one for release.
# It became to hard to keep track of everything so time to automate
# Script for setting up and deploying Sketchup plugin into two parts, 
# the server hosted UI, and the distribution ruby for installation 


###################
#### Variables ####
###################

SRC_DIR=$HOME'/'
README_DIR=$HOME'/'

CLIENT_BUILD_DIR=$HOME'/'
CLIENT_DEPLOY_DIR=$HOME'/'

SERVER_BUILD_DIR=$HOME'/'
SERVER_DEPLOY_DIR=$HOME'/'

PLUGIN_NAME=''
PLUGIN_NAMESPACE=''
PACKAGE_NAME=''
##################
##### Client #####
##################

#clean out directories
rm -rfv "$CLIENT_BUILD_DIR/"*
rm -rfv "$CLIENT_DEPLOY_DIR/"*

mkdir "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE"

#copy to client build dir

cp "$SRC_DIR/$PLUGIN_NAME" "$CLIENT_BUILD_DIR/$PLUGIN_NAME"
cp "$SRC_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAMESPACE.rb" "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAMESPACE.rb"
cp "$SRC_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME" "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME"
cp -r "$SRC_DIR/$PLUGIN_NAMESPACE/bin" "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/bin"

#switch to load html from server
mv "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME" "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME.src.rb"
sed 's/#@dialog.set_url/@dialog.set_url/g' "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME.src.rb" > "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME"

#scramble ruby
Scrambler "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME"
rm "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME"
rm "$CLIENT_BUILD_DIR/$PLUGIN_NAMESPACE/$PLUGIN_NAME.src.rb"

#compress into rbz
cd "$CLIENT_BUILD_DIR"
zip -r "$PACKAGE_NAME.zip" *
mv "$CLIENT_BUILD_DIR/$PACKAGE_NAME.zip" "$CLIENT_DEPLOY_DIR/$PACKAGE_NAME.rbz"

#package into zip with readme
cp "$README_DIR/Readme.txt" "$CLIENT_DEPLOY_DIR/Readme.txt"
cd "$CLIENT_DEPLOY_DIR"
zip -r "$PACKAGE_NAME.zip" *

#copy into deploy directory

##################
##### Server #####
##################

echo "clean out server directories..."
rm -rf "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE"
rm -rf "$SERVER_DEPLOY_DIR/$PLUGIN_NAMESPACE"

echo "copy to server build dir..."
cp -r "$SRC_DIR/$PLUGIN_NAMESPACE" "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE"
rm -rf "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/bin"

echo "uglify and remove main.js..."
uglifyjs -o "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/js/main.min.js" "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/js/main.js"
rm "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/js/main.js"

echo "change link to main.min.js..."
mv "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/dialog.html" "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/dialog_src.html"
sed 's/main.js/main.min.js/g' "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/dialog_src.html" > "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE/dialog.html"

echo "final copy to deploy dir..."
cp -r "$SERVER_BUILD_DIR/$PLUGIN_NAMESPACE" "$SERVER_DEPLOY_DIR/$PLUGIN_NAMESPACE"
