import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load CSVs produced by ./main mode=sim
paths = pd.read_csv('out/sim_paths.csv')               # columns: path, step, S
prices = pd.read_csv('out/sim_option_prices.csv')      # columns: path, step, C
he = pd.read_csv('out/sim_hedge_errors.csv')           # columns: path, HE_T

# ---- ALL STOCK PATHS ON ONE PLOT ----
fig, ax = plt.subplots()
for pid, grp in paths.groupby('path', sort=True):
    g = grp.sort_values('step')
    ax.plot(g['step'], g['S'], linewidth=0.8, alpha=0.5)
ax.set_xlabel('Step')
ax.set_ylabel('Stock Price')
ax.set_title('100 Simulated Stock Paths')
fig.tight_layout()
fig.savefig('out/fig_paths_all.png')
plt.close(fig)

# ---- ALL OPTION PRICE SERIES ON ONE PLOT ----
fig, ax = plt.subplots()
for pid, grp in prices.groupby('path', sort=True):
    g = grp.sort_values('step')
    ax.plot(g['step'], g['C'], linewidth=0.8, alpha=0.5)
ax.set_xlabel('Step')
ax.set_ylabel('Call Price')
ax.set_title('100 corresponding Option Price Series')
fig.tight_layout()
fig.savefig('out/fig_option_prices_all.png')
plt.close(fig)

# ---- HISTOGRAM OF TERMINAL HEDGING ERRORS ----

vals = he['HE_T'].values
mu   = np.mean(vals)
var  = np.var(vals)          # population variance
sigma = np.sqrt(var)

fig, ax = plt.subplots()

# Histogram (counts, not density)
counts, bins, _ = ax.hist(vals, bins=50)

# Normal PDF scaled to histogram counts
bin_width = bins[1] - bins[0]
x = np.linspace(bins[0], bins[-1], 500)
pdf = (1.0 / (sigma * np.sqrt(2.0 * np.pi))) * np.exp(-0.5 * ((x - mu) / sigma)**2)
ax.plot(x, pdf * len(vals) * bin_width)  # scale to match histogram counts

ax.set_xlabel('Terminal Hedging Error (HE_T)')
ax.set_ylabel('Frequency')
ax.set_title('Distribution of Hedging Errors at T with Normal Fit')

# Mean/variance annotation inside the plot
ax.text(
    0.98, 0.95,
    f"mean = {mu:.6f}\nvariance = {var:.6f}",
    transform=ax.transAxes, ha='right', va='top',
    bbox=dict(boxstyle='round', fc='white', ec='0.5', alpha=0.8)
)

fig.tight_layout()

# ensure output dir exists or change to a flat filename
# import os; os.makedirs('out', exist_ok=True)
fig.savefig('out/fig_HE_hist.png')
plt.close(fig)