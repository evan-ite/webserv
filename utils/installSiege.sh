#!/bin/bash

# to install
wget http://download.joedog.org/siege/siege-latest.tar.gz
tar -xzf siege-latest.tar.gz
rm -rf siege-latest.tar.gz
cd siege-*/
./configure --prefix=$HOME/local/siege
make
make install
echo 'export PATH=$HOME/local/siege/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
siege --version

# to run :
# siege -c 10 -t 1M http://localhost:8080