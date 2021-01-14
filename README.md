# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file for the purposes of analytics using Nvidia RAPIDS.   

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

at this stage, only parsing the header data and focusing on building out the rest of the stack. 
