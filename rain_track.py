#!/usr/bin/python
# Rain cloud tracking program
# Calculates potential collision of rain with input location
# -personal info REMOVED-
##written in python 2.7
import os
import glob
import time
from math import sqrt, sin, cos, tan, pi
from numpy import polyfit
from SimpleCV import *

#DEBUG
DEBUG = 1
DRAW = 1

(X, Y) = (0, 1)
# BORDERS
TOP, BOTTOM_1, BOTTOM_2  = 16, 496, 480

#Maximum distance a cloud can travel in one frame
MAX_DIST = 60

# BAND DIMENSIONS
BAND_RATIO = 0.5

# Vector angle variance
SWEEP = (pi/8)

# Area tolerance
AREA_TOL = 1

#==============================================================================#

class Cloud:
    def __init__(self, index):
        self.index = index
        self.linked_blobs = [None] * 6
        self.xcoords = []
        self.ycoords = []
        self.frame_num = 0
        self.curve = []
        self.inveq = []
        self.bounds = []
        self.collides = False

def addxy(cloud, blob):
    cloud.xcoords.append(blob.centroid()[X])
    cloud.ycoords.append(blob.centroid()[Y])

def getpoint(cloud, index):
    return (cloud.xcoords[index], cloud.ycoords[index])

def vector_diff(v1, v2):
    ''' Computes the difference between two tuples
    '''
    return (v2[X] - v1[X], v2[Y] - v1[Y])

def vector_add(v1, v2):
    ''' Computes the addition of two vectors
    '''
    return (v1[X] + v2[X], v1[Y] + v2[Y])

def distance(v1, v2):
    ''' Computes the distance between two tuples
    '''
    diff = vector_diff(v1, v2)
    return sqrt((abs(diff[X]))**2 + (abs(diff[Y]))**2)

def vector_average(vectors):
    ''' Takes an array of vectors and returns the average as a tuple
    '''
    sumx = sumy = 0
    for i in range(len(vectors)-1):
        sumx += vectors[i][X]
        sumy += vectors[i][Y]
    return (sumx/(i+1), sumy/(i+1))

def unitify(v):
    ''' turns a vector into a unit vector
    '''
    length = distance((0,0), v)
    if v[X] == 0:
        return (0,v[Y]/length)
    elif v[Y] == 0:
        return (v[X]/legnth, 0)
    return (v[X]/length, v[Y]/length)

def dotprod(v1, v2):
    return (v1[X] * v2[X]) + (v1[Y] * v2[Y])

def vec_angle(v1, v2):
    v1mag = distance((0,0), v1)
    v2mag = distance((0,0), v2)
    if (v1mag == 0) or (v2mag == 0):
        print "0 vector in angle calculation\n"
        return None
    dot = dotprod(v1, v2)
    return acos(dot/(v1mag * v2mag))

def min_dist_point(point, array, direc, sweep=(2*pi)):
    ''' Returns the index of the closest point in the array to the
        first argument point
    '''
    min_val = 512
    count = retval = 0
    for p in array:
        dist = distance(point, p)
        angle = vec_angle(vector_diff(point, p), direc)
        if (dist < min_val) and (angle<sweep):
            min_val = dist
            retval = count
        count += 1
    if min_val > MAX_DIST:
        return None
    return retval

def match_blob(blob_array_1, blob_array_2, direc, sweep=(2*pi)):
    coord_array = [blob.centroid() for blob in blob_array_2]
    matched_array = []

    for blob in blob_array_1:
        index = min_dist_point(blob.centroid(), coord_array, direc, sweep)
        if not index:
            continue
        matched_array.append([blob, blob_array_2[index]])
    return matched_array

def intersect(eq1, eq2):
    x = (eq2[1] - eq1[1]) / (eq1[0] - eq2[0])
    y = (eq1[0] * x) + eq1[1]
    return (x, y)

def det(u, v):
    ''' Returns the determinant of a 2x2 matrix, 
        where u and v are row vectors
    '''
    return u[0]*v[1] - u[1]*v[0]

def triangle_area(p1, p2, p3):
    # Vectors forming parallelogram
    u = vector_diff(p1, p2)
    v = vector_diff(p1, p3)
    # Returns half the determinant of the 'cross product' of vectors in R2
    return abs(0.5*det(u, v))

def in_gram(corners, point):
    # Triangle areas
    T1 = triangle_area(corners[0], corners[1], point)
    T2 = triangle_area(corners[0], corners[2], point)
    T3 = triangle_area(corners[3], corners[1], point)
    T4 = triangle_area(corners[3], corners[2], point)
    SUM = T1 + T2 + T3 + T4
    # Paralellogram srea
    u = vector_diff(corners[0], corners[1])
    v = vector_diff(corners[0], corners[2])
    trap_area = abs(det(u,v))
    if DEBUG:
        print "Checking point {} in rect {}".format(point, corners)
        print"T1: {}\nT2: {}\nT3: {}\nT4: {}\nSUM area:  {}\nTrap area: {}".format(T1, T2, T3, T4, SUM, trap_area) 
        print "Difference = {}\n".format(fabs(trap_area - SUM))
    if abs(trap_area - SUM) < AREA_TOL:
        return True;
    return False;
    
# Draws rectangle - only works when DRAW == 1
def drawrec(blob):
    if DRAW:
        rect = blob.minRect()
        draw.line(rect[0], rect[1], (255,0,0))
        draw.line(rect[2], rect[3], (0,255,0))
        draw.line(rect[0], rect[2], (255,0,0))
        draw.line(rect[3], rect[1], (255,0,0))
        img2.addDrawingLayer(draw)
        img2.show()

#Program
#==============================================================================#

#Settings
user_location = (300,300)
my_images_path = "/home/mike/Documents/SimpleCV/Loop_1/" #put your image path here if you want to override current directory
extension = "*.png"

if not my_images_path:
    path = os.getcwd() #get the current directory
else:
    path = my_images_path

if DEBUG:
    print "searching in path %s" % path

imgs = list() #load up an image list
directory = os.path.join(path, extension)
files = sorted(glob.glob(directory))

blob1 = Image(files[0]).findBlobs()
blob2 = Image(files[1]).findBlobs()

if DRAW:
    draw = DrawingLayer((512, 512))
    Image(files[0]).crop(0,TOP,BOTTOM_1,BOTTOM_2).show()
else:
    Image(files[0]).crop(0,TOP,BOTTOM_1,BOTTOM_2)
time.sleep(1)

# Calculations

blobs = [[]] * 6

dir_avg_arr = [None] * 5
dir_vectors = [[]] * 5
match_arr = [[]] * 5

# Direction Calculation
if DEBUG:
    print "Linearly approximating cloud direction..."

for i in range(len(files)-1):
    img1 = Image(files[i]).crop(0,TOP,BOTTOM_1,BOTTOM_2)
    img2 = Image(files[i+1]).crop(0,TOP,BOTTOM_1,BOTTOM_2)
    
    blob1 = img1.findBlobs(1)
    blob2 = img2.findBlobs(1)

    if (i==0):
        blobs[i] = blob1
    blobs[i+1] = blob2 

    matched = match_blob(blob1, blob2, (0,1)) #direction arbitrary as it doesnt matter
    
    for j in matched:
        start = j[0].centroid()
        end = j[1].centroid()

        match_arr[i].append((start, end))
        
        # Directon Vectors
        dir_vectors[i].append(vector_diff(start, end))

    # Change in direction calc
    dir_avg_arr[i] = vector_average(dir_vectors[i])

# Net Direction
dir_avg = vector_average(dir_avg_arr)

direction = unitify(dir_avg)

# Direction line
if DRAW:
    draw.line((256,256), (256+(50*direction[X]),256+(50*direction[Y])), (255,170,0))

# Adjusted blob matching
if DEBUG:
    print "Matching blobs using SWEEP accuracy: {}...".format(SWEEP)
tracked_clouds = []
count = 0

for i in range(len(blobs)-1):
    img2 = Image(files[i+1]).crop(0,TOP,BOTTOM_1,BOTTOM_2)
    matched = match_blob(blobs[i], blobs[i+1], direction, SWEEP)
    
    # Storing matched blobs in clouds class
    for m in matched:
        found = 0 # Flag variable
        for n in tracked_clouds:
            # Searching through previously logged clouds
            if (not m[0]) or (not n.linked_blobs[i]):
                continue

            if (m[0].centroid() == n.linked_blobs[i].centroid()):
                found = 1
                n.linked_blobs[i+1] = m[1]
                addxy(n, m[1])
                n.frame_num += 1

        if not found:
            # Haven't found it so let's add it to the list
            temp = Cloud(count)
            temp.linked_blobs[i] = m[0]
            temp.linked_blobs[i+1] = m[1]
            addxy(temp, m[0])
            addxy(temp, m[1])
            temp.frame_num += 2 # All clouds 'should' have two points
            tracked_clouds.append(temp)
            count += 1
    
    # Old raw drawing
    '''for j in matched:
        start = j[0].centroid()
        end = j[1].centroid()
        
        draw.line(start, end, (255, 0, 0))
        if (i == 4):
            draw.circle(end, 10, (0, 255, 0))
    '''
    if DRAW:
        img2.addDrawingLayer(draw)
        img2.show()
    #time.sleep(1)

# Drawing in tracked cloud movements
if DRAW:
    for i in range(len(tracked_clouds)):
        for j in range(tracked_clouds[i].frame_num - 1):
            start = getpoint(tracked_clouds[i], j)
            end = getpoint(tracked_clouds[i], j + 1)

            draw.line(start, end, (0, 255, 0))
            draw.circle(end, 5, (255, 0,0))


    img2.addDrawingLayer(draw)
    img2.show()

# Calculating least square polyfit (degree 1) and inverse equation
if DEBUG:
    print "Calculating least square first degree polynomial..."
if DRAW:
    draw.circle(user_location, 10, (170, 0 ,255))
for i in tracked_clouds:
    x = i.xcoords
    y = i.ycoords
    i.curve = polyfit(x, y, 1)
    invgrad = (-1/i.curve[0])
    i.inveq = [invgrad, (-1 * invgrad * user_location[X]) + user_location[Y]]
    if i.linked_blobs[-1]:
        i.bounds = i.linked_blobs[-1].minRect()
if DEBUG:
    print "Calculating collisions with user_location: {}...".format(user_location)
# Calculating collision from point
for i in tracked_clouds:
    if not i.linked_blobs[-1]:
        # Ensure cloud still exists
        continue
    # Curve drawing for debuging for loop3 only
    #if DRAW and DEBUG and (i.index == 16):
    #    draw.line((0, i.inveq[1]), (500, i.inveq[0]*500 + i.inveq[1]), (255,255,255))
    #    draw.line((0, i.curve[1]), (500, i.curve[0]*500 + i.curve[1]), (255,255,255))

    if DEBUG:
        print "\nCloud: {}".format(i.index)
    cap = intersect(i.curve, i.inveq) # Closest approach point
    diff = vector_diff(cap, user_location) # Vector from cap to user
    
    lkp = (i.xcoords[-1], i.ycoords[-1]) # Last known position 
    cloud_dir = vector_diff(lkp, cap) # Vector from lkp to cap
    cldist = distance(lkp, cap) # Distance from lkp to cap
    
    rect = i.linked_blobs[-1].minRect() # Minimum boudning rectangle
    Tpoint = vector_add(lkp, diff) # Translated point

    if DRAW:
        drawrec(i.linked_blobs[-1])
        draw.circle(rect[0], 5, (255,255,255)) # Circle at point 'A'

    # NOTE: like in many of the calculations, this is greater than as y is inverted
    if (cldist < 500) and (lkp[Y] > (lkp[X]*i.inveq[0])) : # Arbitrary number so far
        i.collides = in_gram(rect, Tpoint)
    elif DEBUG:
        print "Out of range"
    if DEBUG:
        print "Collision: {}".format(i.collides)

p_in = (180, 345)
p_out = (300,300)
p_on_point = (180.6, 395)
p_out2 = (181, 396)
p_out3 = (239, 341.73584443044354)
#==============================================================================#
