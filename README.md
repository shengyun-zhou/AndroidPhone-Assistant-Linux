# AndroidPhone-Assistant-Linux
A simple Android phone assistant on Linux, including the PC client and Android server.You can use the PC client of the Android phone assistant to backup your android phone's short messages and contacts, and manage your apps on your phone easily.


## License
PC Client:GPLv3

Android Server:GPLv3

## Build
### Android server(src/Android-Server)
You can use `Android Studio` to build it directly.

### PC client(src/PC-Client)
+ Install [CMake](http://www.cmake.org/download).
+ Install the dependence developing libraries:

| Library | Package Name on Ubuntu |
|---------|------------------------|
| Qt 5 | qt5-default |
| Qt5svg5 | libqt5svg5-dev|
| GTK+2.0 | libgtk2.0-dev |
| SQLite3 | libsqlite3-dev |
| GKSU2 | libgksu2-dev |

+ Switch to the directory `src/PC-Client` and execute the following commands to use `CMake` to generate project files and make this project. 
```
mkdir build
cd build
cmake .. -G "${GENERATOR}" 
cmake --build .
```

The `${GENERATOR}` represents the generator which you want to use to generate the project files.You can execute the following command to find the available generators on your platform，or you can omit it to use the default generator.

```
cmake --help
``` 