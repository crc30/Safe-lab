import pandas as pd
import matplotlib.pyplot as plt
from sklearn.metrics import mean_absolute_error, mean_squared_error

df = pd.read_csv("csv/people_holtwinters_data_v2.csv")
df["_time"] = pd.to_datetime(df['_time'])
df.set_index('_time', inplace = True)
df['_value'].plot(figsize=(12,5))

df2 = pd.read_csv("csv/people_holtwinters_data_predictions_v2.csv")
df2["_time"] = pd.to_datetime(df2['_time'])
df2.set_index('_time', inplace = True)
df2['_value'].plot(figsize=(12,5))

print("MSE:", mean_squared_error(df['_value'], df2['_value']))
print("\nMAE:", mean_absolute_error(df['_value'], df2['_value']))
plt.show()