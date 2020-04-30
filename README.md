# server-core

## About
Boost::Asio server core static library

## Dependencies
Spdlog  
Boost (tested on 1.72.0)

## Usage
Minimal example:
```c++
#include "servercore/server.h"

int main() {
    //Child is derived from Worker
    std::shared_ptr<Worker> worker = std::make_shared<Child>(4); //worker with 4 threads

    server_core::Server s(worker, 2); //2 threads 
    s.start(1234); //start on port 1234
}
```
