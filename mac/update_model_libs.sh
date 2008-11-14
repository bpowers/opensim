#! /usr/bin/env sh

export CUR_USER=`id -un`
export CUR_GROUP=`id -gn`
export USER=$CUR_USER
export GROUP=admin
export SRC_DIR=`pwd`/..

echo "making the bundle easier to work with"
sudo chown -R $CUR_USER:$CUR_GROUP Model.app

cd Model.app/Contents/Resources

echo "updating python libs"
rm -rf ./opensim
cp -R /Library/Python/2.5/site-packages/opensim ./

# requires libs be built with -headerpad_max_install_names
echo "fixing engine's mach-o loads"
install_name_tool -change /usr/lib/libsim.0.dylib \
                  @executable_path/../Resources/lib/libsim.0.dylib \
                  ./opensim/engine.so

echo "updating opensim libs"
rm -rf ./lib
mkdir ./lib
cp /usr/lib/libsim.* ./lib/
rm ./lib/libsim.a

echo "updating opensim-gtk"
cp $SRC_DIR/src/opensim-gtk ./opensim-gtk.py
cp $SRC_DIR/src/opensim-main.glade ./opensim-main.glade

echo "resetting app privledges"
cd ../../..
sudo chown -R $USER:$GROUP Model.app
