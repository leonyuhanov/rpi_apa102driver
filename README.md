# rpi_apa102driver
A simple APA102 SPI Driver for Raspberry Pi Written in C++. Compile spi.cpp
# For multiplexer version using the HC4067 16 Chanel Multiplexer
Compile multiplexedTest.cpp and have a look at the multiplexer.png for layout


# Setup Instructions for RPI4

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install pure-ftpd
```

//Install BCM C++ driver LIB from http://www.airspayce.com/mikem/bcm2835/index.html
```
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
tar zxvf bcm2835-1.50.tar.gz
cd bcm2835-1.xx
./configure
make
sudo make check
sudo make install
```

# To compile

```
g++ -o appName sourceCode.cpp -l bcm2835
```
