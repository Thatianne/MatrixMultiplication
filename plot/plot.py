# -*- coding: utf-8 -*-
"""
Spyder Editor

Este é um arquivo de script temporário.
"""

import pandas as pd
import matplotlib.pyplot as plt

path = "/home/gabriel/Dropbox/log.csv"
df = pd.read_csv(path, sep=',')

df_summa = df[df.algoritmo == 'summa_mpi']

i = [5, 7, 9, 10, 11, 12, 13]
dfs_summa_np = {}

for k in i:
    dfs_summa_np[str(k)] = df_summa[df_summa.np == k]
    
j = [64,91,128,181,256,362,512,724,1024,1448,2048,2896,4096,5793,8192]
dfs_summa_n = {}
dfs_summa_n_array = []

plt.figure(figsize=(11,10))
for k in j:
    dfs_summa_n[str(k)] = df_summa[df_summa.n == k]
    
    ts = df_summa[df_summa.n == k].temp_max.values[0]
    dfs_summa_n[str(k)]['aceleracao'] = ts/(dfs_summa_n[str(k)]['temp_max'].values)
    dfs_summa_n[str(k)]['eficiencia'] = dfs_summa_n[str(k)]['aceleracao']/(dfs_summa_n[str(k)]['np']/5)
    dfs_summa_n_array.append(df_summa[df_summa.n == k].temp_max.values)
    plt.plot(i, df_summa[df_summa.n == k].temp_max.values)
plt.legend(list(map(lambda x: str(x), j)))
plt.xlabel('Número de processos')
plt.ylabel('Tempo (s)')
plt.show()


df_summa_omp = df[df.algoritmo == 'summa_mpi_omp']

i = [5, 7, 9, 10, 11, 12, 13]
dfs_summa_omp_np = {}

for k in i:
    dfs_summa_omp_np[str(k)] = df_summa_omp[df_summa_omp.np == k]
    
dfs_summa_omp_n = {}
dfs_summa_omp_n_array = []

plt.figure(figsize=(11,10))
for k in j:
    dfs_summa_omp_n[str(k)] = df_summa_omp[df_summa_omp.n == k]
    dfs_summa_omp_n_array.append(df_summa_omp[df_summa_omp.n == k].temp_max.values)
    plt.plot(i, df_summa_omp[df_summa_omp.n == k].temp_max.values)
plt.legend(list(map(lambda x: str(x), j)))
plt.xlabel('Número de processos')
plt.ylabel('Tempo (s)')
plt.show()


