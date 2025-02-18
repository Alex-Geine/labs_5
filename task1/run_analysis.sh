mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ..


mkdir result
cd result

../build/analyse < 4x4_cfg.txt
../build/analyse < 5x5_cfg.txt
../build/analyse < 6x6_cfg.txt

gnuplot plt
