<img style="width:50%;" src="https://github.com/jmjos/ratatoskr/raw/master/docs/misc/logo.png">

[![Build Status](https://travis-ci.com/jmjos/ratatoskr.svg?branch=master)](https://travis-ci.com/jmjos/ratatoskr)
[![CodeFactor](https://www.codefactor.io/repository/github/jmjos/ratatoskr/badge)](https://www.codefactor.io/repository/github/jmjos/ratatoskr)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1c867426f3d5450a9aed9a7d56602ffe)](https://www.codacy.com/app/jmjos/ratatoskr?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=jmjos/ratatoskr&amp;utm_campaign=Badge_Grade)

# Ratatoskr NoC simulator

Welcome to the project website of ratatoskr, a framework to design technology-asymmetric interconnect architectures targeting heterogeneous 3D-SoCs. It is developed at the University in Magdeburg (Germany). Our A-3D-Network-on-Chip simulator is a Network-on-Chip Simulator for asymmetric NoC-architectures. It is developed using SystemC, a system description language based on C++. The software is under MIT license terms.

If you use the simulator in your research, we would appreciate the following citation in any publications to which it has contributed:

---

Jan Moritz Joseph, Sven Wrieden, Christopher Blochwitz, Alberto Garc&iacute;a-Ortiz, Thilo Pionteck: <a href="about:black" target="_blank"><em> A Simulation Environment for Design Space Exploration for Asymmetric 3D-Network-on-Chip</em></a>, 11th International Symposium on Reconfigurable Communication-centric Systems-on-Chip (ReCoSoC 2016), Tallinn, 2016

---

## Installation

There are several ways to compile the project. If you'd just like to use the project, please use the cmake method.

### Build Enviroment

- Ubuntu 18.04 & Boost Libs & pugixml
- CentOS 7 & boost-devel.x86_64 & pugixml-devel.x86_64
- cmake 3.10.2
- SystemC 2.3.3 www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.gz
- cmake assumes an installation of systemc to ``/usr/local/systemc-2.3.3`` If not the case, please modify simulator/CMakeLists.txt

#### Building with build script
We provide a build script, which relys on cmake. It is provided run it via:
```
> cd simulator
> ./build
```


#### Building with cmake
Go into simulator folder. Simply type
```
> cd simulator
> cmake .
> make
```

To use a subfolder and multi-thread build:
```
> mkdir build
> cd build
> cmake ..
> make -j
> cp sim ..
```

#### Building with CLion

Works out-of-the-box using "Open Existing Project" and then opening the CMakeLists.txt in the prject folder. Enjoy.

#### Building with Eclipse

Open a workspace at the location "workspace"-folder. Then, create an new C++ project with the same name as the subfolder of the project you want to work on (i.e. a3dnoc-simulator). After applying the settings, you should be ready to go! 

Project Settings: 

```
> Dialect: C++0x  
> Preprocessor: -D SC_INCLUDE_DYNAMIC_PROCESSES  
> Includes: -I /usr/local/systemc/include   -I ./src
> Libraries: -l systemc, -L /usr/local/systemc/lib-linux64  
```

### Running the Software

Linking the shared libs globally via 
> sudo ln -s /usr/local/systemc-2.3.3/lib-linux64/libsystemc-2.3.3.so /usr/lib/libsystemc-2.3.3.so

or locally via 
> export LD_LIBRARY_PATH=/usr/local/systemc-2.3.3/lib-linux64:$LD_LIBRARY_PATH

or create a .conf file containing your SystemC library path (/usr/local/systemc-2.3.3/lib-linux64) inside
/etc/ld.so.conf.d/


or in eclipse under run configuration, environment, new

you'll link the systemc Libraries.

Execute the simulator:

```
>./sim
```


## Configuring the simulator
The simulator takes three input files:
- config/config.xml for basic configuation. Warning: The path is static linked in the source code, so keep folder structure
- a XML file describing the traffic and a mapping file, mapping tasks to processing elements. Examples can be found in simulator/traffic

Run the simulator using ./sim.

Details on the configuration can be found in the documentation. 

## Model

### Router

The router model is:
- input buffered router with VCs
- variable number of VCs and buffer depths for each VC
- round robin arbitration over VCs (rrVC) or fair arbitration over used VCs (fair)
- routing functions implemented in routing; different deterministic and adaptive routings are given
- selection functions implemented in selection
- supports synchronous and asynchronous clock speeds of layers

To run the uniform random simulation please go to <a href="https://github.com/jmjos/ratatoskr/tree/master/scripts/urand">README.md</a>

## Documentation

Take a look at the [documentation table of contents](docs/TOC.md).
This documentation is bundled with the project which makes it 
available for offline reading and provides a useful starting point for
any documentation you want to write about your project.

## Contributing
If you wish to contribute, please contact me at my univeristy email address jan.joseph@ovgu.de. We offer student development jobs for students from Magdeburg.

## Credits
A special thanks to my students Sven Wrieden and Imad Hajjar (github.com/imadhajjar) for their amazing help with the project. 

## License

This simulator is available to anybody free of charge, under the terms of MIT License:

Copyright (c) 2018 Jan Moritz Joseph

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
