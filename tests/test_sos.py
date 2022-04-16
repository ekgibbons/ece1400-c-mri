import struct

import numpy as np

import utils

    
size_choice = [32, 64, 128]
size_y = np.random.choice(size_choice)
size_x = np.random.choice(size_choice)
size_coil = np.random.randint(0,5)+1
im_re = np.random.uniform(-200, 200,
                          size=(size_coil,size_y,size_x))
im_im = np.random.uniform(-200, 200,
                          size=(size_coil,size_y,size_x))

im_cpx = im_re + 1j*im_im

utils.write_bin("data.bin",np.copy(im_cpx), "double")

im_sos = np.sqrt(np.sum(im_cpx*np.conjugate(im_cpx),
                        axis=0))

im_sos *= 255/np.amax(im_sos)

utils.write_bin("im.bin",im_sos[None,:,:], "int")
