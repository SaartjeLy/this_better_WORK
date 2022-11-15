
import csv
from cuxfilter import charts, DataFrame
import plotly.graph_objects as go
import plotly.express as px
import cudf
import cuxfilter
import numpy as np
import cupy as cp
import pandas as pd
import datetime

def main():
    filename_prompt = input("Enter filename: ")
    
# ######### USD fees per tx
#     fees_per_tx_df = cudf.read_csv(r'fees-usd-per-transaction.csv') # IN USD
#     timestamp = fees_per_tx_df['timestamp']
#     fees_per_tx = fees_per_tx_df['fees-usd-per-transaction'] # Average transaction fees in USD per transaction... Ahead by 2, if you say 196 then it's actually row 198
    headers_filename = filename_prompt + "/Headers.csv"
    inputs_filename = filename_prompt + '/Inputs.csv'
    outputs_filename = filename_prompt + '/Outputs.csv'
    transactions_filename = filename_prompt + '/transactions.csv'
    
    
    
######### Headers  
    headers_df = pd.read_csv(headers_filename)
    block_num = headers_df['block_number']
    difficulty = headers_df['difficulty']
    block_hash = headers_df['block_hash']
    num_of_transactions = headers_df['number_of_transactions']
    block_size = headers_df['block_size']
    prev_block_hash = headers_df['previous_block_hash']
    merkle_root = headers_df['merkle_root']
    time = headers_df['time']
    nonce = headers_df['nonce']

######### Input
    input_df = pd.read_csv(inputs_filename)
    prev_tx_hash = input_df['pre_transaction_hash']

 

######### Output
    output_df = pd.read_csv(outputs_filename)
    output_values = output_df['value']
    
######### Transactions   
    transaction_df = pd.read_csv(transactions_filename)
    tx_ids = transaction_df['TXID']
    num_inputs_list = transaction_df['num_inputs']
    num_outputs_list = transaction_df['num_outputs']

    
######### Headers
    def get_block_num(block_list_index):
        return block_num[block_list_index] 
       
    def get_block_hash(block_hash_list_index):
        return block_hash[block_hash_list_index]
    
    def get_block_size(block_size_list_index):
        return block_size[block_size_list_index]
    
    def get_prev_block_hash(prev_blockhash_list_index):
        return prev_block_hash[prev_blockhash_list_index]
    
    def get_merkle_root(merkle_root_list_index):
        return merkle_root[merkle_root_list_index]
  
    def get_time(time_list_index):  # time taken to mine, UNIX EPOCH
        return time[time_list_index]       
        
    def get_number_transactions(transactions_list_index): #the number of tx each block contains  
        return num_of_transactions[transactions_list_index]  
    
    def get_nonce(nonce_list_index):
        return nonce[nonce_list_index] 
    
    def get_tx_difficulty(index):
        each_txs_difficulty = difficulty[index]/num_of_transactions[index]
        return each_txs_difficulty
    
    def get_date_time(time_list_index):    
        dt = datetime.datetime.fromtimestamp(time[time_list_index])
        return dt
    

   
           
######### Input
    def get_prev_tx_hash(prev_tx_hash_list_index):
        return prev_tx_hash[prev_tx_hash_list_index]
 
    
       
######### Output
    def get_output_value(values_list_index):
        return output_values[values_list_index] # output value for each output
    
    
######### Transactions    
    def get_txid(txid_list_index):
        return tx_ids[txid_list_index]
    
    def get_num_outputs(outputs_list_index): #number outputs per transaction
        return num_outputs_list[outputs_list_index]
    
    def get_num_inputs(inputs_list_index): #number outputs per transaction
        return num_inputs_list[inputs_list_index]
            
  
  
##### Tree map, tx will be different sizes when we have fee paid
    # For each block, for each transaction, for each output per transaction, for each value per output (satoshi value)
    # Output number string, output value string

##### loop thru blocks    
        
    for block in range(1):    #len(block_number)
        numbers = []
        block_root = []
        i_counter = 1
        tx_num_counter = 0 #so doesn't start at 0 each time
        output_list_increment = 0
        input_list_increment = 0
        num_transactions = get_number_transactions(block)
        each_block_size = get_block_size(block)   
        for tx_i in range(tx_num_counter, tx_num_counter + num_transactions):
            if i_counter  == num_transactions + 1:
                    break
            
            num_outputs = get_num_outputs(tx_i) 
            output_string = ""   
            output_counter = 1
            for output_i in range(output_list_increment, output_list_increment + num_outputs): 
                
                #output_str
                output_value = get_output_value(output_i)
                output_string += 'Output ' + str(output_counter) + " - value " + str(output_value) + "<br>" 
                output_counter +=1
            
            
            num_inputs = get_num_inputs(tx_i)   
            input_string = ""
            input_counter = 1
            for input_i in range(input_list_increment, input_list_increment + num_inputs):    
                # input str
                # input_value = 
                input_string += 'Input ' + str(input_counter) + " - value " +  "<br>" # + str(input_value) 
                input_counter +=1
            
            if i_counter == 1:
                numbers.append("Transaction " + str(i_counter) + "<br><br>" + output_string + '<br>' + input_string + '<br>TXID: ' + str(get_txid(0)) + '<br> <br>' + "Block difficulty: " + str(difficulty[block]) + "<br>Block hash: " + str(get_block_hash(block)) + "<br>Previous block hash: " + str(get_prev_block_hash(block)) + "<br>Block size: " + str(each_block_size))
            else:   
                numbers.append("Transaction " + str(i_counter) + "<br><br>" + output_string + '<br>' + input_string + '<br>TXID: ' + str(get_txid(i_counter - 1)))
            i_counter +=1
            output_list_increment += num_outputs
            block_root.append("Block " + str(block))
            
            
        date_time = get_date_time(block)
        tx_difficulty = get_tx_difficulty(block)
        tx_num_counter += num_transactions 
        
            #turn numbers list into pandas df column
        tx_text_df = pd.DataFrame({'col':numbers})  
        
        #maybe turn list of weights into pandas df column here (after feepaid) . Do I need to split up num transactions for weights for each block ?
        #weight_df = ........
        
    ######### WEIGHT  
        #sorted_weight = pd.DataFrame(headers_df.groupby([tx_text_df]).agg({'weight' : 'sum'}))

            
        # treemap figuration
        fig = go.Figure(go.Treemap(
            labels = numbers,
            parents = block_root,
            textinfo = "label+value+percent entry+percent root",
        ))
        
        #fig = px.treemap(headers_df, path=[tx_text_df], values='weight')
        fig.update_traces(root_color="lightgrey")
        fig.data[0].texttemplate = "%{label}<br><br>" + '<br>' + "Tx difficulty: " + str(tx_difficulty)  + "<br>" + "Mined on: " + str(date_time)
        fig.update_layout(margin = dict(t=50, l=25, r=25, b=25)) # this number FROM its side
        fig.show()
                



main()

        


            
    