## Horcrux

For building:

```commandline
mkdir build && cd build
cmake ..
make
```

For launching the server:

It will be hosted on `127.0.0.1:8080` (check that the port is available before launching it)
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


