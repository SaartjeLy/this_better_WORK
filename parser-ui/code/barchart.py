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

app = dash.Dash(__name__)

headers_df = pd.read_csv("5/Headers.csv")
block_num = headers_df['block_number']
last_block = max(block_num)
num_of_transactions = headers_df['number_of_transactions']
difficulty = headers_df['difficulty']
time = headers_df['time']

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

    difficulty_list = [] #sorted list of unique difficulties
    for i in difficulty.unique():
            difficulty_list.append(str(i))
    difficulty_list.sort()
    difficulty_list.reverse()


    diff_count_dict = {} #counts of each difficulty, swap to weight later

    list_blocks = [] # list ALL blocks in terms of difficulty
    for diff in difficulty_list:
        diff_count_dict[diff] = 0
        for block in range(num1, num2 +1):
            if diff == str(difficulty[block]):
                diff_count_dict[str(diff)] += 1
                list_blocks.append(block)       

    counts_list = []
    for i in diff_count_dict.values():
        counts_list.append(i)

    for block in range(num1, num2 +1): #len(list_blocks)
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

    # treemap.update_layout(title={'x': 0.5, 'xanchor': 'center', 'font': {'size': 20}})
    # treemap.update_traces(hovertemplate=
    #                      "<b>%{customdata[0]}</b><br><br>" +
    #                      "Percent of Colonies Impacted: %{customdata[1]:.3s}" +
    #                      "<extra></extra>",
    #   
    # print(frames)
    return (figure)

# ------------------------------------------------------------------------
if __name__ == '__main__':
    app.run_server(debug=True)
