# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file for the purposes of analytics using Nvidia RAPIDS.   

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

parses entire blocks from .dat data files, but only writes the header preamble and header info of each block to csv.   

can currently parse the first 100 blk files (blk00000.dat - blk00099.dat) in roughly 250 seconds. each block is exactly 128MB, meaning the parser can parse 12800 MB every 105 seconds. this gives a parse rate of __~122MB/s__.   
_NOTE: parsing includes writing preamble + block header to CSV file - also writing transaction data to file will __reduce__ the parse rate. not including writing to file in execution time for parsing will __raise__ the parse rate._

at this current parse rate, it should take roughly 248000 / 122 = __~2033 seconds__ to parse the entire blockchain!


## **parsing the entire chain**

the current parser isn't (memory) efficient enough to parse all 2032 blkXXXXX.dat files, despite the 21e8 supermicro having ~180GB of memory, and typically exits with an OOM error at around ~400k+ blocks (in no particular order, as each file is parsed by an individual thread). so, to parse the entire blockchian, a batched approach is necessary. after doing some single threaded benchmarks, i have reason to believe the code itself doesn't have many, if any, memory leaks, but threading with the taskflow library itself is what results in excess memory usage.  

using batch processing, parsing around 100 or so blocks at a time, the entire process took __~2160047ms__. the entire blockchain (as at Dec 16 2020, unix time 1608155940) was about __289874084 KB__, or __289.874 GB__ (6 sf, where 1GB = 1024KB). so, the parse rate using this batched approach to parse the entire BSV blockchain was __~134 MB/s__.   

### __results for each batch:__  


blk00000.dat - blk00121.dat (excl)
- 115371ms
- ~43% peak memory usage
- 289121 blocks

blk00121.dat - blk00242.dat (excl)
- 113211ms
- ~43% peak memory usage
- 58131 blocks

blk00242.dat - blk00363.dat (excl)
- 132416ms
- ~43% peak memory usage
- 33883 blocks

blk00363.dat - blk00484.dat (excl)
- 128610ms
- ~39% peak memory usage
- 24564 blocks

blk00484.dat - blk00605.dat (excl)
- 126445ms
- ~42% peak memory usage
- 20723 blocks

blk00605.dat - blk00726.dat (excl)
- 127194ms
- ~43% peak memory usage
- 19607 blocks

blk00726.dat - blk00847.dat (excl)
- 125992ms
- ~42% peak memory usage
- 17432 blocks

blk00847.dat - blk00968.dat (excl)
- 127368ms
- ~42% peak memory usage
- 45240 blocks

blk00968.dat - blk01089.dat (excl)
- 149734ms
- ~43% peak memory usage
- 67098 blocks

blk01089.dat - blk01210.dat (excl)
- 96126ms
- ~39% peak memory usage
- 22867 blocks

blk01210.dat - blk01331.dat (excl)
- 105749ms
- ~36% peak memory usage
- 13476 blocks

blk01331.dat - blk01452.dat (excl)
- 117684ms
- ~33% peak memory usage
- 9578 blocks

blk01452.dat - blk01573.dat (excl)
- 133737ms
- ~33% peak memory usage
- 7976 blocks

blk01573.dat - blk01694.dat (excl)
- 175306ms
- ~50% peak memory usage
- 7381 blocks

blk01694.dat - blk01815.dat (excl)
- 164517ms
- ~50% peak memory usage
- 12366 blocks

blk01815.dat - blk01936dat (excl)
- 140587ms
- ~40% peak memory usage
- 9781 blocks

blk01936.dat - blk02031dat (incl)
- 80000ms
- ~30% peak memory usage
- 4150 blocks
