#!/usr/bin/env python
import os, sys
import numpy
import bob.io.base
import bob.io.video
import argparse

parser = argparse.ArgumentParser(description='Convert HDF5 files to videos for visualization purposes.')
parser.add_argument('path', metavar='path', type=str, help='path to the HDF5 file to be converted')
parser.add_argument('-e', '--eyes', action='store_true', help='mark eye positions in the RGB video')
args = parser.parse_args(sys.argv[1:])

try:
    f = bob.io.base.HDF5File(args.path)
    Depth = f.read('Depth_Data')
    Color = f.read('Color_Data')
    if args.eyes:
        pos = f.read('Eye_Pos')
        for i in range(0,Color.shape[0]):
            bob.ip.draw_cross(Color[i,:,:,:], int(pos[i,0]), int(pos[i,1]), 10, (255,0,0))
            bob.ip.draw_cross(Color[i,:,:,:], int(pos[i,2]), int(pos[i,3]), 10, (255,0,0))
    head, tail = os.path.split(args.path)
    depth_file = head+'/'+tail.split('.')[0]+'_D.avi'
    color_file = head+'/'+tail.split('.')[0]+'_C.avi'
    depth_video = bob.io.video.writer(depth_file, Color.shape[-2], Color.shape[-1], 30)
    color_video = bob.io.video.writer(color_file, Color.shape[-2], Color.shape[-1], 30)
    D = numpy.right_shift(Depth,3).astype(numpy.uint8)
    depth_video.append(numpy.concatenate((D,D,D),1))
    depth_video.close()
    if Color.shape[1]<3:
        color_video.append(numpy.concatenate((Color,Color,Color),1))
    else:
        color_video.append(Color)
    color_video.close()
    print "Depth video is saved in %s." %depth_file
    print "Color video is saved in %s." %color_file
    del f
except IOError:
    print "The given file cannot be read."
