#!bin/bash


echo "Now going to build the pass"

mkdir build
cd build
cmake ..
make 
cd ..
echo "Pass installatin complete!"
