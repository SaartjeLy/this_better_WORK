import dash
import dash_html_components as html
import dash_core_components as dcc
# from dash import html, dcc
import plotly.graph_objects as go
import pandas as pd
import plotly.express as px
from dash.dependencies import Input, Output

# Load data
df = pd.read_csv('6_blocks_fee_data.csv', index_col=0, parse_dates=True)

# Initialise the app
app = dash.Dash(__name__)
app.config.suppress_callback_exceptions = True

# Define the app
app.layout = html.Div(
    children=[
        html.Div(className='row',
                 children=[
                    html.Div(className='four columns div-user-controls',
                             children=[
                                 html.H2('DASH - STOCK PRICES'),
                                 html.P('Visualising time series with Plotly - Dash.'),
                                 html.P('Pick one or more stocks from the dropdown below.'),
                                 html.Div(
                                     className='div-for-dropdown',
                                     children=[
                                         dcc.Dropdown(id='stockselector', options=get_options(df['stock'].unique()),
                                                      multi=True, value=[df['stock'].sort_values()[0]],
                                                      style={'backgroundColor': '#1E1E1E'},
                                                      className='stockselector'
                                                      ),
                                     ],
                                     style={'color': '#1E1E1E'})
                                ]
                             ),
                    html.Div(className='eight columns div-for-charts bg-grey',
                             children=[
                                 dcc.Graph(id='timeseries',
                                     config={'displayModeBar': False},
                                     animate=True),
                                 dcc.Graph(id='change',
                                     config={'displayModeBar': False},
                                     animate=True),
                             ])
                              ])
        ]

)


# Run the app
if __name__ == '__main__':
    app.run_server(debug=True)
    
    
    
#------------------------------------
