#! /usr/bin/env sh

export CUR_USER=`id -un`
export CUR_GROUP=`id -gn`
export USER=root
export GROUP=admin
export SRC_DIR=`pwd`/..

# make the bundle easier to work with
sudo chown -R $CUR_USER:$CUR_GROUP Model.app

cd Model.app/Contents/Resources

# update python libs
rm -rf ./opensim
cp -R /Library/Python/2.5/site-packages/opensim ./

# requires libs be built with -headerpad_max_install_names
install_name_tool ./opensim/engine.so -change /usr/lib/libsim.0.dylib \
                  @executable_path/../Resources/lib/libsim.0.dylib

rm -f ./lib/*
cp /usr/lib/libsim.* ./lib/

cp $SRC_DIR/src/opensim-gtk ./opensim-gtk.py
cp $SRC_DIR/opensim-main.glade ./opensim-main.glade

sudo chown -R $USER:$GROUP Model.app
