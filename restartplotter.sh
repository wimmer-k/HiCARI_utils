#!/bin/bash
cd
killall plotTemp.py
#screen -dmS tempPlot bash
screen -dr tempPlot -p 0 -X stuff '/home/hicari/picoreader/startplotter.sh
'
