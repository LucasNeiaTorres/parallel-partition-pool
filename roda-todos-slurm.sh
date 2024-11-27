#!/bin/bash
echo "USAGE: ./rodaTodos.sh <nElementsInput> <nElementsP>"
echo "$0 rodando no host " `hostname`  
echo "$0 rodando no host " `hostname` >saida.txt

echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE

NTIMES=10
echo "nt " $NTIMES
MAX_EXECS=20
echo "MAX_EXECS " $MAX_EXECS

for i in {1..8}
do
    echo "Executando $NTIMES vezes com $1 elementos de Input, $2 elementos de P e $i threads:"
    for j in $(seq 1 $NTIMES);
    do
        echo "-----------------------" >>saida.txt
        if [ $j -le $MAX_EXECS ];
        then 
          ./multi_partition $1 $2 $i | tee -a saida.txt | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
        else
          echo "nao executado" | tee -a saida.txt 
        fi  
    done
done
echo "O tempo total dessa shell foi de" $SECONDS "segundos"
echo "SLURM_JOB_NAME: "	$SLURM_JOB_NAME	
echo "SLURM_NODELIST: " $SLURM_NODELIST 
echo "SLURM_JOB_NODELIST: " $SLURM_JOB_NODELIST
echo "SLURM_JOB_CPUS_PER_NODE: " $SLURM_JOB_CPUS_PER_NODE
#imprime infos do job slurm (e.g. TEMPO até aqui no fim do job)
squeue -j $SLURM_JOBID