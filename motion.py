from SimpleCV import *

cam = Camera()
disp = Display()
boolean = 1
img1 = cam.getImage()
while disp.isNotDone():
	if boolean:
		img2 = cam.getImage()
		boolean = 0
	else:
		img1 = cam.getImage()
		boolean = 1
	

	diff = img1 - img2

	if disp.mouseLeft:
		break
	diff.save(disp)