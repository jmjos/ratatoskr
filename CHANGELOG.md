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
