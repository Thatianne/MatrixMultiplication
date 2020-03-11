# -*- coding: utf-8 -*-
"""
Spyder Editor

Este é um arquivo de script temporário.
"""

import pandas as pd
import matplotlib.pyplot as plt

path = "/home/gabriel/Dropbox/log.csv"
path = "/home/gabriel/Downloads/summa_serial_otimizado.csv"

df = pd.read_csv(path, sep=',')
df2 = pd.read_csv('/home/gabriel/Downloads/classic_otimizado.csv')

df_summa = df[df.algoritmo == 'summa_mpi']

i = [5, 7, 9, 10, 11, 12, 13]
dfs_summa_np = {}

for k in i:
    dfs_summa_np[str(k)] = df_summa[df_summa.np == k]
    
j = [64,91,128,181,256,362,512,724,1024,1448,2048,2896,4096,5793,8192]
dfs_summa_n = {}
dfs_summa_n_array = []

fig = plt.figure()
ux = plt.subplot(111)
for k in j:
    dfs_summa_n[str(k)] = df_summa[df_summa.n == k]

    ts = df_summa[df_summa.n == k].temp_max.values[0]
    dfs_summa_n[str(k)]['aceleracao'] = ts/(dfs_summa_n[str(k)]['temp_max'].values)
    dfs_summa_n[str(k)]['eficiencia'] = dfs_summa_n[str(k)]['aceleracao']/(dfs_summa_n[str(k)]['np']/5)
    
    fig = plt.figure()
    ax = plt.subplot(111)
    ax.plot(i, dfs_summa_n[str(k)]['eficiencia'].values)
    plt.title('Eficiência ('+str(k)+') - Summa MPI')
    plt.xlabel('Número de processadores')
    plt.ylabel('Tempo (s)')
    fig.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/eficiencia'+str(k)+'_summaMPI.jpg', dpi=400)
    
    
fig2 = plt.figure(figsize=(11,10))
for k in j:
    plt.plot(i, dfs_summa_n[str(k)]['temp_max'].values, label=str(k))
plt.legend()
plt.xlabel('Número de processadores')
plt.title('Tempo de usuário de todos os trabalhos - Summa MPI')
plt.ylabel('Tempo (s)')
fig2.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/tempo_de_usuario_summaMPI', dpi=400)




df_summa_omp = df[df.algoritmo == 'summa_mpi_omp']

i = [5, 7, 9, 10, 11, 12, 13]
dfs_summa_omp_np = {}

for k in i:
    dfs_summa_omp_np[str(k)] = df_summa_omp[df_summa_omp.np == k]
    
dfs_summa_omp_n = {}
dfs_summa_omp_n_array = []

fig = plt.figure()
ux = plt.subplot(111)
for k in j:
    dfs_summa_omp_n[str(k)] = df_summa_omp[df_summa_omp.n == k]
    ts = df_summa_omp[df_summa_omp.n == k].temp_max.values[0]
    dfs_summa_omp_n[str(k)]['processadores'] = 2*dfs_summa_omp_n[str(k)]['np']
    dfs_summa_omp_n[str(k)]['aceleracao'] = ts/(dfs_summa_omp_n[str(k)]['temp_max'].values)
    dfs_summa_omp_n[str(k)]['eficiencia'] = dfs_summa_omp_n[str(k)]['aceleracao']/(dfs_summa_omp_n[str(k)]['processadores']/10)
    
    fig = plt.figure()
    ax = plt.subplot(111)
    ax.plot(dfs_summa_omp_n[str(k)]['processadores'].values, dfs_summa_omp_n[str(k)]['eficiencia'].values)
    plt.title('Eficiência ('+str(k)+') - Summa MPI OMP (2 trheads)')
    plt.xlabel('Número de processadores')
    plt.ylabel('Eficiência')
    fig.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/eficiencia'+str(k)+'_summaMPI-OMP.jpg', dpi=400)

fig2 = plt.figure(figsize=(11,10))
for k in j:
    plt.plot(dfs_summa_omp_n[str(91)]['processadores'].values, dfs_summa_omp_n[str(k)]['temp_max'].values, label=str(k))
    
    
plt.legend()
plt.xlabel('Número de processadores')
plt.ylabel('Tempo (s)')
plt.title('Tempo de usuário de todos os trabalhos - Summa MPI-OMP (2 trheads)')
fig2.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/tempo_de_usuario_summaMPI-OMP.png', dpi=400)


df_summa_omp = df[df.algoritmo == 'strassen_mpi']

i = [7, 9, 10, 11, 12, 13]
j = [64,91,128,181,256,362,512,724]
dfs_summa_omp_np = {}

for k in i:
    dfs_summa_omp_np[str(k)] = df_summa_omp[df_summa_omp.np == k]
    
dfs_summa_omp_n = {}
dfs_summa_omp_n_array = []

fig = plt.figure()
ux = plt.subplot(111)
for k in j:
    dfs_summa_omp_n[str(k)] = df_summa_omp[df_summa_omp.n == k]
    ts = df_summa_omp[df_summa_omp.n == k].temp_max.values[0]
    min_p = df_summa_omp[df_summa_omp.n == k].np.values[0]
    dfs_summa_omp_n[str(k)]['aceleracao'] = ts/(dfs_summa_omp_n[str(k)]['temp_max'].values)
    dfs_summa_omp_n[str(k)]['eficiencia'] = dfs_summa_omp_n[str(k)]['aceleracao']/(dfs_summa_omp_n[str(k)]['np']/7)
    
    fig = plt.figure()
    ax = plt.subplot(111)
    ax.plot(dfs_summa_omp_n[str(k)]['np'].values, dfs_summa_omp_n[str(k)]['eficiencia'].values)
    plt.title('Eficiência ('+str(k)+') - Strassen MPI')
    plt.xlabel('Número de processadores')
    plt.ylabel('Eficiência')
    fig.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/eficiencia'+str(k)+'_strassenMPI', dpi=400)

fig2 = plt.figure(figsize=(11,10))
for k in j:
    plt.plot(dfs_summa_omp_n[str(91)]['np'].values, dfs_summa_omp_n[str(k)]['temp_max'].values, label=str(k))
    
    
plt.legend()
plt.xlabel('Número de processadores')
plt.ylabel('Tempo (s)')
plt.title('Tempo de usuário de todos os trabalhos - Strassen MPI')
fig2.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/tempo_de_usuario_strassenMPI', dpi=400)



df_summa_omp = df[df.algoritmo == 'classic_otimizado']

i = [7, 9, 10, 11, 12, 13]
j = [64,91,128,181,256,362,512,724,1024,1448,2048,2896,4096,5793,8192]

dfs_summa_omp_np = {}

for k in i:
    dfs_summa_omp_np[str(k)] = df_summa_omp[df_summa_omp.np == k]
    
dfs_summa_omp_n = {}
dfs_summa_omp_n_array = []

fig2 = plt.figure(figsize=(11,10))
plt.plot(df['n'].values, df['tempo_exec'].values, label = 'Summa Serial')
plt.plot(df['n'].values, df2['tempo_exec'].values, label = 'Classic')

plt.legend()
plt.xlabel('Trabalho')
plt.ylabel('Tempo (s)')
plt.title('Tempo de usuário de todos os trabalhos - Summa Serial')
fig2.savefig('/home/gabriel/Dropbox/UEFS/2019.2/CAD/Problema 2/Resultados/tempo_de_usuario_summaSERIAL_classic', dpi=400)