# Changelog

All notable changes to this project will be documented in this file.

## [Released]

## [1.0.0] - 2018-01-09

### Features
- first release candidate
- supports basic NoC simulation features
- configurable NoC routers with routing and selection
- simple processing element model
- link model with transmission matrixes and raw data output for debugging
- synthetic, taskbased, and netrace support; netrace for 2D mode only

## [1.1.0] - 2018-01-22

### Features
- added functionalty of configurable routers
- adaptive routing and selection methods
- separation of processing elements and network interface
- removed netrace
- cmake support and simple build script

### Improvements
- links: removed unnecessary components; moved information to matrix

### Known Issues
- calculation of data amounts still faulty. 
- router matrixes missing
- implementation of agra, esparda, mafa missing

## [1.1.1] - 2018-01-23

### Features
- Sythetic traffic patterns fully implemented
- Fixes for the task model
- implementations of arga, esparda, mafa

### Known Issues
- calculation of data amounts still faulty. 
- router matrixes missing

## [1.1.2] - 2018-01-29

### Features 
- Removed Netrace for licensing

## [1.1.3] - 2018-01-30

### Improvements
- Removed Bug in XYZ routing preventing more than 2x2x2 topologies

## [1.1.4] - 2018-05-11

### Features
- Traffic patterns for face recognition 3D VSoC. Includes task graph and (manual) mapping. Parameters include grade of parallelization
- GUI for network topology

### Improvements
- Bug in uniform random traffic pattern: Distribution in PEs changed.

## [1.1.5] - 2018-05-22

### Features 
- Traffic statistics with minimum, maximum, and average latencies of flits, packets, and network
- Extended network report
- New router arbiter

### Improvements
- Bug fix in router arbiter to fix network stalls

## [1.1.6] - 2018-06-08

### Features
- corrented urand traffic pattern; packets from each processing element send per block (set via interval)
- Start and duration now determine length of synthetic run
- repeat determines the number of packets send per 
- Setting -1 for repeat of for length sets this very value to infinity
- Multiphase support for synthetic traffic patterns

### Known Issues
- min value in sythetic phase is equal to max values

## [1.1.7] - 2018-07-18

### Improvments
- bug fixed in router; flow control corrected
- bug fixed in synthetic traffic generation
- clean up

### Features
- VC arbitration in port level
- Code build

## [1.1.8] - 2018-08-30

### Features
- adding a 'fair' arbiter (round robin over flits from active ports). See Bamberg, Patmos 2018, "Coding-aware Link Energy Estimation for 2d and 3D Networks-on-chip with Virtual Channels" 
- code formatting 
- urand scripts to generate plots for router saturation

### Known Issues
- only one VC for NetworkInterface

## [1.1.9] - 2018-11-22

### Features
- a new selection, which selects the first free VC
- handling asynchronous speed (each layer can have its one clock delay)
- a new routing HeterogeneousXYZ
- each VC can have its one buffer depth
- enhancing the usage of the inner data structures used by routers
- enhancing the urand script and the plotting scripts

### Known Issues
- segmentation faults might happen randomly

## [2.0] - 2019-01-25

### Features
- Very large update with completly new data structures
- New data structures without with reduced complexity
- Many thanks to @imadhajjar

### Known Issues
- Performance not measured
- Still have to verify the VHDL model
- Scripts not finished
- lots more testing required

## [2.0.1] - 2019-02-13

### Features
- Fixed Heisenbug. 
- Added some statistics, not finished.

### Known Issues
- all of version 2.0

## [v2020-11] - Nov 2020

New naming convention as we applied rolling updates.

### New Features
- Netrace mode
- Test envionment
- Bandwidth measurements
- Trace file generator for synthetic mode
- New command line interface in netrace mode

### Known issues
- all from v2.0.1 removed.
- small bugs see open issues
