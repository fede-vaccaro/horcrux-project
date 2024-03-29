## Horcrux

For building:

```commandline
mkdir build && cd build
cmake ..
make
```

For launching the server:

It will be hosted on `127.0.0.1:8383` (please check that the port is available before launching it)

```commandline
mkdir out
./server
```

For testing the client:

```commandline
./horcrux save -n 15 ../test/resource/harrypotter.png
```

The Horcruxes will be automatically stored in `build/out` directory.

Also, since the `load` functionality is still missing, I attached a very small tool for merging the
binaries (`join.cpp`).

However, it requires to manually specify a few variable inside (i.e. UUID, number of Horcruxes and output file)

ToDo:

- Implement `load` functionality
- Server function flow more in depth-testing
- Using C++20 Coroutines
- Update the Server to stream the Horcrux to Filesystem
- Improve log messages to be more understandable
- Implement Checksum
- Refactor Server to use generic interface for storage
- Improve Server code quality (decoupling Logic from network implementation)
- Improve Client code quality (to be more modular and clean, now it's essentially a script)
- load-stress test on Server
