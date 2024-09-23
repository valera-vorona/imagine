# imagine
Simple Image Viewer

**Note! This tool goes without any guarantee! Use it at your risk without any warranty.**

License: [MIT License]- see ``LICENSE`` for more information.

# HOW TO INSTALL
### from source code

## Debian, Ubuntu, Mint

and other Debian derivatives.

* You should have the following packeges installed:

 build-essential git git-lfs cmake libglfw3-dev libglew-dev libopencv-dev nlohmann-json3-dev.

Typical way to do it:

 ```
sudo apt install build-essential git git-lfs cmake libglfw3-dev libglew-dev libopencv-dev nlohmann-json3-dev
```


### Typical installation path:

* cd to the dir you whant to install imagine source code to;
* Clone image source code from git:
```
git clone git@github.com:valera-vorona/imagine.git
```
* Typical simple way to install any project from source code with cmake:
```
cd imagine
mkdir build && cd build
cmake ..
cmake --build .
sudo cmake --install .
```
* That's it.
* *If you don't need ``imagine`` source code any longer, you can remove the whole directory, all installed binaries and other files required to run ``imagine`` will remain.*

## Other OS

### No information yet

* Install ``git`` ``git-lfs`` ``c++ +20``, ``cmake 3.22`` ot higher, ``libglfw3-dev``, ``libglew-dev``, ``libopencv-dev``, ``nlohmann-json3-dev``;
* Clone the source code from ``git@github.com:valera-vorona/imagine.git``;
* Compile and install the project.

# HOW TO USE

* Just run and enjoy;
