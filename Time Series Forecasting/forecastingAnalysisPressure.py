import pandas as pd
import pmdarima as pm
import numpy as np
from influxdb_client import InfluxDBClient
from statsmodels.tsa.arima_model import ARIMA
import datetime
import warnings
warnings.filterwarnings("ignore")

my_token = "********"
my_org = "********" 
bucket = "********"
url = "********"

query = '''
from(bucket: "ProjectIot")
|> range(start: -10d)
|> filter(fn: (r) => r["_measurement"] == "Pressure")
|> filter(fn: (r) => r["_field"] == "value")
|> aggregateWindow(every: 5m, fn: mean, createEmpty: false)
'''
client = InfluxDBClient(url=url, token=my_token, org=my_org, debug=False)
dataframe = client.query_api().query_data_frame(org=my_org, query=query)

dataframe = dataframe.drop(columns=['result', 'table', '_start', '_stop', '_field', '_measurement'])

start_date = dataframe['_time'].iloc[-2]

dataframe["_time"] = pd.to_datetime(dataframe['_time'])

dataframe.set_index('_time', inplace = True)


len_dataframe = len(dataframe)

get_order_arima_model = pm.auto_arima(dataframe, seasonal=True)

arima_model = ARIMA(dataframe,order=get_order_arima_model.order).fit()

n_periods = 12
freq = '5T'

prepare = pd.date_range(start = start_date, periods = n_periods, freq = freq).tz_convert('Europe/Rome')
prediction = arima_model.predict(start = len_dataframe, end = len_dataframe + (n_periods - 1), typ='levels') 

_write_client = client.write_api()

_data_frame = pd.DataFrame(prediction, columns=['presvaluepredicted'])
_data_frame.insert(0, "_time", prepare)

_data_frame["_time"] = pd.to_datetime(_data_frame['_time'])
_data_frame.set_index('_time', inplace = True)

_write_client.write(bucket, my_org, record = _data_frame, data_frame_measurement_name = 'PredictionPressure')

_write_client.close()
client.close()