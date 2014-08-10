########QUADCOPTER IMAGE TRACKING#########

#Developed by Rahul Tiwari and Gary Chien#

##########################################
##
##The MIT License (MIT)
##
##Copyright (c) 2014 Rahul Tiwari and Gary Chien
##
##Permission is hereby granted, free of charge, to any person obtaining a copy
##of this software and associated documentation files (the "Software"), to deal
##in the Software without restriction, including without limitation the rights
##to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
##copies of the Software, and to permit persons to whom the Software is
##furnished to do so, subject to the following conditions:
##
##The above copyright notice and this permission notice shall be included in
##all copies or substantial portions of the Software.
##
##THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
##IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
##FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
##AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
##LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
##OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
##THE SOFTWARE.
##########################################

#ser = serial.Serial(port='COM5',baudrate=9600)
import threading
from Tkinter import *
from time import sleep

directionfile = open('TargetStatus.txt', 'r')
directionfile = directionfile.readline()
directionfile = directionfile.strip()
print directionfile
clamp = lambda n, minn, maxn: max(min(maxn, n), minn)#Used to limit values from 0 to 4096

root = Tk()
frame = Frame(root, width=1, height=1)

thread_list = []

throttle = 0
throttle= clamp(throttle, 0, 4096)
pitch=2048
pitch= clamp(pitch, 0, 4096)
roll=2048
pitch= clamp(pitch, 0, 4096)
flipper=0

def main(threadnum):  
    global throttle
    global pitch
    global roll
    global flipper

    ###Override Commands###
    
    def forward(event):
        print "up"
        global pitch
        if pitch < 4046:
            pitch += 50

    def left(event):
        print "left"
        global roll
        if roll > 50:
            roll -= 50

    def right(event):
        print "right"
        global roll
        if roll < 4046:
            roll += 50

    def backward(event):
        print "down"
        global pitch
        if pitch > 50:
            pitch -= 50
        
    def descend(event):
        print "descend"
        global throttle
        if throttle > 10:
            throttle -= 50
        
    def ascend(event):
        print "ascend"
        global throttle
        if throttle <4046:
            throttle += 50
    def killall(event): #kill switch
        print "SHUTDOWN"
        global throttle
        throttle = 0
    
    if threadnum == 1: #responds to image tracking from file
        global directionfile
        directions = directionfile
        print directions
        directions = directions.split(' ', 1)
        
        print directions
        while True:
            directions = open('TargetStatus.txt', 'r')
            directions = directions.readline()
            directions = directions.strip()
            directions = directions.split(' ', 1)
            print directions
            for direction in directions:
                if (direction == "UPPER") and (throttle < 4096):
                    throttle += 50
                if direction == "LOWER" and throttle > 0:
                    throttle -= 50
                if direction == "LEFT" and roll > 0:
                    roll -= 50
                if (direction == "RIGHT") and (roll < 4096):
                    roll += 50
                if direction == "FORWARD" and pitch < 4096:
                    pitch += 50
                if direction == "BACKWARD" and pitch > 0:
                    pitch += 50
            print('Throttle ' + str(float(throttle)/40.96) + ' Pitch: ' + str(float(pitch)/40.96) + ' Roll: '+ str(float(roll)/40.96))
            sleep(1)
            
            
            
        #ser.write(throttle + pitch + roll + flip + '/n')
        #print(throttle + pitch + roll + flip)
            
    elif threadnum == 2: #manual override
        
        frame.focus_set()
        
        frame.bind("<Up>", forward)
        frame.bind("<Left>", left)
        frame.bind("<Right>", right)
        frame.bind("<Down>", backward)
        frame.bind("q", ascend)
        frame.bind("a", descend)
        frame.bind("<space>", killall)
        frame.pack()
        root.mainloop()

   


for i in range(1,4):
   # Initatiates the thread
   # (i) does not make a sequence, so (i,)
   t = threading.Thread(target=main, args=(i,))
   # Sticks the thread in a list so that it remains accessible 
   thread_list.append(t)

for thread in thread_list:
   thread.start()





