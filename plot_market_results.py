import pandas as pd
import matplotlib.pyplot as plt

res = pd.read_csv('out/result.csv')
res['date'] = pd.to_datetime(res['date'])

fig, ax = plt.subplots()
ax.plot(res['date'], res['S'], label='Stock (S)')
ax.set_xlabel('Date'); ax.set_ylabel('Stock Price'); ax.legend(loc='upper left')
plt.title('Underlying Price Over Hedge Window'); plt.tight_layout(); plt.savefig('out/fig_market_S.png'); plt.close()

fig, ax = plt.subplots()
ax.plot(res['date'], res['V'], label='Option Mid (V)')
ax.set_xlabel('Date'); ax.set_ylabel('Call Mid Price'); ax.legend(loc='upper left')
plt.title('Option Quote Over Hedge Window'); plt.tight_layout(); plt.savefig('out/fig_market_V.png'); plt.close()

fig, ax = plt.subplots()
ax.plot(res['date'], res['imp_vol'], label='Implied Volatility')
ax.set_xlabel('Date'); ax.set_ylabel('IV'); ax.legend(loc='upper left')
plt.title('Implied Volatility (per-day)'); plt.tight_layout(); plt.savefig('out/fig_market_IV.png'); plt.close()

fig, ax = plt.subplots()
ax.plot(res['date'], res['delta'], label='Delta')
ax.set_xlabel('Date'); ax.set_ylabel('Delta'); ax.legend(loc='upper left')
plt.title('Call Delta (from IV)'); plt.tight_layout(); plt.savefig('out/fig_market_delta.png'); plt.close()

fig, ax = plt.subplots()
ax.plot(res['date'], res['PNL'], label='PNL (unhedged)')
ax.plot(res['date'], res['PNL_with_hedge'], label='PNL (with hedge)')
ax.set_xlabel('Date'); ax.set_ylabel('PNL'); ax.legend()
plt.title('Unhedged vs Hedged PNL'); plt.tight_layout(); plt.savefig('out/fig_market_pnl_vs_hedged.png'); plt.close()
