import pandas as pd
import pmdarima as pm
import numpy as np
import matplotlib.pyplot as plt
from statsmodels.tsa.stattools import adfuller
from statsmodels.tsa.seasonal import seasonal_decompose 
from sklearn.metrics import mean_absolute_error, mean_squared_error

df = pd.read_csv("csv/people_04_09_v2.csv")
df = df.drop(df[df['_value'] > 2000].index)
df["_time"] = pd.to_datetime(df['_time'])
df.set_index('_time', inplace = True)

ttdiv = int(len(df) * 80 / 100)
periods = len(df) - ttdiv
train = df.iloc[:ttdiv]
test = df.iloc[ttdiv:]

result = adfuller(df['_value'])
if result[0] < result[4]["5%"]:
    print("STATIONARY")
else:
    print("NON-STATIONARY")

decompose = seasonal_decompose(df, model="additive", period=365)
decompose.plot()

arima = pm.auto_arima(train, trace=True, stationary=True, suppress_warnings=True)
arima_prediction = arima.predict(n_periods=periods)
df_arima_prediction = pd.DataFrame(arima_prediction, dtype=np.int8, index = test.index)

plt.plot(test, label="Test")
plt.plot(df_arima_prediction, label="Prediction")
plt.show()

print("MSE:", mean_squared_error(test['_value'], arima_prediction))
print("\nMAE:", mean_absolute_error(test['_value'], arima_prediction))

