## Running the Software

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
