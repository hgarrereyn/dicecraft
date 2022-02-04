
import matplotlib.pyplot as plt
import numpy as np

im = plt.imread('./res/tilemap.png').reshape(-1)
im = (im * 255.0).astype(np.uint8)

dat = ''
dat += 'unsigned char data[] = {'
dat += ','.join([str(x) for x in im])
dat += '};\n'

open('./src/emb_tilemap.h', 'w').write(dat)
