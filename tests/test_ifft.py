import struct

import numpy as np

import utils

    
size_choice = [32, 64, 128]
size_y = np.random.choice(size_choice)
size_x = np.random.choice(size_choice)
size_coil = np.random.randint(0,5)+1
im = np.random.uniform(-100, 100,
                       size=(size_coil,size_y,size_x))
                           

data = np.fft.fft2(im,axes=(1,2))
data = np.fft.fftshift(data,axes=(1,2))

utils.write_bin("data.bin",np.copy(data), "int")

im = np.fft.ifft2(data, axes=(1,2))

utils.write_bin("im.bin",im, "double")


