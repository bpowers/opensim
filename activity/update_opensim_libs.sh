#! /usr/bin/env sh

export USER="olpc"
export GROUP=$USER

# update python libs
sudo rm -rf site-packages/opensim
sudo cp -R /usr/lib/python2.5/site-packages/opensim site-packages/
sudo chown -R $USER:$GROUP site-packages

#update opensim lib
sudo rm -rf lib/libsim.so.0
sudo cp /usr/lib/libsim.so.0.0.0 lib/libsim.so.0
sudo chown -R $USER:$GROUP lib
