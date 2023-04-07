#!bin/bash
echo "Cloning LLVM 14..."
sleep 2
git clone -b release/14.x https://github.com/llvm/llvm-project.git
echo "Please follow installation of llvm14 from notes w23"
sleep 2
echo "Now going to build the pass"
cd llvm-pass-skeleton
mkdir build
cd build
cmake ..
make 
cd ..
echo "Pass installatin complete!"
