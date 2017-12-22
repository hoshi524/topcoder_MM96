import subprocess

subprocess.call(
    'g++-5 -std=gnu++1y -O2 -o out/main.out src/main.cpp', shell=True)
subprocess.call('javac -d out src/GarlandOfLightsVis.java', shell=True)

for seed in range(0, 10):
    subprocess.call(
        'java -cp out GarlandOfLightsVis -exec out/main.out -seed {0}'.format(seed), shell=True)
