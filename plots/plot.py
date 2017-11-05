import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


def load_csv(filename,key=None):

	data_frame = pd.read_csv(filename)
	data = data_frame.iloc[:,1:].values
	data = np.asarray(data)

	data = data.astype(np.float32)
	return data

if __name__ == "__main__":

    filename = 'time.csv'
    data = load_csv(filename)

    x = np.arange(data.shape[1])+1

    plt.plot(x,data[0],'g-',label='CUDA')
    plt.plot(x,data[1],'b-',label='OpenMP')
    plt.plot(x,data[2],'r-',label='Serial')
    #plt.plot(x,data[3],'k-',label='MPI')
    plt.plot(x,data[0],'go')
    plt.plot(x,data[1],'bo')
    plt.plot(x,data[2],'ro')
    #plt.plot(x,data[3],'ko')
    plt.xticks(x)


    plt.legend()
    plt.xlabel('Images')
    plt.ylabel('Execution Time in seconds')
    plt.savefig('plot'+'.png')
    plt.clf()
    plt.close()

    plt.plot(x,data[0],'g-',label='CUDA')
    plt.plot(x,data[0],'go')
    plt.legend()
    plt.xticks(x)
    plt.xlabel('Images')
    plt.ylabel('Execution Time in seconds')
    plt.savefig('CUDAplot'+'.png')
    plt.clf()
    plt.close()

