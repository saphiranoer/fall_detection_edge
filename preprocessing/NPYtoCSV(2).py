import numpy as np
import pandas as pd

# Load the .npy file
data = np.load("fall_data.npy")

# Convert the data to float64
data = data.astype(np.float64)

# Transpose the array if needed
data = data.T

# Assuming each row represents 20ms, for 3000ms data we need 150 rows per segment
rows_per_segment = 150

# Create a range of numbers that increments by 20 starting from 0
numbers = np.arange(0, 453*20, 20)

# Loop through the columns of the data array
for i in range(0, data.shape[1], 3):
    # Select the current 3 columns of data (x, y, z)
    column_data = data[:, i:i+3]
    
    # Calculate the magnitude
    magnitudes = np.sqrt(np.sum(column_data**2, axis=1))
    
   # Split the data into 3 equal parts, each representing 3000ms
    for part in range(3):
        start_row = part * rows_per_segment
        end_row = start_row + rows_per_segment
        segment_data = column_data[start_row:end_row]
        segment_magnitudes = magnitudes[start_row:end_row]
        
        # Create a new timestamp array starting from 0 for each segment
        segment_timestamps = np.arange(0, len(segment_data)*20, 20)
        
        # Create a DataFrame for the segment
        df_segment = pd.DataFrame(segment_data, columns=["x", "y", "z"])
        df_segment.insert(0, "timestamp", segment_timestamps)
        df_segment['m'] = segment_magnitudes
        
        # Save the DataFrame to a CSV file
        df_segment.to_csv(f"fall.{i//3}_{part}.csv", index=False)
