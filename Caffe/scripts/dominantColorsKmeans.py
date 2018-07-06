#https://stackoverflow.com/questions/3241929/python-find-dominant-most-common-color-in-an-image
import Image
import numpy as np
import scipy
import scipy.misc
import scipy.cluster


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
    peak, centers = calc(k, filePath)

    # return hsv value
    #results_hsv =[]
    #for rgb in results_rgb:
    #    results_hsv.append(colorsys.rgb_to_hsv(rgb[0],rgb[1],rgb[2]))
    #return results_hsv
    return peak