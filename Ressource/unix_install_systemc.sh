#!/bin/bash

tar xzf systemc-2.3.2.tar.gz
cd systemc-2.3.2
./configure --prefix=/usr/local/systemc-2.3.2 --with-unix-layout --enable-debug
make -j 8
sudo make install
