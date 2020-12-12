import sys
import os
import subprocess

def gen_simpoint(prog):
    simpoints_dir = "./simpoints/" + prog
    temp_dir = "temp/" + prog + "/"
    simpoint_file = temp_dir + prog + "_simpoints"
    weights_file = temp_dir + prog + "_weights"

    # Step 1: Run first simulation to get simpoint data
    subprocess.call(["./build/X86/gem5.opt", "-d", temp_dir, "configs/spec2k6/run.py", "-b", prog, "--simpoint-profile", "--simpoint-interval", "100000000", "--cpu-type=NonCachingSimpleCPU"])

    # Step 2: Generate simpoint vectors
    subprocess.call(["simpoint", "-loadFVFile", temp_dir + "simpoint.bb.gz", "-maxK", "30", "-saveSimpoints", simpoint_file, "-saveSimpointWeights", weights_file, "-inputVectorsGzipped"])

    # Step 3: Generate architectural state at each simpoint with warmup time
    subprocess.call(["./build/X86/gem5.opt", "-d", simpoints_dir, "configs/spec2k6/run.py", "-b", prog, "--take-simpoint-checkpoint="+simpoint_file+","+weights_file+",100000000,10000000", "--cpu-type=NonCachingSimpleCPU"])

def main(benchmarks):
    for prog in benchmarks:
        print("-------- Generating simpoint for {} --------".format(prog))
        gen_simpoint(prog)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: gen_simpoints.py <bench1> ... <benchN>")
        exit(1)

    main(sys.argv[1:])
