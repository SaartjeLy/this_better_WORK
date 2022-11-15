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
from dash import dcc
from dash import html
from dash.dependencies import Input, Output

from collections import Counter

app = dash.Dash(__name__)

headers_df = pd.read_csv("5/Headers.csv")
block_num = headers_df['block_number']
last_block = max(block_num)
num_of_transactions = headers_df['number_of_transactions']
difficulty = headers_df['difficulty']
time = headers_df['time']

transaction_df = pd.read_csv("5/transactions.csv")
num_outputs_list = transaction_df['num_outputs']

outputs_df = pd.read_csv("5/Outputs.csv")
satoshi_val = outputs_df['value']



# ------------------------------------------------------------------------


app.layout = html.Div([
    html.Div([
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

    html.Br(),
    dcc.Graph(id="mymap"),
])

# ------------------------------------------------------------------------

@app.callback(
    Output(component_id='mymap', component_property='figure'),
    Input(component_id='my_number1', component_property='value'),
    Input(component_id='my_number2', component_property='value')
)


def update_graph(num1, num2): #, num2
    
    frame0 = None
    frames = []
    satoshi_value_list = []
    block_tx_num_dict = {}
    
    for block in range(num1, num2):
        tx_num_counter = 0
        output_list_increment = 0
        num_transactions = num_of_transactions[block]
        for tx_i in range(tx_num_counter, tx_num_counter + num_transactions):
               
            num_outputs = num_outputs_list[tx_i]
            output_counter = 1
            for output_i in range(output_list_increment, output_list_increment + num_outputs): 
                satoshi_value = satoshi_val[output_i]
                satoshi_value_list.append(str(satoshi_value))
                
                if satoshi_value in block_tx_num_dict:
                    block_tx_num_dict[satoshi_value][0] +=1
                    block_tx_num_dict[satoshi_value] += [f'block num {block}:', f' tx {tx_i}']
                else:
                    block_tx_num_dict[satoshi_value] = [1, f'block num {block}:', f' tx {tx_i}']

                output_counter +=1
            output_list_increment += num_outputs
            
    
    sort_frequency = [n for n,count in Counter(satoshi_value_list).most_common() for i in range(count)]
    # sort_frequency = [0] + sort_frequency 
    freq_pd_series = pd.Series(sort_frequency)
    
    unique_freq = freq_pd_series.unique()
    
    counts_list = []
    counts = 1
    for i in range(1, len(unique_freq)):
        if unique_freq[i] == unique_freq[i-1]:
            counts +=1
        else:
            counts_list.append(counts)
            counts = 1

    
    bar_chart = go.Figure(data=[go.Bar(
        x=freq_pd_series,
        y=counts_list,
        
    # marker_color=colors # marker color can be a single color value or an iterable
    )])
    
    bar_chart.update_layout(title_text='Least Used Feature', bargap = 0.2) 
    bar_chart.update_traces(hovertemplate = tx_i)
    if frame0 is None:     
        frame0 = bar_chart

            
    frames.append(go.Frame(name=f"frame-{block}", data=bar_chart['data'][0]))
    
    layout = go.Layout(
        xaxis = dict(
            rangeslider = {'visible': True},
        ),
    )  
    
    figure= go.Figure(
    data=frame0['data'][0],
    layout=layout,
    frames=frames
    )
    
    return (figure)
    #-----------------------------------

    # for block in range(num1, num2 +1): #len(list_blocks)
    #     for diff in range(len(difficulty_list)):
    #         if difficulty_list[diff] == str(difficulty[block]):
    #             n = diff
    #             break
   
    #     colors = ['lightslategray',] * 5
    #     colors[n] = 'crimson'

    

# ------------------------------------------------------------------------
if __name__ == '__main__':
    app.run_server(debug=True)
