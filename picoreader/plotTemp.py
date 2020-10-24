#!/usr/bin/python3

import numpy as np
import sys
import time
import datetime
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from multiprocessing import Process

plt.ion()

nch = 4
namedet = ["MB4 RT #17", "MB2 RC #16", "MB0 RB #12", "MB5 LT #13", "CL0 RB #09", "CL1 LB #12", "CL2 RT #08", "CL3 LT #14", "MB1 LB #23", "MB3 LC #18", "dummy2", "dummy3"]
donotdraw = [10,11]

class PlotProc():

    def on_launch(self):
        # setting up plot
        self.figure, self.ax = plt.subplots()
        self.lines = []
        for i in range(nch):
            if i+self.mod*nch in donotdraw:
                l, = self.ax.plot([],[], linewidth=1)
            else:
                l, = self.ax.plot([],[], linewidth=1, label=namedet[i+self.mod*nch])
            self.lines.append(l)
        # autoscale on unknown axis
        self.ax.set_autoscaley_on(True)
        self.ax.grid()
        self.ax.set_xlabel("Time", fontsize = 16)
        self.ax.set_ylabel(r"Resistance ($\Omega$)", fontsize = 16)
        # date formatting
        formatter = mdates.DateFormatter("%d/%m %H:%M:%S")
        self.ax.xaxis.set_major_formatter(formatter)
        plt.gcf().autofmt_xdate()
        plt.legend(loc="best", ncol=2)
        
    def on_running(self, xdata, ydata):
        if not plt.fignum_exists(1):
            quit()
        # plot all points
        for i in range(nch):
            if i+self.mod*nch in donotdraw:
                continue
            self.lines[i].set_xdata(xdata)
            self.lines[i].set_ydata(ydata[i])

        
        
        # rescale plot
        onedayago = xdata[-1] + datetime.timedelta(hours = -24)
        if onedayago > xdata[0]:
            self.ax.set_xlim(xmin =onedayago, xmax= xdata[-1]+ datetime.timedelta(hours = 0.5))
        self.ax.relim()
        self.ax.autoscale_view()
            
        plt.tight_layout()

        if self.plot > 0:
            # update canvas, flush
            self.figure.canvas.draw()
            self.figure.canvas.flush_events()
        else:
            # save to file
            self.figure.savefig('current_temp_%d.png'%self.mod)
        
    # run the reader, main method
    def run(self, filename, mod, plot):
        self.mod = mod
        self.plot = plot
        self.on_launch()
        xdata = []
        ydata = [[] for i in range(nch)]
        
        print(filename)
        
        # read existing data
        datafile = open(filename, "r")
        datafile.readline()
        for x in datafile.readlines():
            dateword = x.split()[0]
            timeword = x.split()[1]
            date_time_obj = datetime.datetime.strptime(dateword+" "+timeword, '%Y-%m-%d %H:%M:%S')
            xdata.append(date_time_obj)
            for i in range(nch):
                ydata[i].append(float(x.split()[2+i]))

        # keep reading from end of file
        l = self.readnewfromfile(datafile)
        for x in l:
            dateword = x.split()[0]
            timeword = x.split()[1]
            date_time_obj = datetime.datetime.strptime(dateword+" "+timeword, '%Y-%m-%d %H:%M:%S')
            #print('Date-time:', date_time_obj)            
            xdata.append(date_time_obj)
            for i in range(nch):
                ydata[i].append(float(x.split()[2+i]))
            self.on_running(xdata, ydata)
            time.sleep(1)

    def readnewfromfile(self, datafile):
        # end of file
        datafile.seek(0,2) 
        while True:
            line = datafile.readline()
            if not line:
                # wait
                time.sleep(0.1) 
                continue
            yield line



def main(argv):
    if len(argv) < 3:
        print("usage ./plotTemp.py FILENAME MODULENR SHOWPLOT")
        return
    d = PlotProc()
    d.run(argv[1],int(argv[2]),1 if len(argv) > 3 else 0)
        

if __name__ =="__main__":
    sys.exit(main(sys.argv))

