#!/usr/bin/env python

"""This script converts a Magicpoint presentation and outputs
a KPresenter document (XML).

Magicpoint's homepage: http://www.mew.org/mgp/
KPresenter's homepage: http://www.koffice.org/kpresenter

This is free software, released under GPL v2.
Author: Lukas Tinkl <lukas@kde.org>, 2002

"""

import os, sys
import fileinput
import string
from xml.dom.DOMImplementation import implementation
from xml.dom import Document
from xml.dom.ext import PrettyPrint

def getYDpi():
  """Utility function; parses xdpyinfo output and returns the vertical DPI of the X display.
  Needed to transform the %-like font size into real font point size.
  """
  pipe=os.popen("(xdpyinfo | grep dot | cut -b21-22) 2>/dev/null", "r")
  if pipe:
    dpi=pipe.read().strip()
    if dpi:
      return dpi
  return 75

Y_OFFSET=510

class MgpImporter:
  def __init__(self):
    "Constructor"
    self.charset="iso8859-1"
    self.alignment="1"       #text alignment, left
    self.textColor="black"
    self.pageBuffer=""
    self.pageCount=-1        #page counter

    #font properties
    self.fontName="standard"
    self.fontItalic=0
    self.fontBold=0
    self.fontSize=24         #default (approximated) font size (5%)
    self.defFonts={}         #default (symbolic) font names
    
    self.vgap=1              #line spacing
    self.useDefaults=1       #use page default values?
    self.ydpi=int(getYDpi()) #Y DPI

    # background properties
    self.backtype="0"
    self.backview="0"
    self.bctype="0"          #single color (0) or gradient (non 0)
    self.color1="white"
    self.color2="white"

  def __setFontSize(self, command):
    tokens=string.split(command,' ')
    self.fontSize=int(Y_OFFSET/self.ydpi*72*float(tokens[1].strip())/100)
    #print self.fontSize

  def __setupDefaultFonts(self,command):
    tokens=string.split(command,' ')
    _key=string.replace(tokens[1], '"', '')
    _val=string.replace(tokens[3], '"', '')
    self.defFonts[_key]=_val
    #print self.defFonts

  def __setFontIndirect(self,command):
    tokens=string.split(command,' ')
    _font=string.replace(tokens[1], '"', '')
    if _font in self.defFonts.keys():        # we have a "default" font, find it in the map
      self.__setFont(None,self.defFonts[_font])
      #print self.defFonts[_font]

  def __setFont(self,command,font=""):
    if command:
      tokens=string.split(command,' ')
      font=string.replace(tokens[1], '"', '').strip() #XLFD-like, eg: mincho-medium-r (family-weight-slant)
    
    _numDash=string.find(font,"-")           #find dashes

    if (_numDash==-1):                       #mincho
      self.fontName=font
      return
    else:                                    #mincho-medium-r
      _xlfd=string.split(font, "-")
      self.fontName=_xlfd[0]
      if (_xlfd[1]=="bold" or _xlfd[1]=="semibold" or _xlfd[1]=="demibold" or _xlfd[1]=="demi bold"): #this sucks :)
        self.fontBold=1
      else:
        self.fontBold=0

      if (_xlfd[2]=="i"):
        self.fontItalic=1
      else:
        self.fontItalic=0
        
    #print self.fontName

  def __setBgColor(self, command):
    tokens=string.split(command,' ')
    self.bctype="0"
    self.color1=string.replace(tokens[1].strip(),'"', '') #strip quotes and \n    

  def __setBgGradient(self, command):
    tokens=string.split(command,' ')
    #xsize=tokens[1]
    #ysize=tokens[2]
    #numcolors=tokens[3] or "256"
    dir=tokens[4] or "0"
    #zoomflag=tokens[5] or "0"
    if (len(tokens[6:])>2):
      return
      #print "Unsupported number of gradients colors."
    else:
      color1=string.replace(tokens[6].strip(),'"', '') #strip quotes and \n
      color2=string.replace(tokens[7].strip(),'"', '')

    if (dir=="0"): #vertical
      value="1"
    elif (dir=="90"): #horizontal
      value="2"
    elif (dir=="180"): #vertical, swapped colors
      value="1"
      color1,color2=color2,color1
    elif (dir=="270"): #horizontal, swapped colors
      value="2"
      color1,color2=color2,color1
    elif (dir=="45"): #diagonal 1 
      value="3"
    elif (dir=="135"): #diagonal 2
      value="4" #TODO swap colors for diagonals too?
    else:
      value="1"

    self.bctype=value
    self.color1=color1
    self.color2=color2
    
  def __setAlign(self,command):
    tokens=string.split(command,' ')
    if (tokens[0]=='leftfill'): #justify
      self.alignment="8"
    elif (tokens[0]=='right'):
      self.alignment="2"
    elif (tokens[0]=='center'):
      self.alignment="4"
    else:
      self.alignment="1" #left
    #print self.alignment

  def __setBackground(self,parent):
    pageElem=self.document.createElement("PAGE")

    elem=self.document.createElement("BACKTYPE")   #color
    elem.setAttribute("value", self.backtype)
    pageElem.appendChild(elem)

    elem=self.document.createElement("BACKVIEW")   #always zoomed
    elem.setAttribute("value", self.backview)
    pageElem.appendChild(elem)

    elem=self.document.createElement("BCTYPE")     #single color
    elem.setAttribute("value", self.bctype)
    pageElem.appendChild(elem)

    elem=self.document.createElement("BACKCOLOR1") #1st color
    elem.setAttribute("color", self.color1)
    pageElem.appendChild(elem)

    if (self.bctype!="0"):
      elem=self.document.createElement("BACKCOLOR2") #2nd color
      elem.setAttribute("color", self.color2)
      pageElem.appendChild(elem)

    parent.appendChild(pageElem)

  def __handlePage(self,parent,bgParent):
    if (self.pageBuffer):
      self.__setBackground(bgParent) #set the background for this page
      
      self.pageCount=self.pageCount+1
      objElem=self.document.createElement("OBJECT") #KPresenter text object
      objElem.setAttribute("type", "4")

      elem=self.document.createElement("ORIG") #object position
      elem.setAttribute("x", "30")
      elem.setAttribute("y", str(self.pageCount*Y_OFFSET+30))
      objElem.appendChild(elem)

      elem=self.document.createElement("SIZE") #object size
      elem.setAttribute("width", "610")
      elem.setAttribute("height", "440")
      objElem.appendChild(elem)

      textElem=self.document.createElement("TEXTOBJ") #text object

      pElem=self.document.createElement("P") #paragraph
      pElem.setAttribute("align", self.alignment) 

      elem=self.document.createElement("NAME") #style name
      elem.setAttribute("value", "Standard")   ###is this needed at all?
      pElem.appendChild(elem)

      elem=self.document.createElement("TEXT") #paragraph text
      elem.setAttribute("VERTALIGN", "0")
      elem.setAttribute("family", self.fontName)
      elem.setAttribute("pointSize", str(self.fontSize))
      elem.setAttribute("color", self.textColor)
      text=self.document.createTextNode(self.pageBuffer)
      elem.appendChild(text)
      pElem.appendChild(elem)

      textElem.appendChild(pElem)
      objElem.appendChild(textElem)
      parent.appendChild(objElem)

    self.useDefaults=1
    self.pageBuffer=""

  def __handleText(self,line):
    self.pageBuffer+=unicode(line, self.charset, 'ignore')
    #print "*** text: " + line

  def __setCharset(self,command):
    tokens=string.split(command,' ')
    self.charset=tokens[1].strip()

  def __setTextColor(self,command):
    tokens=string.split(command,' ')
    self.textColor=string.replace(tokens[1].strip(),'"', '') #strip quotes
    #print self.textColor

  def __setPaper(self,parent):
    paperElem=self.document.createElement("PAPER")
    paperElem.setAttribute("ptWidth", "680")
    paperElem.setAttribute("ptHeight", str(Y_OFFSET))
    paperElem.setAttribute("orientation", "0") #landscape
    paperElem.setAttribute("format", "5")      #screen
    paperElem.setAttribute("unit", "0")        #mm

    borderElem=self.document.createElement("PAPERBORDERS")
    borderElem.setAttribute("ptLeft","0")
    borderElem.setAttribute("ptRight","0")
    borderElem.setAttribute("ptTop","0")
    borderElem.setAttribute("ptBottom","0")

    paperElem.appendChild(borderElem)
    
    parent.appendChild(paperElem)

  def convert(self, fileIn, fileOut):
    """Parses the Magicpoint document and returns a KPresenter XML document.

    fileIn: path to the input file
    fileOut: path to the output file, or sys.stdout if omitted
    """
    doctype=implementation.createDocumentType("DOC", "-//KDE//DTD kpresenter 1.2//EN",
                                              "http://www.koffice.org/DTD/kpresenter-1.2.dtd")
    self.document=implementation.createDocument("http://www.koffice.org/DTD/kpresenter", "DOC", doctype)
    
    rootElem=self.document.documentElement                #the root "DOC" element
    rootElem.setAttribute("mime", "application/x-kpresenter")
    rootElem.setAttribute("syntaxVersion", "2")
    rootElem.setAttribute("editor", "mgp2kpr import filter, (c) Lukas Tinkl, 2002")
    
    self.__setPaper(rootElem)
    bgElem=self.document.createElement("BACKGROUND")
    objsElem=self.document.createElement("OBJECTS")
    
    for line in fileinput.input(fileIn):
      if (line.startswith('#') or line.startswith('%%')): #skip comments
          continue
      elif (line.startswith('%')):                        #commands
        commands=string.split(string.replace(line, '%', ''),',') #list of commands, comma separated, remove '%'
        for command in commands:
          command=command.strip().lower()
          #print command
          if (command.lower().startswith('page')):        #new page
            self.__handlePage(objsElem, bgElem)
          elif (command.startswith('bgrad')):             #background gradient
            self.__setBgGradient(command)
          elif (command.startswith('deffont')):           #default fonts
            self.__setupDefaultFonts(command)
          elif (command.startswith('default')):           #document defaults TODO!!!
            pass
          elif (command.startswith('xfont')):             #font
            self.__setFont(command)
          elif (command.startswith('font')):              #font from default fonts
            self.__setFontIndirect(command)
          elif (command.startswith('size')):              #font size
            self.__setFontSize(command)
          elif (command.startswith('left') or
                command.startswith('center') or
                command.startswith('right')):             #text alignment
            self.__setAlign(command)
          elif (command.startswith('charset')):           #charset
            self.__setCharset(command)
          elif (command.startswith('fore')):              #font color
            self.__setTextColor(command)
          elif (command.startswith('back')):              #background color
            self.__setBgColor(command)
          elif (command.startswith('nodefault')):         #use default page values?
            self.useDefault=0
          else:
            continue
      else:
        self.__handleText(line)                           #text

    self.__handlePage(objsElem, bgElem)                   #flush the last page

    rootElem.appendChild(bgElem)
    rootElem.appendChild(objsElem)
    self.document.appendChild(rootElem)
    PrettyPrint(self.document)

if __name__ == '__main__':
  if (len(sys.argv)==1):
    print """Magicpoint to KPresenter converter, (c) Lukas Tinkl <lukas@kde.org>, 2002
    Usage: mgp2kpr infile.mgp [outfile.kpr]
    If you give only one parameter, it will output to stdout."""
  else:
    importer=MgpImporter()
    importer.convert(sys.argv[1], sys.argv[2:] or sys.stdout)
