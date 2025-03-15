#!/bin/bash

# Create test directory if it doesn't exist
mkdir -p test_build

# Compile and run basic test
echo "Running basic test..."
g++ -std=c++11 -o test_build/test_basic test/test_basic/test_basic.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c
./test_build/test_basic

# Compile and run Station test
echo "Running Station test..."
g++ -std=c++11 -o test_build/test_station test/test_station/test_station.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -Itest/mocks
./test_build/test_station

# Compile and run WaveBandManager test
echo "Running WaveBandManager test..."
g++ -std=c++11 -o test_build/test_wave_band_manager test/test_wave_band_manager/test_wave_band_manager.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -Itest/mocks
./test_build/test_wave_band_manager

# Compile and run SpeedManager test (Arduino version)
echo "Running SpeedManager test (Arduino)..."
g++ -std=c++11 -o test_build/test_speed_manager test/test_speed_manager/test_speed_manager.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -Itest/mocks
./test_build/test_speed_manager

# Compile and run SpeedManager test (ESP-IDF version)
echo "Running SpeedManager test (ESP-IDF)..."
g++ -std=c++11 -o test_build/test_speed_manager_esp test/test_speed_manager_esp/test_speed_manager_esp.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -DESP_PLATFORM -I$IDF_PATH/components/driver/include -I$IDF_PATH/components/esp_common/include -I$IDF_PATH/components/hal/include
./test_build/test_speed_manager_esp

# Morse timings test
echo "Running Morse timings test..."
g++ -std=c++11 test/test_morse_timings/test_morse_timings.cpp test/mocks/Config.cpp -o test_build/test_morse_timings -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -Itest/mocks
./test_build/test_morse_timings

# Add more tests here as needed
# For example:
# g++ -o test_build/test_station test/test_station/test_station.cpp -I.pio/libdeps/test/Unity/src .pio/libdeps/test/Unity/src/unity.c -Itest/mocks -Isrc
# ./test_build/test_station

echo "All tests completed." 