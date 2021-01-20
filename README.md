# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file for the purposes of analytics using Nvidia RAPIDS.   

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

parses entire blocks from .dat data files, but only writes the header preamble and header info of each block to csv.   

can currently parse the first 100 blk files (blk00000.dat - blk00099.dat) in roughly 250 seconds. each block is exactly 128MB, meaning the parser can parse 12800 MB every 250 seconds. this gives a parse rate of __~51.12MB/s__.   
_NOTE: parsing includes writing preamble + block header to CSV file - also writing transaction data to file will __reduce__ the parse rate. not including writing to file in execution time for parsing will __raise__ the parse rate._

at this current parse rate, it should take roughly 248000 / 51.2 = __4843.75 seconds__!
