from matplotlib import pyplot as plt
import numpy as np
import struct

import rawloadX


pfile = rawloadX.geX("P26112.7",display=True)
data = pfile.rawloadX()

print(data.shape)
filename = "P26113.7"
exam = 2020110305 # 4 bytes
ncoils = 16 # 2 bytes
ny = 256 # 2 bytes
nx = 256 # 2 bytes
hdr_size = 1506 + 4 + 60# 4 bytes

data = np.fft.fft(data, axis=1)
data = np.fft.fftshift(data, axes=(1))
data_write = np.copy(data[:,5,0,:,:].squeeze())
data_size = data_write.size*2*4 # 4 bytes

f = open(filename, "wb")
f.write(struct.pack("<494s",b"junk")) # offset 0
f.write(struct.pack("<L",exam)) # offset 494
f.write(struct.pack("<738s",b"junk")) # offset 498
f.write(struct.pack("<H",ncoils)) # offset 1236
f.write(struct.pack("<200s",b"junk")) # offset 1238
f.write(struct.pack("<H",ny)) # offset 1438
f.write(struct.pack("<H",nx)) # offset 1440
f.write(struct.pack("<60s",b"junk")) # offset 1442
f.write(struct.pack("<I",hdr_size)) # offset 1502
f.write(struct.pack("<L",data_size)) # offset 1506
f.write(struct.pack("<60s",b"junk")) # offset 1442


images = np.fft.fft2(data_write, axes=(1, 2))

for n in range(images.shape[0]):
    plt.figure()
    plt.imshow(abs(images[n,:,:].squeeze()),
               cmap="gray")

plt.show()

data_write = data_write.flatten()
data_write_int = np.zeros(2*data_write.size)
data_write_int[::2] = data_write.real
data_write_int[1::2] = data_write.imag
data_write_int = data_write_int.astype(np.int32)

data_write_int.tofile(f)

f.close()

f = open(filename, "rb")
f.seek(494)
print(struct.unpack_from("<L",f.read(4)))
f.seek(1236)
print(struct.unpack_from("<H",f.read(2)))
f.seek(1438)
print(struct.unpack_from("<H",f.read(2)))
f.seek(1440)
print(struct.unpack_from("<H",f.read(2)))
f.seek(1502)
print(struct.unpack_from("<I",f.read(4)))
f.seek(1506)
print(struct.unpack_from("<L",f.read(4)))









data = data_write_int[::2] + 1j*data_write_int[1::2]
data = data.reshape(16,256,256)
print(data[3,100,100])

data = np.fft.ifft2(data, axes=(1,2))
data = np.fft.fftshift(data, axes=(1,2))
image = abs(np.sqrt(np.sum(data*np.conjugate(data),axis=0)))
print(image.shape)

plt.figure()
plt.imshow(image.squeeze().transpose(1,0),
           cmap="gray")
plt.show()


# # raw_data = np.copy(data[:,20,1,:,:])

# data = np.fft.ifft2(data, axes=(3,4))
# data = np.fft.fftshift(data, axes=(3,4))
# image = abs(np.sqrt(np.sum(data*np.conjugate(data),axis=0)))
# print(image.shape)

# for ii in range(44):
    
#     plt.figure()
#     plt.imshow(image[ii,0,:,:,].squeeze().transpose(1,0),
#                cmap="gray")
#     plt.title("slice %i" % ii)
#     plt.show()
