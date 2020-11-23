## Installation Guide

Quick script, see in bin/install.sh

Installation of zmq from 3rd party repo not required since Ubuntu 20.01. Otherwise, get Debian 9 repository
```bash
echo "deb http://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/ ./" | sudo tee -a /etc/apt/sources.list
```
download Debian 9 key and add to keychain
```bash
wget https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/Debian_9.0/Release.key -O- | sudo apt-key add
```
With Ubuntu 20.01 and all other OSs, install pugxml, boost, cmake, ZeroMQ and python3
```bash
sudo apt install -y git libpugixml-dev libboost-program-options-dev libboost-system-dev cmake libzmq3-dev python3-venv
```
install Tcl/Tk, Tk for python
```bash
sudo apt install -y tk-dev python-tk python3-tk
```
install systemc
```bash
cd ~/Downloads
wget www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.gz
tar -xzf systemc-2.3.3.gz
cd systemc-2.3.3/
sudo mkdir -p /usr/local/systemc-2.3.3
mkdir objdir
cd objdir
../configure --prefix=/usr/local/systemc-2.3.3
make -j3
sudo make install
```
[fork](https://help.github.com/en/github/getting-started-with-github/fork-a-repo) ratatoksr repository on github.com  
clone repository from your own account
```bash
cd ~
git clone https://github.com/YOUR-USERNAME/ratatoskr.git
cd ratatoskr/simulator
./build.sh
```

### Build Enviroment

- Ubuntu 18.04 (tested) or 19.10 (tested) & Boost Libs & pugixml
- CentOS 7 & boost-devel.x86_64 & pugixml-devel.x86_64
- cmake 3.10.2
- [SystemC 2.3.3] (www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.gz)
- cmake assumes an installation of systemc to ``/usr/local/systemc-2.3.3``. If not the case, please modify simulator/CMakeLists.txt
- installation of ZeroMQ, see http://zeromq.org/intro:get-the-software

If you are using Ubuntu 16, you must compile against SystemC 2.3.1a!

#### Building with build script
We provide a build script, which relys on cmake. It is provided run it via:
```
cd ~/ratatoskr/simulator
./build.sh
```


#### Building with cmake
Go into simulator folder. Simply type
```
cd simulator
cmake .
make
```

To use a subfolder and multi-thread build:
```
mkdir build
cd build
cmake ..
make -j
cp sim ..
```

#### Building with CLion

Works out-of-the-box using "Open Existing Project" and then opening the CMakeLists.txt in the project folder. You need to change one thing which is, the Generation path:
- File -> Settings -> Build, Execuation, Deployment -> CMake. Now set the Generation path to the current folder i.e. '.'.

Enjoy.

#### Building with Eclipse

Open a workspace at the location "workspace"-folder. Then, create an new C++ project with the same name as the subfolder of the project you want to work on (i.e. a3dnoc-simulator). After applying the settings, you should be ready to go!

Project Settings:

```
> Dialect: C++0x  
> Preprocessor: -D SC_INCLUDE_DYNAMIC_PROCESSES  
> Includes: -I /usr/local/systemc/include   -I ./src
> Libraries: -l systemc, -L /usr/local/systemc/lib-linux64  
```
## Compile Flags

- `ENABLE_GUI` enables the zmq forwarding from simulator to gui. Please disable for parallel execution of multiple instances of the simulator, as they block the same port and fail.
- `ENABLE_BUFFER_VC_STATS` enables detailed buffer stats per VC in form of a histogram. Large performance penalty, so disable if not required.
- `ENABLE_NETRACE` enables netrace mode. Separate mode, requires recompilation for many settings. Only works with config/nt*.xml configuration files.
