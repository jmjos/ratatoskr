# Traffic Generator and Receiver for NoC:

## 1- Python

### Data Generator Unit (DGU):
The goal of this unit is generating the injection data (sending data) through a 2D or 3D network. The required data for the injection has been provided in the high-level language and the injector itself has been implemented in VHDL language. In the first step the desired data type is converted to binary and then with multiple further conversions, the input data file produces a couple of data sets. After this step, the Data Generator Unit (DGU) generates “Data Flits”, “Injection Packet Length” and “Injection Time” text files. The conversion system is very flexible and the end-user can define different data injection scenarios. The following variables can be altered by the user for each scenario:
-	The data type (the input data can be in any given data format)
-	Addressing (with the address of the source and destination routers, as well as the network size)
-	Random and constant packet length
-	Flit size
-	Injection time
-	Multi-header
-	Packet counter
-	Flit-padding
-	User-defined extra options
- Header included or not

### Data Converter Unit (DCU):
The operation of this unit starts after receiving data from NoC. The inputs for this unit are “Injection Packet Length” and “Received Data” text files. After processing and receiving the data in the network (from VHDL part of the project), this unit becomes handy for further processing. With using Data Converter Unit (DCU) the end-user is able to do the following processing:
-	Convert back the received data to the original data type and do further processing (like noise analysis)
-	Comparison of the received data (from VHDL) and the generated data (from Python) in a line by line system for having a more level of processing details (This option can be very useful when the data is not successfully received and further processing in the original data file is not possible)
-	Publish error reports including the mismatch in data lines and the error in the line counting.
-	Publish the report about the routing path and data injection characteristics (Parsing the Header information)


## 2- VHDL

### Traffic Generator Unit (TGU):
This unit plays a crucial role in the project and with connecting this unit to a NoC, the converted data from DGU can flow through the network with the credit-based flow control technique. The only required step is defining the interface between the network and the Traffic Generator Unit (DGU). After this setup, everything from VHDL side will be automatically executed and the data will be injected to the network. For further processes, this unit generates a text file which indicates the injection time of the data into the first router in the network (which is different from the injection time of the previous section because of the reset signal and the latency in the data injection).

### Traffic Receiver Unit (TRU):
This unit receives the data from the network and sends an Acknowledge signal to the sender router. This unit stores the “Received data” and the “Received time” in separated text files. The output text files from this unit will be used in DCU for further processing.


## How to use this Traffic Generator and Receiver?
The core section of this traffic generator is designed in a high-level programming language and the user can easily alter the required parameters in the provided Python files. The output data from python files (file2data.py or random2data.py) can be used as an input dataset to the desired Network-On-Chip. As an example the traffic generator has been connected to a "3D NoC" and the data has been transferred through the network in two different scenarios (A random dataset & A correlated dataset).
After the transmission of a dataset, further analysis will be provided in the python section of the project (data2file.py). In this step, the system publishes several report files. Moreover, if the initial given data to the python file derived from an existing data-type, the initial file can be converted back to the initial format after the transmission through the network. The designed traffic generator and receiver gives the opportunity to make further analysis (Like image processing of a received image)
