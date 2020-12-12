import sys
import os
import csv


def find_all(name, path):
    result = []
    for root, dirs, files in os.walk(path):
        if name in files:
            result.append(os.path.join(root, name))

    return result


def dump_parse(res, out_name):
    with open(out_name, 'w') as fp:
        writer = csv.DictWriter(fp, fieldnames = ['name', 'cpi', 'ipc', 'num_cycles', 'incorrect', 'predicted', 'loop_correct', 'loop_wrong', 'branch_rate'], restval = "NOT FOUND!")
        writer.writeheader()
        writer.writerows(res)


def parse(name):
    lines = []
    res = {}
    base = 'system.switch_cpus_1.'

    (head, tail) = os.path.split(name)
    res['name'] = head

    with open(name, 'r') as fp:
        lines = fp.readlines()

    # Do something
    for line in lines:
        words = line.split()
        if len(words) < 2:
            continue
        elif words[0] == base + 'cpi':
            res['cpi'] = words[1]
        elif words[0] == base + 'branchPred.condIncorrect':
            res['incorrect'] = words[1]
        elif words[0] == base + 'branchPred.condPredicted':
            res['predicted'] = words[1]
        elif words[0] == base + 'branchPred.loop_predictor.loopPredictorCorrect':
            res['loop_correct'] = words[1]
        elif words[0] == base + 'branchPred.loop_predictor.loopPredictorWrong':
            res['loop_wrong'] = words[1]
        elif words[0] == base + 'fetch.branchRate':
            res['branch_rate'] = words[1]
        elif words[0] == base + 'numCycles':
            res['num_cycles'] = words[1]
        elif words[0] == base + 'ipc':
            res['ipc'] = words[1]
            

    return res


def parse_all(root):
    names = find_all("stats.txt", root)
    res = []

    for name in names:
        res.append(parse(name))

    dump_parse(res, 'collect.csv')


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Need a root folder name\n")
        exit(1)

    parse_all(sys.argv[1])
