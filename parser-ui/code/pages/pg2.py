import dash
from dash import dcc, html
import plotly.graph_objects as go
import pandas as pd
from dash import dcc, html, Output, Input, callback, dash_table
from dash import html
from dash.dependencies import Input, Output
from dash import dcc
from datetime import datetime as dt
from datetime import date

app = dash.Dash(__name__)

dash.register_page(__name__)

headers_df = pd.read_csv("/home/dev/Documents/Parser/5/Headers.csv")
# block_num = headers_df['block_number']
# num_of_transactions = headers_df['number_of_transactions']
difficulty = headers_df['difficulty']
time = headers_df['time']
fee_df = pd.read_csv('/home/dev/Documents/Parser/fee_calendar-time.csv') #, index_col=0, parse_dates=True
fee = fee_df['fee']
block_num = fee_df['block_num']
tx_num = fee_df['tx_num']
time = fee_df['time']
last_block = max(block_num)


non_zero_txs = block_num.value_counts()
fee_dict = {}
for i in range(len(fee_df)):
    if block_num[i] in fee_dict:
        fee_dict[block_num[i]] += [(tx_num[i], fee[i])] # add another tuple to the list of tx num, fee
    
    else: #if the block num not in dict
        fee_dict[block_num[i]] = [(tx_num[i], fee[i])]


layout = html.Div([
    dcc.Markdown('*Range available: *' + str(time[0]) + ' to ' + str(time[len(time) - 1])),
    html.Div([
        dcc.DatePickerRange(  # maybe put a note of what dates are available on screen?
            id='my-date-picker-range',
            day_size = 30,
            min_date_allowed=dt(2009, 1, 3), #Make a max one when we get all the data (max frequently updates with the most recent date data we get
            clearable=True,
            month_format='DD/MM/YYYY',
            end_date_placeholder_text='DD/MM/YYYY',
            start_date=dt(2017, 6, 21).date(),
            end_date=dt(2018, 6, 21).date(),
            
            persistence=True,
            persisted_props=['start_date', 'end_date'],
            persistence_type='local', #can close tab and it'll remember the input
            updatemode='singledate'
            
        ), 
    ]),
    html.H3("Bitcoin Block Visualiser", style={'textAlign': 'center'}),
    dcc.Graph(id="mymap1"),
])

@dash.callback(
    Output(component_id='mymap1', component_property='figure'),
    Input('my-date-picker-range', 'start_date'),
    Input('my-date-picker-range', 'end_date'),
    
)

def update(start_date, end_date):
    start_1 = start_date[:4]
    start_2 = start_date[5:7]
    start_3 = start_date[8:]
    end_1 = end_date[:4]
    end_2 = end_date[5:7]
    end_3 = end_date[8:]
    start_date = f'{start_1}-{start_3}-{start_2}'
    end_date = f'{end_1}-{end_3}-{end_2}'
    

    first_date_occurance = time.where(time== start_date).first_valid_index()
    last_date_occurance = time.where(time== end_date).last_valid_index()
    num1 = int(block_num[first_date_occurance])
    #figure out why it's saying none, and why it says 0 when it's any start
    num2 = int(block_num[last_date_occurance])
    
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
        block_root = [] 
        display_txs = [] #the non-zero fees
        
        fee_list = []
        for tuple1 in fee_dict[block]: #layout in tuple is tx_num, fee
            display_txs += [tuple1[0]]
            fee_list += [tuple1[1]]
            
        tx_list = []
        for index in range(len(display_txs)):
            block_root.append("Block " + str(block)) #change to str of the block
            tx_list.append("Transaction " + str(display_txs[index]))
            

        
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
        # 'title': "Animation of bitcoin blocks",
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
