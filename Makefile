JSON_PATH = ./external

all:
	g++ -std=c++17 src/main.cpp src/csv_writer.cpp src/json_writer.cpp src/bsv_block.cpp src/bsv_transaction.cpp src/bsv_input.cpp src/bsv_output.cpp -I$(JSON_PATH) -I -O2 -pthread -o parser -lcryptopp
example:
	./parser example/dat/
