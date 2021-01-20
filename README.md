# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file for the purposes of analytics using Nvidia RAPIDS.   

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

parses entire blocks from .dat data files, but only writes the header preamble and header info of each block to csv.
