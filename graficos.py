import matplotlib.pyplot as plt
import csv
  
time = []
bucket_width = []
alg = []
binary = []
binomial = []
binary = []
binomial = []

with open('losdatos.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter = ',')      
    for row in plots:
        n.append(int(row[0]))
        binary.append(1000*float(row[1]))
        binomial.append(1000*float(row[2]))
  
plt.plot(n, binary, color = 'g', label = "Binary Heap")
plt.plot(n, binomial, color = 'r', label = "Binomial Heap")
plt.xlabel('Número de elementos')
plt.ylabel('Tiempo(ms)')
plt.title('Comparación Merge: Binary Heap v/s Binomial Heap')
plt.legend()
plt.show()