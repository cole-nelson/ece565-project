import os
import sys
import subprocess
import argparse

NPROCS = 4

def run_benchmark(progs):
    print("Running chunk: ")
    print(progs)
    gem5 = './build/X86/gem5.opt'
    spec_script = 'configs/spec2k6/run.py'
    commands = []

    for prog in progs:
        dest_dir = 'new_runs/ltage_0/' + prog
        commands.append([gem5, '-d', dest_dir, spec_script, '-b', prog, '--fast-forward=100000000', '--warmup-insts=10000000', '--standard-switch=10000000', '--maxinsts=100000000', '--cpu-type=DerivO3CPU', '--caches', '--l2cache']) 
   
    procs = [subprocess.Popen(cmd) for cmd in commands]
    for p in procs:
        p.wait()

if __name__ == "__main__":
    #if(len(sys.argv) <= 1):
    #    print("Not enough arguments.\nUsage: run_benchmarks.py <list of benchmarks>")

    raw_args = ['astar', 'bzip2', 'gromacs', 'hmmer', 'sjeng', 'milc', 'lbm', 'libquantum', 'sphinx3', 'mcf', 'leslie3d', 'namd']

    chunks = [raw_args[i*NPROCS:(i+1)*NPROCS] for i in range((len(raw_args) + NPROCS - 1) // NPROCS)]
    print(chunks)
    for chunk in chunks:
        run_benchmark(chunk)
