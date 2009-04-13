#! /usr/bin/env sh

export USER=`id -un`
export GROUP=`id -gn`

# update python libs
sudo rm -rf site-packages/opensim
sudo cp -R /usr/lib/python2.5/site-packages/opensim site-packages/
sudo chown -R $USER:$GROUP site-packages
rm site-packages/opensim/engine.so
rm site-packages/opensim/engine.so.0
mv site-packages/opensim/engine.so.0.0.0 site-packages/opensim/engine.so

#update opensim lib
sudo rm -rf lib/libsim.so.0
sudo cp /usr/lib/libsim.so.0.0.0 lib/libsim.so.0
sudo chown -R $USER:$GROUP lib
