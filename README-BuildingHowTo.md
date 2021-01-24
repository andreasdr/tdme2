![LOGO](https://raw.githubusercontent.com/andreasdr/tdme2/master/resources/github/tdme2-logo.png)

# 1. How to build TDME2?

## 1.1. FreeBSD(tested on FreeBSD 12.0)
### 1.1.1 Install packages

```bash
su -
pkg install git
pkg install gmake
pkg install glew
pkg install openal-soft
pkg install glfw
exit
```

### 1.1.2. Clone repository

```bash
git clone https://github.com/andreasdr/tdme2
cd tdme2
```

### 1.1.3. Build

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores and hyperthreading, you can use 4.

```bash
gmake clean && gmake -j HARDWARE_THREADS mains
```
## 1.2. OpenBSD(tested on OpenBSD 6.8)
### 1.2.1 Install packages

```bash
su -
pkg_add git
pkg_add gmake
pkg_add glew
pkg_add openal
pkg_add glfw
exit
```

### 1.2.2. Clone repository

```bash
git clone https://github.com/andreasdr/tdme2
cd tdme2
```

### 1.2.3. Build

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores, you can use 2.

```bash
gmake clean && gmake -j HARDWARE_THREADS mains CXX=clang++ CC=clang
```

## 1.3. Ubuntu(tested on XUBUNTU 18.04)
### 1.3.1 Install packages

```bash
sudo apt-get install git
sudo apt-get install make
sudo apt-get install g++
sudo apt-get install libglew-dev
sudo apt-get install libopenal-dev
sudo apt-get install libglfw3-dev
```

### 1.3.2. Clone repository

```bash
git clone https://github.com/andreasdr/tdme2
cd tdme2
```

### 1.3.3. Build

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores and hyperthreading, you can use 4.

```bash
make clean && make -j HARDWARE_THREADS mains
```

### 1.3.4. Build with GLES2

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores and hyperthreading, you can use 4.

```bash
make clean && make -j HARDWARE_THREADS mains GLES2=YES
```

## 1.4. Mac OS X
### 1.4.1 Install packages
You have to install XCode or XCode command line tools.

### 1.4.2. Clone repository

```bash
git clone https://github.com/andreasdr/tdme2
cd tdme2
```

### 1.4.3. Build

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores and hyperthreading, you can use 4.

```bash
make clean && make -j HARDWARE_THREADS mains
```

## 1.5. Windows MSYS2/MINGW64(tested on Windows 10 Home)
### 1.5.1 Install packages
Install 64 Bit MSYS2 from https://www.msys2.org/, e.g. msys2-x86_64-20190524.exe, then update your MSYS2 installation and install packages.

```bash 
pacman -Syu
pacman -S git
pacman -S make
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-glew
pacman -S mingw-w64-x86_64-openal
pacman -S mingw-w64-x86_64-glfw
```

### 1.5.2. Clone repository

```bash
git clone https://github.com/andreasdr/tdme2
cd tdme2
```

### 1.5.3. Build

Note: HARDWARE_THREADS should be replaced with the number of your CPU hardware threads, e.g. if you have 2 cores and hyperthreading, you can use 4.

```bash
make clean && make -j HARDWARE_THREADS mains
```

## 1.6. Windows MSC/X64(tested on Windows 7 Ultimate)
### 1.6.1 Install packages
Install Visual Studio 2019 Community Edition and install Windows 7 SDK. Be aware that the makefile uses the default installation paths for libraries and headers.

### 1.6.2. Download or clone repository

Use a GIT client with the following URL: https://github.com/andreasdr/tdme2 and clone to a folder into your home folder like "tdme2" OR 
send your browser to "https://github.com/andreasdr/tdme2" and click "Clone or download/Download ZIP" and extract ZIP somewhere in your home folder, e.g. "tdme2".

### 1.6.3. Build

Open your Visual Studio 2019 Community command line interface app for native X64 builds and execute the following commands. 
You need to adjust "tdme2" if you used a different folder.

```
cd %HOMEPATH%/tdme2
nmake -f Makefile.nmake
```

This will build TDME2 executables into your "tdme2" folder. You need to copy DLLs still to this folder, just that those dependencies are found.
You need to adjust "tdme2" if you used a different folder.

```
cd %HOMEPATH%/tdme2
copy ext\fbx\win64\lib\libfbxsdk.dll .
copy ext\win-freeglut\libs\freeglut.dll .
copy ext\win-glew\libs\glew32.dll .
copy ext\win-openal-soft\libs\OpenAL32.dll .
copy ext\win-pthread\libs\pthreadVC2.dll .
```

### 1.6.4. Note
- All *.EXE files are generated in your "tdme2" folder root. You should just be able to launch them from there. So you can ignore 2.
- The MSC build process is WIP

# 2. Running a TDME2 binary

You find binaries in
- ./bin/tdme/tests/
- ./bin/tdme/tools/

You have to execute the binaries from tdme2 root folder like:

```bash
$ pwd
/usr/home/andreas/Development/drewke.net/tdme2
$ ./bin/tdme/tests/AngleTest
```

# 3. Other information

- Links
    - TDME2 Engine, see [README.md](./README.md)
    - TDME2 Model Editor, see [README-ModelEditor.md](./README-ModelEditor.md)
    - TDME2 Particle System Editor, see [README-ParticleSystemEditor.md](./README-ParticleSystemEditor.md)
    - TDME2 Terrain Editor, see [README-TerrainEditor.md](./README-TerrainEditor.md)
    - TDME2 Scene Editor, see [README-SceneEditor.md](./README-SceneEditor.md)
    - TDME2 How to build, see [README-BuildingHowTo.md](./README-BuildingHowTo.md)

- Build instructions will follow for NetBSD if they get more recent graphics card support
 
