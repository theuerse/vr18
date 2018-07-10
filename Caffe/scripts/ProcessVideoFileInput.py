import os
import sys
import time
import sqlite3
import multiprocessing as mp
from subprocess import check_output
from dominantColorsKmeans import getDominantColors

DEBUG = False

# shot detection
SHOT_DIR = os.path.abspath("shots")
SHOT_DETECTOR = "../../ShotDetection/ShotDetect2++"
THRESHOLD_H = 0.5
THRESHOLD_D = 0.05

# concept identification
CAFFE_DIR = "/home/theuers/caffe/"
CONCEPT_DETECTOR = os.path.join(CAFFE_DIR, "./examples/cpp_classification/classification.bin")
MODELS = ["bvlc_reference_caffenet","bvlc_alexnet","bvlc_googlenet"]
BINARY_PROTO = os.path.join(CAFFE_DIR, "data/ilsvrc12/imagenet_mean.binaryproto")
SYNSET = os.path.join(CAFFE_DIR, "data/ilsvrc12/synset_words.txt")


def detectShots(filepath):
    start_time = time.time()
    if DEBUG: print("SHOTDETECT: " + os.path.basename(filepath))
    os.system(" ".join([SHOT_DETECTOR, str(THRESHOLD_H), str(THRESHOLD_D), filepath, SHOT_DIR]))
    if DEBUG: print("END OF SHOTDETECT: " + os.path.basename(filepath) + " " + str(time.time() - start_time) + " seconds")


def classifyImage(filepath):
    name = os.path.basename(filepath)
    start_time = time.time()
    if DEBUG: print("INDEX: " + name)

    # getting concept
    results = {}
    for model in MODELS:
        DEPLOY_PROTO = os.path.join(CAFFE_DIR, "models/" + model + "/deploy.prototxt")
        CAFFE_MODEL = os.path.join(CAFFE_DIR, "models/" + model + "/" + model +".caffemodel")

        output = check_output([CONCEPT_DETECTOR, DEPLOY_PROTO, CAFFE_MODEL, BINARY_PROTO, SYNSET, filepath]).decode("utf-8")
        firstLine = output.split("\n")[1]
        concept = firstLine.split('"')[1].split()[0]
        concept_confidence = firstLine.split()[0]
        results[concept_confidence]=concept

    concept_confidence = max(results.keys())
    concept = results[concept_confidence]

    # calculating dominant color
    dominantColor = getDominantColors(3,filepath)
    #if DEBUG: print(name + ": " + str(dominantColor))
    #if DEBUG: print(name + ": " + concept_confidence)

    conn = sqlite3.connect('./vr.db')
    c = conn.cursor()
    c.execute('insert or replace into images values (?,?,?,?,?,?)', [os.path.basename(filepath), concept, concept_confidence] + dominantColor)
    conn.commit()
    conn.close()

    if DEBUG: print("END OF INDEX: " + name + " " + str(time.time() - start_time) + " seconds")





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

pool = mp.Pool(mp.cpu_count())
results = pool.map(detectShots, fileList)



# image indexing
fileList = []
for file in sorted(os.listdir(SHOT_DIR)):
    fileList.append(os.path.join(SHOT_DIR, file))
results = pool.map(classifyImage, fileList)


print("done!")




