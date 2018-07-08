import os
import sys
import time
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from subprocess import check_output

# concept identification
PROJECT_DIR = "/home/theuers/"
CONCEPT_DETECTOR = "python " + os.path.join(PROJECT_DIR,"vr18/models/tutorials/image/imagenet/classify_image.py --image_file ")


def displayImage(filepath, text):
    fig, ax = plt.subplots()
    img = mpimg.imread(filepath)
    imgplot = plt.imshow(img)
    # these are matplotlib.patch.Patch properties
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)

    ax.text(0.05, 0.95, text, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=props)
    plt.show()

def classifyImage(filepath):
    name = os.path.basename(filepath)
    start_time = time.time()
    print("INDEX: " + name)
    # getting concept
    cmd = CONCEPT_DETECTOR + filepath
    print(cmd)
    output = check_output(cmd.split()).decode("utf-8")
    print("END OF INDEX: " + name + " " + str(time.time() - start_time) + " seconds")
    print(output)
    displayImage(filepath, output)


if len(sys.argv) != 2:
    print("specify video-folder!")
    exit(-1)

input_dir = sys.argv[1]


# image classification
for file in sorted(os.listdir(input_dir)):
   classifyImage(os.path.join(input_dir, file))







