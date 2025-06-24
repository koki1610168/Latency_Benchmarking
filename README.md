# Latency Benchmarking

A simple C++20 tool to measure network round-trip between a client and server TCP.

---

## Features
- TCP client and echo server
- Mesaure average rount-trip time (RTT) in nanosecond over 1000 messages
- Build with CMake
---

## Project Structure
<pre>
latency-benchmarking/
|---include/
|---src/
|---tests/
|---CMakeLists.txt
|---README.md
|---LICENSE.md
</pre>

---

## Getting Started

### Prerequisites

- C++20 compatible compiler
- CMake 3.14 or higher
- POSIX OS recommended

### Build Instructions
```bash
git clone https://github.com/koki1610168/Latency_Benchmarking.git
cd Latency_Benchmarking

mkdir build && cd build
cmake ..
make
```

### Running
Start the server (in terminal 1):
```build
./server
```

Start the client (in terminal 2):
```build
./client
```
The client will output RTT in nanosecond for each message. \
It also outputs an average RTT in nanosecond.

### Sample Output
```
RTT: 37300 ns
RTT: 39209 ns
    .
    .
    .
RTT: 36325
Average RTT over 1000 messages: 36003.6 ns
```
## License
MIT License 
MIT License © 2025 Koki Yahata


