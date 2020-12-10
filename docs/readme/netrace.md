# Netrace mode

The netrace mode is more hard-coded, with much less configuration options.
It can be compiled using `cmake -D DEFINE_ENABLE_NETRACE=ON`

You can use a 2D or a 3D mode.
In 2D mode, the netrace cores are mapped to a 8x8 NoC.
In 3D mode, the cores are mapped to a 4x4x2 NoC.
The modes can be toggled using `GlobalResources::netrace2Dor3Dmode`.

Netrace relies on trace files from [here](https://www.cs.utexas.edu/~netrace/).

The netrace configuration is exemplified by the folder `simulator/tests/netrace`.

Both 2D and 3D mode use the same fixed (!) config file for the network.
It is an 8x8x2 NoC.
Mapping for both modes is different, see `GlobalResources.cpp`, lines 44ff.;
`GlobalResources::netraceNodeToTask`.
Do not change the config file as some parameters are hard coded.

In the netrace mode, the simulator always will read the `config/ntConfig.xml` file.
The file defines the `network.xml` used; you MUST use the same as in `simulator/tests/netrace/config/ntNetwork.xml`. It defines the aforementioned 8x8x2 NoC.

You can change the benchmark trace file in `ntConfig.xml`, `<netraceFile>config/example.tra.bz2</netraceFile>

Alternatively, you can use the command line arguments provided: `--simTime` for the simulation time, `--netraceStartRegion` (with an int from 0 to 5) for the start region, default is PARSEC's ROI, and `--netraceTraceFile` for the trace file destination.
