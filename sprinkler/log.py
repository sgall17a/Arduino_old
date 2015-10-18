import pandas as pd
import matplotlib.pyplot as plt



from pylab import *

#t = arange(0.0, 2.0, 0.01)
#s = sin(2*pi*t)
#plot(t, s)
inp = '/Users/stuartgalloway/Desktop/data.csv'
df = pd.DataFrame.from_csv(inp)
print df.index.max()
print df.index.min()
print df.Temperature.min()
print df.Temperature.max()
df[['Temperature','Humidity']].plot()
xlabel('time (s)')
ylabel('voltage (mV)')
title('About as simple as it gets, folks')
grid(True)
savefig("test.png")
show()
