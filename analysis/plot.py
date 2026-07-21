import pandas as pd 
import matplotlib.pyplot as plt

#load the recorded data from read_serial
df = pd.read_csv('../data/walkTrial.csv')

#plot the data
plt.figure(figsize=(10, 5))
plt.plot(df['Angle'], label ='Cane Tilt Angle')
plt.xlabel('Time')
plt.ylabel('Angle (degrees)')
plt.title('Cane Tilt Angle Over Time')
plt.legend()
plt.grid(True)
plt.show()

#plot step data 
plt.figure(figsize=(10,5))
plt.plot(df['Steps'], label='Detected Steps', color='orange')
plt.xlabel('Sample Number')
plt.ylabel('Step Count')
plt.title('Steps Detected Over Time')
plt.legend()
plt.grid(True)
plt.show()