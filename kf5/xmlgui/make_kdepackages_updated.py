# -*- coding: utf-8 -*-
#
# Script to automatically update the "kdepackages.h" file
# FIXME - This is a slow script. Rewrite me using a smart logic. Thanks!
#
import string
import urllib

def unescape(text):
    text = text.replace("&nbsp;"," ")
    text = text.replace("&#8209;","-")
    text = text.replace("&amp;","&")
    return text


print "Fetching products and components from bugs.kde.org..."

pkg = open("kdepackages.h","w")
pkg.write("// DO NOT EDIT - EDIT bugs/Maintainers instead\n")
pkg.write("const char * const packages[] = {\n")

data = urllib.urlopen('http://bugs.kde.org/describecomponents.cgi').read()

for line in string.split(data,' '):
  if line.count("describecomponents.cgi") > 0:
    index1 = line.index("\">")+2
    product = line[index1:len(line)]
    index2 = product.index("<")
    product = product[0:index2]

    link = "describecomponents.cgi?product="
    index1 = line.index(link)
    link = line[index1:len(line)]
    index2 = link.index("\"")
    link = link[0:index2]

    link = 'http://bugs.kde.org/' + link
    data2 = urllib.urlopen(link).read()

    productname = unescape(product)
    print productname
    pkg.write("\"" + productname + "\",\n")
    data2 = string.split(data2,'\n')
    iter = 0
    end = len(data2)
    while( iter < end-1 ):
      iter = iter+1
      line = data2[iter]
      if line.count("<td rowspan=\"2\">")>0:
        iter = iter+1
        line = data2[iter]
        index1 = line.index("\">")+2
        product = line[index1:len(line)]
        index2 = product.index("<")
        product = unescape(product[0:index2])
        if product!="general":
          pkg.write("\"" + productname + "/" + product + "\",\n")
          print productname + "/" + product

pkg.write("0 };\n")
pkg.close()