# SPEED TESTER

A simple speed testing tool written in C++. It automatically connects to one of the **nearest MLab servers**, prioritizing **lowest ping**, runs tests over **WebSocket**, and uses the **Locate V2 API** to fetch the available servers.

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


