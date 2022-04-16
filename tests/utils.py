import struct

import numpy as np


def write_bin(filename,data,dtype="int"):
    f = open(filename,"wb")
    f.write(struct.pack("<i",data.shape[0]))
    f.write(struct.pack("<i",data.shape[1]))
    f.write(struct.pack("<i",data.shape[2]))
    
    data = data.flatten()
    data_int = np.zeros(2*data.size)
    data_int[::2] = data.real
    data_int[1::2] = data.imag

    if dtype == "int":
        data_int = data_int.astype(np.int32)
    elif dtype == "double":
        data_int = data_int.astype(np.float64)
    
    data_int.tofile(f)

    f.close()
