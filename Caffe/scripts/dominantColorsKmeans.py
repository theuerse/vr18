#https://stackoverflow.com/questions/3241929/python-find-dominant-most-common-color-in-an-image
import Image
import numpy as np
import scipy
import scipy.misc
import scipy.cluster
import colorsys


def calc(k, filepath):
    #print('reading image')
    im = Image.open(filepath)
    #im = im.resize((150, 150))      # optional, to reduce time
    ar = np.asarray(im)
    shape = ar.shape
    ar = ar.reshape(scipy.product(shape[:2]), shape[2]).astype(float)

    #print('finding clusters')
    codes, dist = scipy.cluster.vq.kmeans(ar, k)
    #print('cluster centres:\n', codes)

    vecs, dist = scipy.cluster.vq.vq(ar, codes)         # assign codes
    counts, bins = scipy.histogram(vecs, len(codes))    # count occurrences

    index_max = scipy.argmax(counts)                    # find most frequent
    peak = codes[index_max]
    #colour = ''.join(chr(int(c)) for c in peak).encode('hex')
    #print('most frequent is %s (#%s)' % (peak, colour))
    return peak, codes


def getDominantColors(k,filePath):
    peak_rgb, centers = calc(k, filePath)

    # return hsv value
    h, s, v = colorsys.rgb_to_hsv(peak_rgb[0] / 255., peak_rgb[1] / 255., peak_rgb[2] / 255.)
    peak_hsv = [360 * h, 100 * s, 100 * v]
    #print("rgb:" + str(peak_rgb) + " hsv: " +str(peak_hsv))
    return peak_hsv