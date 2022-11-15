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
                
        dcc.DatePickerRange(
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
            persistence_type='local', #xan close tab and it'll remember the input
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

    
def update_graph(num1, num2): 

    dff = headers_df.copy()
            
    frame0 = None
    frames = []


    difficulty_list = []
    for i in range(len(difficulty)):
        if difficulty[i] not in difficulty_list:
            difficulty_list.append(difficulty[i])
    difficulty_list.sort()

    list_blocks = [] # sorting blocks in terms of difficulty

    for diff in difficulty_list:
        for block in range(num1, num2 +1):
            if difficulty[block] == diff:
                list_blocks.append(block)       
            else:
                pass

    for block in range(num1, num2 +1): #len(block_num)
        txs = num_of_transactions[block]
        block_root = []


        #using time as a placeholder, it gives block time for each tx which is wrong (4 now!) :)
        df_total = headers_df[['block_number', 'difficulty', 'number_of_transactions', 'time']]

    # mask = block #try block_num or block_n
    # df_temp = (
    #     df_total.loc[mask]
    #     # .dropna(subset="time")[['number_of_transactions', 'time']] #IMPLEMENT L8R?
    #     # .fillna("")
    # ) #0 = block_num, 1 = num tx, 2 = time


        tx_list = []
        for tx_num in range(1, txs +1):
            block_root.append("Block " + str(block)) #change to str of the block
            tx_list.append("Transaction " + str(tx_num))
            
        times_list = [] #increment thru the list later
        time_i_increment = 0 
        for i in range(time_i_increment, time_i_increment + txs):
            times_list.append(time[i])
        time_i_increment += txs


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
                            dict(mode='e', frame=dict(redraw=True), transition=dict(duration=200))
                        ],
                        label=f"{block}"
                    )
                    for block in range(num1, num2 +1) #len(block_num)
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
