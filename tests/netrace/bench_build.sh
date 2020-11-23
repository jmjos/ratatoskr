mkdir build
cd build
cmake ../../../simulator -DDEFINE_ENABLE_NETRACE=ON
make
cd ..
cp build/sim .

if [ ! -f "netrace-1.0.tar.bz2" ]; then
 	wget https://www.cs.utexas.edu/~netrace/download/netrace-1.0.tar.bz2 
fi 
tar -xf netrace-1.0.tar.bz2
mv netrace-1.0/testraces/example.tra.bz2 config/
rm config/config.xml
cp config/ntConfig.xml config/config.xml
