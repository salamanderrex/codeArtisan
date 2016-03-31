#!/usr/bin/python
import gtk
import gtk.gdk
import xdot
import re
import sys


#read data
FunctionName = []
Inst = []
Direction = []
dotcode_list = []


IR_list = []
class IR:


    def __init__(self,FunctionName,Instname,direction,pointerStatus):
        self.functionName = FunctionName
        self.direction = direction
        self.Instname = Instname
        self.pointerStatus = pointerStatus
        
        
class IR_cluster:
    def __init__(self):
        self.cluster = []
    def memberOrNot(self,IR):
        if len(self.cluster) == 0:
            return True
        if IR.functionName== self.cluster[0].functionName and \
            IR.direction == self.cluster[0].direction and \
            IR.Instname == self.cluster[0].Instname :
            return True
        else:
            return False


def readFile(filename):
    file = open(filename,'r')
    dot_code = []
    functionName = ''
    direction = ''
    Instname = ''
    pointerStatus = ''
    IRcluster = IR_cluster()
    for line in file.readlines():
        line=line.strip()
        print line
        if not line or line =="\n" or line== " " or line =="=======":
            continue
        matchFunctionName = re.match(r'.*Function.*',line)
        if matchFunctionName:
            functionName =  line
            continue
        matchInstruction = re.match(r'.*tation.*',line)
        if matchInstruction:
            Instname = line
            continue
        #print "lllll", line
        matchDirection = re.match(r'.*ing.*',line)
        if matchDirection:
            direction = line
            continue

        pointerStatus =  line
        print pointerStatus
        #print line,"kk"
        ir = IR(functionName,Instname,direction,pointerStatus)
        if (IRcluster.memberOrNot(ir)):
            IRcluster.cluster.append(ir)
        else:
            IR_list.append(IRcluster)
            IRcluster = IR_cluster()
            IRcluster.cluster.append(ir)
    print IRcluster

            

#print IR_list
'''
for i in range(len(IR_list)):
    cluster = IR_list[i]
    for x in range(len(cluster.cluster)):
        print cluster.cluster[x].pointerStatus,
    print "\n"
'''
i = 0
#display
def getNextIR():
    global  i
    if len(IR_list) > i:
        i= i+1
        return  IR_list[i-1]
    else:
        return 0


class MyDotWindow(xdot.DotWindow):

    def __init__(self):
        xdot.DotWindow.__init__(self)
        self.widget.connect('clicked', self.clicked)

    def clicked(self, widget, url, event): 
        print "click"
        next = getNextIR()
        self.set_dotcode(getDotCode(next))
        self.set_title(next.cluster[0].Instname)
        return True


def getDotCode(IR_cluster):
    if (IR_cluster==0):
        return getDotCode(getNextIR())
    string ="" 
    for i in range(len(IR_cluster.cluster)):
        string = string + IR_cluster.cluster[i].pointerStatus + '\n'
        print string
    returnstr =  "digraph G { \n Next [URL=\"http://en.wikipedia.org/wiki/Hello\"] \n" + string + "}"
    print returnstr
    return returnstr
        



def main():
    filename= sys.argv[1]
    print filename
    readFile(filename)
    print "read DONE"
    window = MyDotWindow()
    window.set_dotcode( getDotCode(getNextIR()))
    window.set_title("CSE231")
    window.connect('destroy', gtk.main_quit)
    gtk.main()


if __name__ == '__main__':
    main()
