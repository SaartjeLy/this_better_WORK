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
difficulty = headers_df['difficulty']
time = headers_df['time']
block_num = headers_df['block_number']

frame0 = None
frames = []

difficulty_list = [] #sorted list of unique difficulties
for i in difficulty.unique():
        difficulty_list.append(str(i))
difficulty_list.sort()
difficulty_list.reverse()


diff_count_dict = {} #counts of each difficulty, swap to weight later

list_blocks = [] # list ALL blocks in terms of difficulty
for diff in difficulty_list:
    diff_count_dict[diff] = 0
    for block in range(len(block_num)):
        if diff == str(difficulty[block]):
            diff_count_dict[str(diff)] += 1
            list_blocks.append(block)       

counts_list = []
for i in diff_count_dict.values():
    counts_list.append(i)

for block in range(200): #len(list_blocks)
    for diff in range(len(difficulty_list)):
        if difficulty_list[diff] == str(difficulty[block]):
            n = diff
            break
   
    colors = ['lightslategray',] * 5
    colors[n] = 'crimson'

    bar_chart = go.Figure(data=[go.Bar(
        x=difficulty_list,
        y=counts_list,
        marker_color=colors # marker color can be a single color value or an iterable
    )])
    bar_chart.update_layout(title_text='Least Used Feature', bargap = 0.2) 

    
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

figure.show()