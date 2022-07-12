# bitcoinsv block parser

parses blkXXXXX.dat files retrieved from a BSV full node into a csv file or json file, for the purposes of analytics using Nvidia RAPIDS or uploading new bitcoin data to the 21e8 index.

this time, using C++ and Taskflow!   

should work for BTC and BCH, or any other coin that uses the original Bitcoin data format - retrieved from here: https://wiki.bitcoinsv.io/index.php/Block   

can currently read the first 500 blk files (blk00000.dat - blk00499.dat) into memory in roughly 42095ms (~42 seconds) and parse this data in 55672ms (~55 seconds). each block is exactly 128MB, meaning the parser can read ~64000MB into memory at __1520.37MB/s__ and parse block data in memory at __1149.59MB/s__!

at this current parse rate, it should take roughly 278000 / 1520.37 = __~182s__ to read the entire blockchain into memory, and __~241s__ to parse the blockchain in memory.    
_NOTE: the 21e8 supermicro has ~192GB memory, so parsing the entire blockchain still needs to be done using batched processing!_    

# Running

```bash
git clone --recurse-submodules https://github.com/21e8-ltd/21e8-parser
cd 21e8-parser
make -j
./parser <location of dat files>
```

# parser details

build and run using __build/run.sh__. the parser first will parse blkXXXXX.dat files and output either csv or json files, please read through __src/main.cpp__, __src/bsv_block.cpp__, __src/json_writer.cpp__, and __src/csv_writer.cpp__ to understand the general flow and what features the parser has.   

## general features:
- write BSV block headers to a csv file
- write number of twetches per BSV block to a csv file
- write BSV block hashes to a json file (doesn't re-calculate the hash, just uses the previous block hash field in the block header, see confluence for more details on block format)
- write twetch data to a json file (raw output data in ASCII so it's human readable)    
    

## suggested improvements:
- refactor the general parser architecture as this could be a lot better, and the project should really be using CMake because it's not tiny
    - refactor the way you choose to make either a csv or json file, this should be a runtime argument
    - refactor the way the number of blkXXXXX.dat files that should be read in and parsed is done, this should also be a runtime argument
    - other architecture improvements, up to you
- refactor the way some data is handled and certain types are used to speed up the parser
    - for example, in __src/main.cpp__, in the *get_file_data()* function, the data is read into a std::string, which causes a number of performance issues and slows down the parser by probably 2-3x
        - instead using std::string_view so that later on you don't create copies of that data in memory with std::string.substring()
        - std::string.substring runs in O(n) time but std::string_view.substring() runs in O(1) time, this would probably be the biggest contributor to parser speed
        - i looked into doing this at some point, but required a pretty big refactor and i had other sprint tasks that were more important
    - potentially remove the intial hex conversion in the *get_file_data()* function as well and instead only convert the binary data to hex when getting the data from the BSV_BLOCK object (will have a roughly 5x speedup on file reading, little to no speedup on parsing)
    - some other things that aren't coming to mind at 5:31PM on a Friday, also my last day

&nbsp;   
&nbsp;   
Lastly, forgive my average code (in my opinion), I didn't know C++ till I came to 21e8.   
