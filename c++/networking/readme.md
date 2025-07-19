## Dependencies

1. [asio](https://think-async.com/Asio/)
2. [protobuf-v31.1](https://github.com/protocolbuffers/protobuf) - [tuturial](https://mengbaoliang.cn/archives/92200/)
3. [Imgui](https://github.com/ocornut/imgui)
4. [glfw](https://www.glfw.org/download.html)

## Setup

```
E:.
├─build
├─dist
├─libs
│  ├─asio-1.30.2
│  ├─imgui
│  ├─glfw-3.3.8.bin.WIN64
│  └─protobuf-v31.1-g++
└─modules
    ├─net-client
    ├─net-common
    ├─net-server
    ├─protobuf-types
    └─simple-example
```

1. create `libs` which contain asio 
2. create `dist` folder for having the output file
3. use cmake build


![img](./public/Sample.png)