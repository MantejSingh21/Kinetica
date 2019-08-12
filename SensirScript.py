#!/usr/bin/python

import sys, serial, argparse
import numpy as np
import glob
from collections import deque
from time import sleep
import struct
import itertools
import subprocess
import time
import datetime
import getopt
import os

from sklearn.multiclass import OneVsOneClassifier
from sklearn.multiclass import OneVsRestClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.preprocessing import StandardScaler  
from sklearn import preprocessing
from sklearn import svm

import matplotlib.animation as animation
import matplotlib.pyplot as plt 
from pylab import *

parentPath = os.getcwd()

currentGesture = 0
numGestures = 15
currentIteration = 0
	
class AnalogPlot:
	def __init__(self, strPort, maxLen):
		self.ser = serial.Serial(strPort, 115200)
		self.x, self.y = np.meshgrid(np.linspace(0, 13, 14), np.linspace(0, 13, 14))
		self.z = np.sin(self.x)*np.sin(self.x)+np.sin(self.y)*np.sin(self.y)
		self.paused = False
		self.packetCount = 0
		self.trained = False
		self.classify = False
		self.clf = OneVsRestClassifier(MLPClassifier(solver='lbfgs', alpha=0.05, hidden_layer_sizes=(24,), random_state=1))                
		#self.clf = svm.SVC()
		self.currentGesture = 0
		self.features = []
		self.labels = []
		self.scaler = StandardScaler()
		
	def trainInstance(self, gestureType):
		global currentIteration
		print currentIteration
		#print 'adding training instance', gestureType
		#self.features[int(gestureType)-1].append(self.z.ravel())
		self.features.append(np.array(self.z.ravel()))
		self.labels.append(int(gestureType))
		print self.labels

	def trainClassifier(self):
		print 'training...'
		trainingLabels = np.array(self.labels)	
		trainingSet = np.array(self.features)
		trainingSet = self.scaler.fit_transform(trainingSet)  

		#for i in range(0,4):
		#	for j in range(0,len(self.features[i])):
		#		trainingLabels.append(i)
		print 'trainingLabels:'
		print trainingLabels
		print 'trainingSet:'
		print trainingSet
		self.clf.fit(trainingSet, trainingLabels)
		self.trained = True
		print 'trained'

	# update plot
	def update(self, frameNum, mesh):
		
		while self.ser.inWaiting() > 50:
			try:
				first_bits = self.ser.read(2)
				while first_bits != '\xBE\xEF':
					first_bits = self.ser.read(2)
				led_val = self.ser.read()
				led_num = struct.unpack('B', led_val)[0]
				diode_data = self.ser.read(14*2)
				diode_values = struct.unpack('14H', diode_data)
				#self.z[led_num, :] = frameNum % 10
				self.z[led_num, :] = diode_values
				#for i in range(0,14):
				#	self.z[led_num,i] = diode_values[i]

				#print 'led ',led_num
				#if led_num == 11:
				#	print self.z
				mesh.set_array(self.z)
				
				if self.classify:
					if not self.trained:
						#train instance
						print 'not trained'
						self.classify = False
					else:
						print 'classifying...'
						testingInstance = np.array([self.z.ravel()])
						print testingInstance
						testingInstance = self.scaler.transform(testingInstance)
						result = self.clf.predict(testingInstance)
						print result
						self.classify = False
						

				#if not self.paused:

			except KeyboardInterrupt:
				print('exiting')
		
		#return a0, 

	def close(self):
		#self.ser.write('\xFF')
		#self.ser.flush()
		#self.ser.close()		
		print 'closing analogPlot'

def saveData():
	timestr = time.strftime("%Y%m%d-%H%M%S")	
	dirpath = os.path.join(parentPath, timestr)
	os.mkdir(dirpath)
	os.chdir(dirpath)
	trainingLabels = np.array(analogPlot.labels)	
	trainingSet = np.array(analogPlot.features)
	np.save('labels', trainingLabels)
	np.save('data', trainingSet)

	os.chdir(parentPath)
	#exit(0)

def press(event, mesh, analogPlot):
    sys.stdout.flush()
	#if evet.key == '-':
	#	max
    if event.key == '1' or event.key == '2' or event.key == '3' or event.key == '4' or event.key =='5' or event.key == '6' or event.key == '7' or event.key == '8' or event.key == '9':
	analogPlot.trainInstance(event.key)
    if event.key == 't':
	analogPlot.trainClassifier()
    if event.key == ' ':
	#analogPlot.classify = True

	global currentGesture, numGestures, currentIteration
	if currentIteration < 10:
		analogPlot.trainInstance(currentGesture)
		currentGesture += 1

	if currentGesture >= numGestures and currentIteration < 10:
		currentGesture = 0	
		currentIteration += 1
		if currentIteration == 10:
			saveData()
			analogPlot.trainClassifier()
			#analogPlot.paused = not analogPlot.paused

	if currentIteration == 10:
		analogPlot.classify = True
		
	
    if event.key == 'w':
	saveData()
	print 'training data saved'
    if event.key == 'q':
	analogPlot.ser.write('\xFF')
	analogPlot.ser.flush()
	analogPlot.ser.close()
	analogPlot.close()
	exit(0)

def changeMinMax(val):
	global s_min, s_max, mesh 
	print 'changing val:', val
	#mesh.vmin = s_min.val
	#mesh.vmax= s_max.val
	mesh.set_clim(vmin=s_min.val) 
	mesh.set_clim(vmax=s_max.val) 

def main():

	if sys.platform.startswith('win'):
		ports = ['COM' + str(i + 1) for i in range(256)]

	elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
		# this is to exclude your current terminal "/dev/tty"
		ports = glob.glob('/dev/ttyACM*')
		#ports = glob.glob('/dev/ttyUSB*')

	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.usb*')

	else:
		raise EnvironmentError('Unsupported platform')

	result = []
	for port in ports:
		try:
			s = serial.Serial(port)
			s.close()
			result.append(port)
		except (OSError, serial.SerialException):
			pass
	
	if len(result) > 1:
		print 'Too many ports to choose from'
		#exit(0)
	if len(result) < 1:
		print 'No ports found'
		exit(0)
	if len(result) is 1:
		print 'Found serial port: ', result

	strPort = result[0]

    #    print 'Connecting to :', strPort,'...'
	global analogPlot
	analogPlot = AnalogPlot(strPort, 500)

	# wait for port to open
	while analogPlot.ser.isOpen() == 0:
		print '...'
		sleep(1)
	sleep(2)

	print 'Connected!'

	analogPlot.ser.write("ready\n")  

	fig = plt.figure()
	ax = plt.axes()

	axmin = axes([0.15, 0.1, 0.65, 0.03], axisbg='lightgoldenrodyellow')
	axmax = axes([0.15, 0.15, 0.65, 0.03], axisbg='lightgoldenrodyellow')

	global s_min 
	s_min = Slider(axmin, 's_min', 0, 60000, valinit=0)
	global s_max
	s_max = Slider(axmax, 's_max', 0, 60000, valinit=60000)

	s_min.slidermax = s_max
	s_max.slidermin = s_min

	#mesh = ax.pcolormesh(analogPlot.x,analogPlot.y,analogPlot.z,shading='gouraud')
	#mesh = ax.pcolormesh(analogPlot.x,analogPlot.y,analogPlot.z,shading='flat',vmin = 0, vmax = 15)
	global mesh
	mesh = ax.imshow(analogPlot.z,cmap=plt.cm.coolwarm,
			extent=[analogPlot.x.min(), analogPlot.x.max(), analogPlot.y.min(), analogPlot.y.max()],
			#interpolation='nearest', origin='lower', vmin = 0, vmax = 66000)
			interpolation='nearest', origin='lower', vmin = s_min.val, vmax = s_max.val)
	#mesh.set_array([])

	fig.canvas.mpl_connect('key_press_event', lambda event: press(event, mesh, analogPlot))

	s_min.on_changed(changeMinMax)
	s_max.on_changed(changeMinMax)

	anim = animation.FuncAnimation(fig, analogPlot.update, 
								 fargs=(mesh,), 
								 #interval=50, blit=False,repeat=False)
								 interval=10, blit=False,repeat=False)
	analogPlot.ser.flushInput()
	#analogPlot.ser.write('\xFF')
	analogPlot.ser.write('\xFF')
	plt.show()
	analogPlot.close()

	print('exiting.')

if __name__ == '__main__':
	main()
