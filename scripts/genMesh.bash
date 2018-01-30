#!/bin/bash


#Copyright 2018 Jan Moritz Joseph
#
#Permission is hereby granted, free of charge, to any person obtaining a
#copy of this software and associated documentation files (the "Software"),
#to deal in the Software without restriction, including without limitation
#the rights to use, copy, modify, merge, publish, distribute, sublicense,
#and/or sell copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included
#in all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### Parameter List ###
file=$1
x=$2
y=$3
z=$4
### Parameter List ###

echo -e '<?xml version="1.0" encoding="UTF-8"?>' > $file
echo -e '<network-on-chip xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="layer.xsd">' >> $file
echo -e '\t<nodeTypes>' >> $file
echo -e '\t\t<nodeType id="0">' >> $file
echo -e '\t\t\t<routerModel value="XYZ"/>' >> $file
echo -e '\t\t\t<clockSpeed value="2"/>' >> $file
echo -e '\t\t</nodeType>' >> $file
echo -e '\t</nodeTypes>\n' >> $file

# Generate Layer List
echo -e '\t<layerTypes>' >> $file
i=0
while [ $i -lt $z ]
do
	echo -e '\t\t<layerType id="'$i'">' >> $file
	echo -e '\t\t\t<technology value="130"/>' >> $file
	echo -e '\t\t</layerType>' >> $file

	i=$(($i + 1))
done
echo -e '\t</layerTypes>\n' >> $file


# Generate Node List
echo -e '\t<nodes>' >> $file  
num=0 
i=0
while [ $i -lt $z ]; do
	j=0
	while [ $j -lt $y ]; do
		k=0
		while [ $k -lt $x ]; do
			echo -e '\t\t<node id="'$num'">' >> $file
			echo -e '\t\t\t<xPos value="'$(echo "scale=2; $k/$(($x-1))" | bc | sed "s/^\./0./")'"/>' >> $file
			echo -e '\t\t\t<yPos value="'$(echo "scale=2; $j/$(($y-1))" | bc | sed "s/^\./0./")'"/>' >> $file
			echo -e '\t\t\t<zPos value="'$(echo "scale=2; $i/$(($z-1))" | bc | sed "s/^\./0./")'"/>' >> $file
			echo -e '\t\t\t<nodeType value="0"/>' >> $file
			echo -e '\t\t\t<layerType value="'$i'"/>' >> $file
			echo -e '\t\t</node>' >> $file
			num=`expr $num + 1`
			k=`expr $k + 1`
		done
		j=`expr $j + 1`
	done
	i=`expr $i + 1`
done
echo -e '\t</nodes>\n' >> $file

# Generate Connection List
echo -e '\t<connections>' >> $file
xoff=(1 0 0)
yoff=(0 1 0)
zoff=(0 0 1)
num=0 
i=0
while [ $i -lt $z ]; do
	j=0
	while [ $j -lt $y ]; do
		k=0
		while [ $k -lt $x ]; do
			node1=$(($i*($x*$y)+($j*$x)+$k))
			echo -e '\t\t<con id="'$num'">' >> $file
			echo -e '\t\t\t<length value="0"/>' >> $file
			echo -e '\t\t\t<ports>' >> $file
			echo -e '\t\t\t\t<port id="0">' >> $file
			echo -e '\t\t\t\t\t<node value="'$node1'"/>' >> $file
			echo -e '\t\t\t\t\t<bufferDepth value="16"/>' >> $file
			echo -e '\t\t\t\t\t<vcCount value="4"/>' >> $file
			echo -e '\t\t\t\t</port>' >> $file
			echo -e '\t\t\t</ports>' >> $file
			echo -e '\t\t</con>'>>$file            
			num=`expr $num + 1`			

			l=0
			while [ $l -lt 3 ]; do
				if	([ $l -eq 0 ] && [ $k -ne $(($x-1)) ]) || 
					([ $l -eq 1 ] && [ $j -ne $(($y-1)) ]) ||
					([ $l -eq 2 ] && [ $i -ne $(($z-1)) ])
				then
					node1=$(($i*($x*$y)+($j*$x)+$k))
					node2=$((($i+${zoff[$l]})*($x*$y)+(($j+${yoff[$l]})*$x)+$k+${xoff[$l]}))
                    
					echo -e '\t\t<con id="'$num'">' >> $file
					echo -e '\t\t\t<length value="900"/>' >> $file
                    echo -e '\t\t\t<width value="3"/>' >> $file
                    echo -e '\t\t\t<depth value="1"/>' >> $file
                    echo -e '\t\t\t<interface value="0"/>' >> $file
					echo -e '\t\t\t<ports>' >> $file
					echo -e '\t\t\t\t<port id="0">' >> $file
					echo -e '\t\t\t\t\t<node value="'$node1'"/>' >> $file
					echo -e '\t\t\t\t\t<bufferDepth value="16"/>' >> $file
					echo -e '\t\t\t\t\t<vcCount value="4"/>' >> $file
					echo -e '\t\t\t\t</port>' >> $file
					echo -e '\t\t\t\t<port id="1">' >> $file
					echo -e '\t\t\t\t\t<node value="'$node2'"/>' >> $file
					echo -e '\t\t\t\t\t<bufferDepth value="16"/>' >> $file
					echo -e '\t\t\t\t\t<vcCount value="4"/>' >> $file
					echo -e '\t\t\t\t</port>' >> $file
					echo -e '\t\t\t</ports>' >> $file
					echo -e '\t\t</con>'>>$file
                    
					num=`expr $num + 1`
				fi
				l=`expr $l + 1`
			done
			k=`expr $k + 1`
		done
		j=`expr $j + 1`
	done
	i=`expr $i + 1`
done
echo -e '\t</connections>' >> $file
echo -e '</network-on-chip>' >> $file
