# SPEED TESTER

A simple speed testing tool written in C++. It automatically connects to one of the **nearest MLab servers**, prioritizing **lowest ping**, runs tests over **WebSocket**, and uses the **[Locate V2 API](https://www.measurementlab.net/develop/locate-v2/)** to fetch the available servers.

## Features
- Automatically selects the closest server using MLab's Locate V2 API
- Measures **download**, **upload**, and **latency**
- Communication over **WebSocket**
- Terminal Based

## Dependencies
- C++17 or newer
- cmake
- make
- libcurl
- libssl
- zlib1g
- g++
- [nlohmann/json](https://github.com/nlohmann/json)
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)

## Installation
Before building the project, you need to install the required dependencies.

### Install dependencies (Debian)
```bash
# Update packages
sudo apt update
# Install Core dependencies
sudo apt install git cmake make libcurl4-openssl-dev libssl-dev zlib1g-dev build-essential

#Install IXWebSocket
git clone https://github.com/machinezone/IXWebSocket.git
cd IXWebSocket
mkdir build
cd build
cmake -DUSE_TLS=ON ..
make -j$(nproc)
sudo make install

# Clean Up IXWebSocket unnecessary files
cd ../..
rm -rf IXWebSocket
```

### Install Speed Tester
```
# clone repository
git clone https://github.com/KonstantinosRamm/speed_tester.git
cd speed_tester

# build project
mkdir build
cd build
cmake ..
make
```

### Run
Navigate in build directory and 
```bash
./speedtester
```




