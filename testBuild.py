import os
from subprocess import run

path = os.path.dirname(os.path.realpath(__file__)) + "/.docker/"
files = [f for f in os.listdir(path)]
for i, f in enumerate(files):
    print(f"{i}: {f}")

for dockerfile in files:
    full_path = path + dockerfile
    tag = dockerfile.replace("Dockerfile", "albert-extra-kde-settings")
    run(["docker", "build", "-t", tag, "-f", full_path, "."]).check_returncode()