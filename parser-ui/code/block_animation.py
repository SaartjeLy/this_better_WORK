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


headers_df = pd.read_csv("5/Headers.csv")
transaction_df = pd.read_csv('5/transactions.csv')
print(transaction_df['TXID'][0])


block_num = headers_df['block_number']
num_of_transactions = headers_df['number_of_transactions']
time = headers_df['time']
difficulty = headers_df['difficulty']


frame0 = None
frames = []

#THIS WORKS if u can't figure out groupby
difficulty_list = [] #sorted list of unique difficulties
for i in range(len(difficulty)):
    if difficulty[i] not in difficulty_list:
        difficulty_list.append(difficulty[i])
difficulty_list.sort()

list_blocks = [] # sorting blocks in terms of difficulty

for diff in difficulty_list:
    for block in range(len(block_num)):
        if difficulty[block] == diff:
            list_blocks.append(block)       
        else:
            pass

  
    
for block in range(5): #len(list_blocks)
    txs = num_of_transactions[block]
    block_root = []

    tx_list = []
    for tx_num in range(1, txs +1):
        block_root.append("Block " + str(block)) #change to str of the block
        tx_list.append("Transaction " + str(tx_num))
        
    times_list = [] #increment thru the list later
    time_i_increment = 0 
    for i in range(time_i_increment, time_i_increment + txs):
        times_list.append(time[i])
    time_i_increment += txs

      
        

    #if doesn't work, try df_temp values
    treemap = go.Treemap(
        labels = tx_list, 
        values= times_list, 
        parents= block_root,
        branchvalues = 'total',
        root_color='lightgrey',
        # textinfo = "label+ percent parent",
        texttemplate='%{label} <br> %{value} <br> %{percentRoot}',
        pathbar_textfont_size=15,
    )
    if frame0 is None:
        frame0 = treemap
    frames.append(go.Frame(name=f"frame-{block}", data=treemap))


    #make sliders
    sliders = [
        dict(
            steps=[
                dict(
                    method='animate',
                    args=[
                        [f"frame-{block}"],
                        dict(mode='e', frame=dict(redraw=True), transition=dict(duration=100))
                    ],
                    label=f"{block}"
                )
                for block in range(5) #len(block_num)
            ],
            transition=dict(duration=0),
            x=0,
            y=0,
            currentvalue=dict(
                font=dict(size=12), prefix='Block: ', visible=True, xanchor='center'
            ),
            len=1.0,
            active=True,
        )
    ]

#create the layout object with slider parameters
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
        ],
        'sliders': sliders,
}


#create final figure
figure= go.Figure(
    data=frame0,
    layout=layout,
    frames=frames
)

figure.show()



# for number in difficulty_groups.values():
#     print(number)
    
#     string = str(difficulty_groups[diff_key])
#     if string[13] == ' ':
#         end = string.rfind("],")
#         block_string += string[13:end]
#     else:
#         end = string.rfind("],")
#         block_string += string[12:end]

# white_space_split = block_string.split()
# print(white_space_split[10])


# for string in white_space_split:
#     number = int(string[:-1])
#     print(number)
        

    
    # # for number in range(difficulty_groups[diff_key]):
    #     print(number)
#     list_blocks.append(difficulty_groups[diff_key])

# print(list_blocks[0])

#     for block in range(len(block_num)):
#         if difficulty[block] == diff_key:
#             list_blocks.append(block)       
#         else:
#             pass


    
    # #using time as a placeholder, it gives block time for each tx which is wrong (4 now!) :)
    # df_total = headers_df[['block_number', 'difficulty', 'number_of_transactions', 'time']]

    # mask = block #try block_num or block_n
    # df_temp = (
    #     df_total.loc[mask]
    #     # .dropna(subset="time")[['number_of_transactions', 'time']] #IMPLEMENT L8R?
    #     # .fillna("")
    # ) #0 = block_num, 1 = num tx, 2 = time
    
    