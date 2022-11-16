import csv
import numbers
from cuxfilter import charts, DataFrame
import plotly.graph_objects as go
import plotly.express as px
import cudf
import cuxfilter
import numpy as np
import cupy as cp
import pandas as pd
import datetime
import dash
from dash import dcc, html, Output, Input, callback, dash_table
from dash import html
from dash.dependencies import Input, Output

from datetime import datetime as dt
from datetime import date
from dash import dcc

app = dash.Dash(__name__)

headers_df = pd.read_csv("5/Headers.csv")
block_num = headers_df['block_number']
num_of_transactions = headers_df['number_of_transactions']
difficulty = headers_df['difficulty']
time = headers_df['time']

fee_df = pd.read_csv('fee_data_time.csv') #, index_col=0, parse_dates=True
fee = fee_df['fee']
block_num = fee_df['block_num']
tx_num = fee_df['tx_num']
last_block = max(block_num)

non_zero_txs = block_num.value_counts()
fee_dict = {}
for i in range(len(fee_df)):
    if block_num[i] in fee_dict:
        fee_dict[block_num[i]] += [(tx_num[i], fee[i])] # add another tuple to the list of tx num, fee
    
    else: #if the block num not in dict
        fee_dict[block_num[i]] = [(tx_num[i], fee[i])]

# ------------------------------------------------------------------------ the tx weighting is in terms of fee at the moment, not with difficulty :)


app.layout = html.Div([
    html.Div([
                
        dcc.DatePickerRange(  # maybe put a note of what dates are available on screen?
            id='my-date-picker-range',
            day_size = 30,
            min_date_allowed=dt(2009, 1, 3), #Make a max one when we get all the data (max frequently updates with the most recent date data we get
            clearable=True,
            month_format='MM/DD/YYYY',
            end_date_placeholder_text='MM/DD/YYYY',
            start_date=dt(2017, 6, 21).date(),
            end_date=dt(2018, 6, 21).date(),
            
            persistence=True,
            persisted_props=['start_date', 'end_date'],
            persistence_type='local', #can close tab and it'll remember the input
            updatemode='singledate'
            
        ), 
        
        dcc.Input(
            id = 'my_number1',
            type='number',
            placeholder='insert number',
            min = 0, max = last_block, step = 1,
            minLength = 0, maxLength = len(str(last_block)),
            autoComplete='on',
            disabled=False, #remove these falsies ?
            readOnly=False,
            required=False,
            style={'marginRight':'10px'}
            # later put in persistence='' watch his vid 18:50
            
        ),
        
        dcc.Input(
            id = 'my_number2',
            type='number',
            placeholder='insert number',
            debounce=False, #click enter or click out of the box (if you want it to auto change, make it True)
            min = 0, max = last_block, step = 1,
            minLength = 0, maxLength = len(str(last_block)),
            autoComplete='on',
            disabled=False, #remove these falsies ?
            readOnly=False,
            required=False,
            
            # later put in persistence='' watch his vid 18:50
            
        ) 

        ]),
    
    # html.Br(),
    html.H3("Bitcoin Block Visualiser", style={'textAlign': 'center'}),
    dcc.Graph(id="mymap"),
])

# ------------------------------------------------------------------------

@app.callback(
    Output(component_id='mymap', component_property='figure'),
    Input('my-date-picker-range', 'start_date'),
    Input('my-date-picker-range', 'end_date'),
    Input(component_id='my_number1', component_property='value'),
    Input(component_id='my_number2', component_property='value')
)

    
def update_graph(start_date, end_date, num1, num2): 
    print(start_date, end_date)
    frame0 = None
    frames = []


    difficulty_list = [] #getting unique diff vals (of blocks)
    for i in range(len(difficulty)):
        if difficulty[i] not in difficulty_list:
            difficulty_list.append(difficulty[i])
    difficulty_list.sort()

    list_blocks = [] # sorting blocks in terms of difficulty
    # for block in range(num1, num2+1): # do this if u don't care about diff order of blocks
    #     list_blocks.append(block)

    for diff in difficulty_list: #this sorts the blocks in terms of difficulty
        for block in range(num1, num2 +1):
            if difficulty[block] == diff:
                list_blocks.append(block)       
            else:
                pass

    for block in range(num1, num2 +1): #len(block_num)
        txs = num_of_transactions[block]
        block_root = []

        tx_list = []
        for tx_num in range(1, txs +1):
            block_root.append("Block " + str(block)) #change to str of the block
            tx_list.append("Transaction " + str(tx_num))

        
        fee_list = []
        for tuple1 in fee_dict[block]:
            tx_list += [tuple1[0]]
            fee_list += [tuple1[1]]


        treemap = go.Treemap(
            labels = tx_list, 
            values= fee_list, 
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
                            dict(mode='e', frame=dict(redraw=True), transition=dict(duration=200))
                        ],
                        label=f"{block}"
                    )
                    for block in range(num1, num2 +1) #len(block_num)
                ]#,   #IF U want the text of what block it is below the block too
                # transition=dict(duration=0),
                # x=0,
                # y=0,
                # currentvalue=dict(
                #     font=dict(size=12), prefix='Block: ', visible=True, xanchor='center'
                # ),
                # len=1.0,
                # active=True,
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

    return (figure)

# ------------------------------------------------------------------------
if __name__ == '__main__':
    app.run_server(debug=True)
