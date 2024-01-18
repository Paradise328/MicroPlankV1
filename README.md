# MicroSurgery Robot

# project structure

you must construct the project as following structure:

```
mikroPlack root folder
|- build
|- bin
|- src
    |- lib
    |- ui
    |- module
    |- external
    |- utils
|- history
|- log
|- gitignore
|- README.md
|- CMAkeLists.txt
```

# instructions


## some commands



to start cifx card:
sudo modprobe uio_netx

to start com card:
sudo emp2init

## build
at project top dir, run in terminal:

```
mkdir build
cd build
cmake ..
make
```

if you run on your own computer which doesnt contain all the peripheral devices, go to applications, comment the subdirectories which you doesnt need.

## run:
the executable programmes are in build/run/.For example, at build directory, run:
```
./run/LiftColumn 
```
to call the LiftColumn program

# Update Log

# Todo List
