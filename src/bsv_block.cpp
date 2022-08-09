#include "../include/bsv_block.h"

std::string BSV_BLOCK::reverse_pairs(std::string const & src)
{
    assert(src.size() % 2 == 0);
    std::string result;
    result.reserve(src.size());

    for (std::size_t i = src.size(); i != 0; i -= 2)
    {
        result.append(src, i - 2, 2);
    }

    return result;
}

std::string BSV_BLOCK::get_blockHash(std::string version, std::string previous_block_hash, std::string merkle_root, std::string time, std::string bits, std::string nonce)
{
    using namespace CryptoPP;
    SHA256 hash;
    HexDecoder decoder;
    std::string result;
    std::string decoded;
    std::string to_hash = reverse_pairs(version)+reverse_pairs(previous_block_hash)+reverse_pairs(merkle_root)+reverse_pairs(time)+reverse_pairs(bits)+reverse_pairs(nonce);

    decoder.Attach( new StringSink( decoded ) );
    decoder.Put( (byte*)to_hash.data(), to_hash.size() );
    decoder.MessageEnd();
    std::string out;

    StringSource foo(decoded, true, 
        new HashFilter(hash,new StringSink(out))
    );
    StringSource poo(out, true, 
        new HashFilter(hash,new HexEncoder(new StringSink(result),true))
    );

    return reverse_pairs(result);
}

std::string BSV_BLOCK::get_TXID(std::string raw_transaction)
{
    using namespace CryptoPP;
    SHA256 hash;
    HexDecoder decoder;
    std::string result;
    std::string decoded;
    std::string to_hash = reverse_pairs(raw_transaction);

    decoder.Attach( new StringSink( decoded ) );
    decoder.Put( (byte*)to_hash.data(), to_hash.size() );
    decoder.MessageEnd();
    std::string out;

    StringSource foo(decoded, true, 
        new HashFilter(hash,new StringSink(out))
    );
    StringSource poo(out, true, 
        new HashFilter(hash,new HexEncoder(new StringSink(result),true))
    );

    return reverse_pairs(result);

}

BSV_BLOCK::BSV_BLOCK(uint32_t* ptr, std::string* data) : file_data(data), ptr(ptr) {
     

    // preamble
    magic_number = read_bytes(4);

    block_size = read_bytes(4);
    

    // header
    version = read_bytes(4);
    previous_block_hash = read_bytes(32);
    merkle_root = read_bytes(32);
    
    time = read_bytes(4);

    bits = read_bytes(4);
    nonce = read_bytes(4);
    block_hash = get_blockHash(version, previous_block_hash, merkle_root, time, bits, nonce);
    
    block_size = std::to_string(hex_to_int(block_size));
    time = std::to_string(hex_to_int(time));
    bits = std::to_string(hex_to_int(bits));
    nonce = std::to_string(hex_to_int(nonce));


    // transactions
    unsigned long long num_bytes = 0;
    number_of_transactions = read_variable_bytes(num_bytes);
    transactions.reserve(number_of_transactions);
    for (int i=0; i<number_of_transactions; i++) {
        num_bytes = 0;
        BSV_TRANSACTION transaction;

        transaction.version = read_bytes(4);
        num_bytes += 4;

        transaction.number_of_inputs = read_variable_bytes(num_bytes);
        transaction.inputs.reserve(transaction.number_of_inputs);

        for( int x=0; x<transaction.number_of_inputs; x++) {
            
            BSV_INPUT input;

            input.pre_transaction_hash = read_bytes(32);
            input.pre_transaction_out_index = read_bytes(4);
            input.script_length = read_variable_bytes(num_bytes);
            input.script = read_bytes(input.script_length);
            input.sequence = read_bytes(4);

            transaction.inputs.push_back(input);
            num_bytes += 40 + input.script_length;
        }
        
        transaction.number_of_outputs = read_variable_bytes(num_bytes);
        transaction.outputs.reserve(transaction.number_of_outputs);


        for( int y=0; y<transaction.number_of_outputs; y++) {
            BSV_OUTPUT output;

            output.value = read_bytes(8);
            output.script_length = read_variable_bytes(num_bytes);
            output.script = read_bytes(output.script_length);

            transaction.outputs.push_back(output);

            num_bytes += 8 + output.script_length;
        }
    
        transaction.lock_time = read_bytes(4);
        num_bytes += 4;

        *ptr -= num_bytes*2;

        std::string raw_transaction = read_bytes(num_bytes);
        transaction.TXID = get_TXID(raw_transaction);

        transactions.push_back(transaction);
    }
    
}   

/**
 * @brief takes a hex string and converts it to its equivalent unsigned long long value
 * 
 * @param hex_string 
 * @return unsigned long long
 */
unsigned long long BSV_BLOCK::hex_to_int(std::string hex_string) {
    unsigned long long val = std::stoull(hex_string, 0, 16);
    return val;
}

/**
 * @brief reads bytes number of bytes, starting at ptr, from the file_data
 * 
 * @param bytes 
 * @return std::string* 
 */
std::string BSV_BLOCK::read_bytes(int bytes) {
    std::string byte_string = (*file_data).substr(*ptr, bytes*2);
    *ptr += bytes*2; // increments ptr as necessary

    // convert to big-endian format in O(n) time
    std::string big_endian_byte_string;
    int local_ptr = bytes*2 - 1;
    while (local_ptr >= 0) {
        big_endian_byte_string += byte_string.substr(local_ptr-1, 2);
        local_ptr -= 2;
    }
    
    return big_endian_byte_string;
}

/**
 * @brief reads a VarInt from the file_data, starting from ptr pointer
 * 
 * @return unsigned long long
 */
unsigned long long BSV_BLOCK::read_variable_bytes(unsigned long long &num_bytes) {
    std::string hex_string = read_bytes(1);
    num_bytes++;
    unsigned long long val = hex_to_int(hex_string);

    if (val == 253) {
        std::string two_byte_hex_string = read_bytes(2);
        num_bytes += 2;
        unsigned long long two_byte_val = hex_to_int(two_byte_hex_string);
        return two_byte_val;
    }
    else if (val == 254) {
        std::string four_byte_hex_string = read_bytes(4);
        num_bytes += 4;
        unsigned long long four_byte_val = hex_to_int(four_byte_hex_string);
        return four_byte_val;
    }
    else if (val == 255) {
        std::string eight_byte_hex_string = read_bytes(8);
        num_bytes += 8;
        unsigned long long eight_byte_val = hex_to_int(eight_byte_hex_string);
        return eight_byte_val;
    }
    else if (val > 255) {
        perror("VarInt greater than 255!");
        exit(1);
    }

    return val;
}

BSV_BLOCK::~BSV_BLOCK() {
    //TODO: implement destructor if necessary in the future
}