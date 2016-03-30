#use python otsu_thresholding.py -v ./sing.webm -f ./out_of_tracks.txt -s 3 -t10

import numpy as np
import cv2
import cv2.cv as cv
import matplotlib.pyplot as plt
import argparse
import time

def readFile(path):
	f = open(path, 'r')
	data = f.readlines()

	l = 0;
	trajectories = []
	otsu_nums = []

	for line in data:
		words = line.split()
		
		var_x = int(round(float(words[5])))
		var_y = int(round(float(words[6])))

		otsu_nums.append(var_x * var_y)

		l = int(words[2])

		num = range((l+1)*2+1)
		num[0] = int(words[0]) - l
		shift = 2 * l + 7

		i = 0
		for i in range (0, (l+1)*2):
			num[i+1] = int(float(words[i + shift]))
			i+=1

		trajectories.append(num)	

	return l, trajectories, otsu_nums



def draw_trajectories(file, trajectories, _time, scale, l):
	cap = cv2.VideoCapture(file)

	cv2.namedWindow('frame', cv2.WINDOW_NORMAL)
	cv2.resizeWindow('frame', int(cap.get(cv.CV_CAP_PROP_FRAME_WIDTH) * scale), int(cap.get(cv.CV_CAP_PROP_FRAME_HEIGHT) * scale))	

	i = 0
	while(True):
	    # Capture frame-by-frame
	    ret, frame = cap.read()    

	    # Our operations on the frame come here
	    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

	    for p in trajectories:
			if (p[0] >= i - l) and (p[0] <= i + l):
				for j in range (2, i - p[0]):
					cv2.line(frame, (p[2*(j-1)-1], p[2*(j-1)]), (p[2*j-1],p[2*j]), (0,255,0), 1)
					#cv2.circle(frame, (p[2*j-1],p[2*j]), 1, (0,255,0))

	    # Display the resulting frame
	    cv2.imshow('frame',frame)
	    time.sleep(0.033 * _time)
	    i += 1
	    if cv2.waitKey(1) & 0xFF == ord('q'):
	        break


	# When everything done, release the capture
	cap.release()
	cv2.destroyAllWindows()


def mhist(array):
	size = np.amax(array) + 1
	harr = np.zeros((size,), dtype=np.int)

	for i in array:
		harr[i] += 1

	return harr


def otsu_thresholding(histogram, total):
	_sum = 0

	for i in range(0, len(histogram)):
		_sum += i * histogram[i]

	sumB = 0
	wB = 0
	wF = 0
	mB = 0.0
	mF = 0.0
	_max = 0
	between = 0.0
	threshold = 0

	for i in range(0, len(histogram)):
		wB += histogram[i]
		if wB == 0:
			continue

		wF = total - wB
		if wF == 0:
			break

		sumB += i * histogram[i]
		mB = sumB / wB
		mF = (_sum - sumB) / wF

		between = wB * wF * pow(mB - mF, 2)
		if between > _max:
			_max = between
			threshold = i

	return threshold


# construct the argument parse and parse the arguments
parser = argparse.ArgumentParser()
parser.add_argument("-f", "--file", required=True, help="path to trajectories file [required]")
parser.add_argument("-v", "--video", required=True, help="path to video file [required]")
parser.add_argument("-s", "--scale", help="scale size", type = int)
parser.add_argument("-t", "--time", help="time slow", type = int)
args = parser.parse_args()

scale = 1;
if args.scale:
	scale = args.scale

_time = 1;
if args.time:
	_time = args.time

l, trajectories, otsu_nums = readFile(args.file)
hist_arr = mhist(otsu_nums)
#print hist_arr

thres = otsu_thresholding(hist_arr, len(otsu_nums))
print "Otsu thresholding: %d" % thres

plt.hist(otsu_nums, bins=50)
plt.title("Trajectory variance")
plt.xlabel("Value")
plt.ylabel("Frequency")
plt.show()

thresholded_trajectories = []

for i in range(0, len(otsu_nums)):
	if otsu_nums[i] >= thres:
		thresholded_trajectories.append(trajectories[i])

"""
f = open('workfile.txt', 'w')
for n in thresholded_trajectories:
	f.write(str(n) + '\n')
f.close()
"""

draw_trajectories(args.video, thresholded_trajectories, _time, scale, l)