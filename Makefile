all:
	g++ -std=c++17 src/main.cpp src/csv_writer.cpp src/json_writer.cpp src/bsv_block.cpp src/bsv_transaction.cpp src/bsv_input.cpp src/bsv_output.cpp -I -O2 -pthread -lstdc++fs -o parser
example:
	./parser example/
