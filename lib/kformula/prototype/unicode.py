#!/usr/bin/env python

import sys
from qt import *
from xml.sax import saxutils, handler, make_parser

class Form1(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if name == None:
            self.setName('Form1')

        self.setCaption(self.tr('Form1'))
        grid = QGridLayout(self)
        grid.setSpacing(6)
        grid.setMargin(11)

        self.widgets = {}

        begin = 32
        end = 256
        for i in range(begin, end):

            charLabel = QLabel(self,'charLabel' + chr(i))
            charLabel.setFont(QFont("symbol", 16, QFont.Normal, 0, QFont.AnyCharSet))
            charLabel.setText(self.tr(chr(i)))
            grid.addWidget(charLabel, i-begin, 0)
            
            number = QLineEdit(self,'Number' + chr(i))
            grid.addWidget(number, i-begin, 1)

            latexName = QLineEdit(self,'latexName' + chr(i))
            grid.addWidget(latexName, i-begin, 2)

            charClass = QLineEdit(self,'charClass' + chr(i))
            grid.addWidget(charClass, i-begin, 3)
            
            self.widgets[i] = (number, latexName, charClass)


class Widget(QWidget):

    def __init__(self):
        QWidget.__init__(self)
        
        vbox = QVBoxLayout(self)
        vbox.setSpacing(6)
        vbox.setMargin(0)

        hbox = QHBoxLayout()
        hbox.setSpacing(6)
        hbox.setMargin(0)

        loadButton = QPushButton("load", self)
        saveButton = QPushButton("save", self)

        QObject.connect(loadButton, SIGNAL("pressed()"), self.load)
        QObject.connect(saveButton, SIGNAL("pressed()"), self.save)
        
        hbox.addWidget(loadButton)
        hbox.addWidget(saveButton)
                
        vbox.addLayout(hbox)
        
        sv = QScrollView(self)
        big_box = QVBox(sv.viewport())
        sv.addChild(big_box, 0, 0)
        self.child = Form1(big_box)

        vbox.addWidget(sv)

    def load(self):
        parser = make_parser()
        parser.setContentHandler(ContentGenerator(self.child.widgets))
        parser.parse("symbol.xml")
        
    def save(self):
        f = open("symbol.xml", "w")
        f.write('<?xml version="1.0" encoding="iso-8859-1"?>\n')
        f.write('<unicodetable>\n')
        widgets = self.child.widgets
        for key in widgets.keys():
            number, latexName, charClass = widgets[key]
            f.write('    <entry key="' + `key` +
                    '" number="' + str(number.text()) +
                    '" name="' + str(latexName.text()) +
                    '" class="' + str(charClass.text()) +
                    '"/>\n')
            
        f.write('</unicodetable>\n')
        f.close()


class ContentGenerator(handler.ContentHandler):  
    def __init__(self, widgets):
        handler.ContentHandler.__init__(self)
        self.widgets = widgets

    def startElement(self, name, attrs):
        if name == 'entry':
            for (name, value) in attrs.items():
                if name == "key": key = int(value)
                elif name == "number": number = value
                elif name == "name": latexName = value
                elif name == "class": charClass = value

            numberWidget, latexNameWidget, charClassWidget = self.widgets[key]
            numberWidget.setText(number)
            latexNameWidget.setText(latexName)
            charClassWidget.setText(charClass)
            
    
def main():
    a = QApplication(sys.argv)

    mw = Widget()
    mw.setCaption('Unicode mapping util')
    mw.show()

    a.connect(a, SIGNAL('lastWindowClosed()'), a, SLOT('quit()'))
    a.exec_loop()

if __name__ == '__main__':

    main()
