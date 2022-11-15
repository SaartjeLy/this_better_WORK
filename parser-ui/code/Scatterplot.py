import csv
import dash
from cuxfilter import charts, DataFrame
import plotly.graph_objects as go
import plotly.express as px
import dash as dcc  #_core_components
import cudf
import cuxfilter
from dash.dependencies import Input, Output
import dash as html #_core_components
import numpy as np
import cupy as cp
import pandas as pd
import math
import requests

df = pd.read_csv("5/Headers.csv")
transaction_df = pd.read_csv('5/transactions.csv')
tx_ids = transaction_df['TXID']

header_df_slice = df.iloc[:500, :] # variable num for range
num_tx = header_df_slice['number_of_transactions']
difficulty = header_df_slice['difficulty']
block_num = header_df_slice['block_number']

tx_df_slice = transaction_df.iloc[:500, :]
tx_ids = tx_df_slice['TXID']

frame0 = None
frames = []

fee_list = []
count_list = []
text_list = []

df_dict = {'fee': [], 'block_num': [], 'tx_num': []}
fee_tx_dict = {}
tx_sum = 0

################ getting fee paid and putting into csv

# for num in range(6): # len block_num
#     # getting the fee in each block
#     num_transactions = num_tx[num]
#     for index in range(1, num_transactions +1): #index = tx_num
#         try:
#             response = requests.get('https://blockchain.info/rawtx/' + str(tx_ids[tx_sum + index])) #CHECK THIS RIGHT
#         except:
#             print('error w response')
#         online_fee_dict = response.json()
#         fee = online_fee_dict['fee']
#         tx_sum +=1
        
#         if fee == 0:
#             continue
        
#         df_dict['fee'].append(fee)
#         df_dict['block_num'].append(num)
#         df_dict['tx_num'].append(index)
# df = pd.DataFrame.from_dict(df_dict)
# df.to_csv(r'C:\Documents\Parser\export_dataframe.csv', index=False, header=True)

#####################

fee_df = pd.read_csv("6_blocks_fee_data.csv")
fee = fee_df['fee']
block_num = fee_df['block_num']
tx_num = fee_df['tx_num']

for index in range(len(fee)): 
    bn = int(block_num[index]) #block number
    each_fee = fee[index] 
    if each_fee in fee_tx_dict: #if fee is already in dict
        if bn in fee_tx_dict[each_fee]: #if the block is in this fee key inside dictionary
            if len(fee_tx_dict[each_fee][bn][0]) > 120: #nested dictionary, the length of the transaction string value inside block num key. 120 for arbitraty len
                fee_tx_dict[each_fee][bn] += f'<br />tx{index}'
                fee_tx_dict[each_fee]['counter'] +=1
            else: 
                fee_tx_dict[each_fee][bn] += f'   tx{index}'
                fee_tx_dict[each_fee]['counter']+=1
                    
        else:
            fee_tx_dict[each_fee][bn] = f' tx{index}' 
            fee_tx_dict[each_fee]['counter'] +=1
    else:
        fee_tx_dict[each_fee] = {'counter': 1, bn: f' tx{index}'}
       

for fee in fee_tx_dict:
    fee_list.append(fee)
    count_list.append(fee_tx_dict[fee]['counter'])
    

    string = ''
    previous = None
    each_bnum_counter = 0

    for b_num in fee_tx_dict[fee]:
        if b_num == 'counter':
            string += str(b_num) + ':' + str(fee_tx_dict[fee][b_num]) + '<br /> <br />' 
        else:
            string += 'B'+ str(b_num) + ':' + str(fee_tx_dict[fee][b_num]) + '<br /> <br />'
    text_list.append(string)    


fee_count_df = pd.DataFrame({'fee':fee_list, 'count': count_list, 'text': text_list})# 'Hover text': hover_text


for index in range(max(block_num)): #weight_index == block_num
    colour_list = []
    opacity_list = []
    specific_block = index
    for fee in fee_tx_dict:
        if specific_block in fee_tx_dict[fee]: # as it's moving with the treemap animation, it should highlight the specific block it's on and where it's transactions are!
            colour_list.append('red')
            opacity_list.append(1)
        else:
            colour_list.append('grey')
            opacity_list.append(.19)
            


        
    scatter_fig = go.Figure(data=go.Scatter(x=fee_count_df['fee'], 
                                    y=fee_count_df['count'], 
                                    text = fee_count_df['text'],
                                    mode='markers',
                                    marker=dict(
                                    size=40,
                                    color = colour_list,
                                    line_width=.5,
                                    opacity= opacity_list
                                                )
                                    ))


    if frame0 is None:
        frame0 = scatter_fig
    frames.append(go.Frame(name=f"frame-{index}", data=scatter_fig['data'][0]))




layout = {
    'title': "Animation of bitcoin blocks",
    "updatemenus": [ 
        {'type': 'buttons',
            'buttons': [
                {

                    'args': [ 
                        None, 
                        dict(
                            frame=dict(duration=600, redraw=True),
                            transition=dict(duration=200),
                            fromcurrent=True,
                            mode='intermediate',
                        ),
                        
                        ],
                    'label': 'Play',
                    'method': 'animate'
                    
                },
                
                {
                "args": [[None], {"frame": {"duration": 0, "redraw": False},
                                    "mode": "immediate",
                                    "transition": {"duration": 0}}],
                "label": "Pause",
                "method": "animate"
                },
                
            ],
            },
        ]
}


#create final figure
figure= go.Figure(
    data=frame0['data'][0],
    layout=layout,
    frames=frames
)


figure.show()

