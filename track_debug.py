#use  python track_debug.py -v ./a1.webm -f ./out_of_tracks_debug.txt -s 3 -t 2

import numpy as np
import cv2
import cv2.cv as cv
import argparse
import time

def readFile(path):
	f = open(path, 'r')
	line = f.readline()
	arr = line.split()
	l = int(arr[0])
	numbers = []
	while True:
	    line = f.readline()
	    if not line: break
	    words = line.split()	   
	    num = range(l*2+3)
	    i = 0
	    for x in words:
	        num[i] = int(float(x))
	        i+=1

	    num[0] -= l
	    numbers.append(num)	
	return l, numbers

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

l, numbers = readFile(args.file)

cap = cv2.VideoCapture(args.video)

cv2.namedWindow('frame', cv2.WINDOW_NORMAL)
cv2.resizeWindow('frame', int(cap.get(cv.CV_CAP_PROP_FRAME_WIDTH) * scale), int(cap.get(cv.CV_CAP_PROP_FRAME_HEIGHT) * scale))	

i = 0
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()    

    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    for p in numbers:
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