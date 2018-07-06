import os
import sys
import time
import multiprocessing as mp

SHOT_DIR = os.path.abspath("shots")
SHOT_DETECTOR = "/home/theuers/CLionProjects/OpenCV/cmake-build-debug/ShotDetect"
THRESHOLD = 0.5

def detectShots(filepath):
    start_time = time.time()
    print("SHOTDETECT: " + os.path.basename(filepath))
    os.system(" ".join([SHOT_DETECTOR, str(THRESHOLD), filepath, SHOT_DIR]))
    print("END OF SHOTDETECT: " + os.path.basename(filepath) + " " + str(time.time() - start_time) + " seconds")


if len(sys.argv) != 2:
    print("specify video-folder!")
    exit(-1)

input_dir = sys.argv[1]

# perform setup
os.system("sh setup.sh")

fileList = []
for file in os.listdir(input_dir):
    if(file.endswith(".mp4")):
        fileList.append(os.path.join(input_dir,file))

pool = mp.Pool(processes=4)
results = pool.map(detectShots, fileList)






