#!/usr/bin/python3
import sys
import time
from pathlib import Path

nBD = 5

errcodes = ["On", "Ramp Up", "Ramp Down", "Over Current",
	    "Over Voltage", "UnderVoltage", "Ext. Trip",
	    "max V", "ext. Disable", "Int. Trip", "Cal. Err",
	    "Unplugged","Empty","Over Voltage Protection",
	    "Power Fail", "Temp. Err"]

def status(stat):
    code = ""
    for i in range(len(errcodes)):
        if stat & (1 << i) :
            code += errcodes[i] + ", "
    if code.startswith("On, "):
        code = code[4:]   
    if len(code) > 1:
        code = code[:-2]
    return code
    
def genTable(filename):
    boardID = []
    boardTemp = []
    filename_html = Path(filename).with_suffix('.BD.html')
    datafile = open(filename, "r")

    tableheader = "<table> <tr> <th> Slot ID </th> <th> Temp (C) </th></tr>\n"
    tablefotter = "\n </table>"
    website = open(filename_html, 'w')
    website.write(tableheader)
    
    for i in range(nBD):
        l = datafile.readline()
        #boardID.append(int(l.split()[1]))
        #boardTemp.append(float(l.split()[-1]))
        if len(l) == 0 or len(l.split()) <2:
            continue
        
        boardID = l.split()[1]
        boardTemp = l.split()[-1]
        website.write("<tr><td>"+boardID+ "</td><td>" +boardTemp+"</td></tr>\n")


    website.write(tablefotter)
    website.close()
    filename_html = Path(filename).with_suffix('.CH.html')
    tableheader = "<table> <tr> <th> Channel </th> <th> Vmon (V) </th><th> Imon (uA) </th> <th> Status </th>  <th>  </th> </tr>\n"
    
    website = open(filename_html, 'w')
    website.write(tableheader)
    for l in datafile.readlines():
        if len(l) == 0 or len(l.split()) < 7:
            continue
        name = l.split()[2][:-1]
        stat = l.split()[3]
        volt = l.split()[4]
        curr = l.split()[5]
        erro = int(l.split()[6])
        erro = status(erro)
        website.write("<tr><td>"+name+ "</td><td>" +volt+"</td><td>" +curr+"</td>")
        if stat == "ON":
             website.write("<td style=\"background-color: #00CC00;\">"+stat+ "</td>")
        if stat == "OFF":
             website.write("<td style=\"background-color: #CC0000;\">"+stat+ "</td>")
             
        if erro == errcodes[1] or erro == errcodes[2]:
             website.write("<td style=\"background-color: #00CCCC;\">"+erro+ "</td>")
        elif len(erro) >0:
            website.write("<td style=\"background-color: #FF6600;\">"+erro+ "</td>")
        website.write("</tr>\n")
        
        
        
    website.write(tablefotter)
    website.close()
    datafile.close()

def main(argv):
    if len(argv) < 3:
        print("usage ./genWeb.py FILENAME SHOWPLOT")
        return

    while True:
        genTable(argv[1]);
        time.sleep(5)
        

if __name__ =="__main__":
    sys.exit(main(sys.argv))
