# indi_buinskii_focuser

# Install deps for build
```
sudo apt-get install -y libnova-dev libcfitsio-dev libusb-1.0-0-dev zlib1g-dev libgsl-dev build-essential cmake git libjpeg-dev libcurl4-gnutls-dev libtiff-dev libfftw3-dev
```

# Install indi core
```
git clone git@github.com:indilib/indi.git
cd ./indi
mkdir -p ./build/indi-core
cd ./build/indi-core
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ../../
make -j4
sudo make install
```

# Install
```
git clone git@github.com:buinskii/indi_buinskii_focuser.git
cd indi_buinskii_focuser
mkdir build
cd build
cmake ..
make
sudo make install
```