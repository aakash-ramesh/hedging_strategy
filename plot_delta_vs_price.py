import pandas as pd
import matplotlib.pyplot as plt

res = pd.read_csv('result.csv')
res['date'] = pd.to_datetime(res['date'])

fig, ax1 = plt.subplots()
ax1.plot(res['date'], res['S'], label='S')
ax1.set_ylabel('S')
ax2 = ax1.twinx()
ax2.plot(res['date'], res['delta'], label='Delta', linestyle='--')
ax1.set_xlabel('Date')
plt.title('Price vs Delta')
fig.tight_layout(); plt.savefig('out/fig_delta_vs_price.png'); plt.close()
