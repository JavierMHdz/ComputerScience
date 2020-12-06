#Makefile for EE450 Homework 1
# build an executable
all: crc_tx crc_rx crc_vs_checksum

crc_tx: crc_tx.cpp
	g++ -g -Wall -o crc_tx crc_tx.cpp

crc_rx: crc_rx.cpp
	g++ -g -Wall -o crc_rx crc_rx.cpp

crc_vs_checksum: crc_vs_checksum.cpp
	g++ -g -Wall -o crc_vs_checksum crc_vs_checksum.cpp

clean:
	rm crc_tx crc_rx crc_vs_checksum
