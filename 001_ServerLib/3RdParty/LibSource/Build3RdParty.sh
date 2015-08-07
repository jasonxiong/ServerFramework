#!/bin/sh

#build protobuf
tar -zxvf protobuf-2.5.0.tar.gz
cd protobuf-2.5.0
./configure || (echo "Failed to configure protobuf, check it!" && exit -1)
make || (echo "Failed to build protobuf, check it!" && exit -1)
cp src/.libs/libprotobuf.a ../../protobuf-2.5.0
cp src/protoc ../../../../103_Protocol/Conf/proto/
cd ..
rm -rf protobuf-2.5.0
echo "Success to build protobuf"

#build gtest
unzip gtest-1.6.0.zip
cd gtest-1.6.0
./configure || (cho "Failed to configure gtest, check it!" && exit -1)
make || (echo "Failed to build gtest, check it!" && exit -1)
cp lib/.libs/libgtest.a ../../gtest-1.6.0
cd ..
rm -rf gtest-1.6.0
echo "Success to build gtest"

#build lua
tar -zxvf lua-5.2.2.tar.gz
cd lua-5.2.2
make linux || (echo "Failed to build lua, check it !" && exit -1)
cp src/liblua.a ../../lua-5.2.2/
cd ..
rm -rf lua-5.2.2
echo "Success to build lua"

#build zeromq
tar -zxvf zeromq-3.2.3.tar.gz
cd zeromq-3.2.3
./configure || (echo "Failed to configure zeromq, check it!" && exit -1)
make || (echo "Failed to configure zeromq, check it!" && exit -1)
cp src/.libs/libzmq.a ../../zeromq-3.2.3
cd ..
rm -rf zeromq-3.2.3
echo "Success to build zeromq"

#build UdpLog
tar -zxvf UdpLog.tar.gz
cd UdpLog/Build/
cmake ../
make || (echo "Failed to build UdpLog lib, check it!" && exit -1)
cp libUdpLog.a ../../../UdpLog/
cd ../..
rm -rf UdpLog
echo "Success to build udp log"

echo -e "\n\n"

echo "Success to build all 3rdparty libs!"
