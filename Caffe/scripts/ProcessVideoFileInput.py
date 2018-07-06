import os
import sys
import time
import sqlite3
import multiprocessing as mp
from subprocess import check_output
from dominantColorsKmeans import getDominantColors

# shot detection
SHOT_DIR = os.path.abspath("shots")
SHOT_DETECTOR = "/home/theuers/CLionProjects/OpenCV/cmake-build-debug/ShotDetect"
THRESHOLD = 0.7

# concept identification
CAFFE_DIR = "/home/theuers/caffe/"
CONCEPT_DETECTOR = os.path.join(CAFFE_DIR, "./examples/cpp_classification/classification.bin")
MODEL = "bvlc_reference_caffenet"
DEPLOY_PROTO = os.path.join(CAFFE_DIR, "models/"+ MODEL +"/deploy.prototxt")
CAFFE_MODEL = os.path.join(CAFFE_DIR, "models/" + MODEL + "/bvlc_reference_caffenet.caffemodel")
BINARY_PROTO = os.path.join(CAFFE_DIR, "data/ilsvrc12/imagenet_mean.binaryproto")
SYNSET = os.path.join(CAFFE_DIR, "data/ilsvrc12/synset_words.txt")


def detectShots(filepath):
    start_time = time.time()
    print("SHOTDETECT: " + os.path.basename(filepath))
    os.system(" ".join([SHOT_DETECTOR, str(THRESHOLD), filepath, SHOT_DIR]))
    print("END OF SHOTDETECT: " + os.path.basename(filepath) + " " + str(time.time() - start_time) + " seconds")


def classifyImage(filepath):
    name = os.path.basename(filepath)
    start_time = time.time()
    print("INDEX: " + name)

    # getting concept
    output = check_output([CONCEPT_DETECTOR, DEPLOY_PROTO, CAFFE_MODEL, BINARY_PROTO, SYNSET, filepath]).decode("utf-8")
    concept = output.split("\n")[1].split('"')[1].split()[0]

    # calculating dominant color
    dominantColor = getDominantColors(3,filepath)
    print(name + ": " + str(dominantColor))

    conn = sqlite3.connect('./vr.db')
    c = conn.cursor()
    c.execute('insert or replace into images values (?,?,?,?,?)', [os.path.basename(filepath), concept] + dominantColor)
    conn.commit()
    conn.close()

    print("END OF INDEX: " + name + " " + str(time.time() - start_time) + " seconds")





if len(sys.argv) != 2:
    print("specify video-folder!")
    exit(-1)

input_dir = sys.argv[1]

# perform setup
os.system("sh setup.sh")


# shot detection
fileList = []
for file in sorted(os.listdir(input_dir)):
    if(file.endswith(".mp4")):
        fileList.append(os.path.join(input_dir,file))

pool = mp.Pool(processes=4)
results = pool.map(detectShots, fileList)



# image indexing
fileList = []
for file in sorted(os.listdir(SHOT_DIR)):
    fileList.append(os.path.join(SHOT_DIR, file))
results = pool.map(classifyImage, fileList)







