# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file or json file, for the purposes of analytics using Nvidia RAPIDS or uploading new bitcoin data to the 21e8 index.

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

can currently read the first 500 blk files (blk00000.dat - blk00499.dat) into memory in roughly 42095ms (~42 seconds) and parse this data in 55672ms (~55 seconds). each block is exactly 128MB, meaning the parser can read ~64000MB into memory at __1520.37MB/s__ and parse block data in memory at __1149.59MB/s__!

at this current parse rate, it should take roughly 278000 / 1520.37 = __~182s__ to read the entire blockchain into memory, and __~241s__ to parse the blockchain in memory.    
_NOTE: the 21e8 supermicro has ~192GB memory, so parsing the entire blockchain still needs to be done using batched processing!_   
