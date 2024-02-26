import numpy as np

# Load the .npy file
data = np.load("fall_data.npy")

# Iterate over each sub-array in the main array
for sub_array in data:
    # Print the entire content of the sub-array
    print(sub_array)
