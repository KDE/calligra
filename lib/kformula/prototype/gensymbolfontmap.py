#!/usr/bin/env python

import time
from xml.sax import saxutils, handler, make_parser

table = {}

class ContentGenerator(handler.ContentHandler):  

    def startElement(self, name, attrs):
        if name == 'entry':
            number = ''
            for (name, value) in attrs.items():
                if name == "key": key = int(value)
                elif name == "number": number = value
                elif name == "name": latexName = value
                elif name == "class": charClass = value

            if number != '':
                table[number] = (key, latexName, charClass)


def main():
    parser = make_parser()
    parser.setContentHandler(ContentGenerator())
    parser.parse("symbol.xml")

    f = open('../symbolfontmapping.cc', 'w')
    f.write('''//
// Created: ''' + time.ctime(time.time()) + '''
//      by: gensymbolfontmap.py
//    from: symbol.xml
//
// WARNING! All changes made in this file will be lost!

''')
    for key in table.keys():
        pos, latexName, charClass = table[key]
        if len(latexName) > 0:
            f.write('char symbolFontMap_' + latexName.replace('\\', '') + '[] = "' +
                    latexName.replace('\\', '\\\\') + '";\n' )
    f.write('\nstruct { int unicode; uchar pos; CharClass cl; char* latexName; } symbolFontMap[] = {\n')
    for key in table.keys():
        pos, latexName, charClass = table[key]
        if len(latexName) > 0:
            latexName = 'symbolFontMap_' + latexName.replace('\\', '')
        else:
            latexName = '0'
        f.write('    { ' + key + ', ' + `pos` + ', ' + charClass + ', ' +
                latexName + ' },\n')
    f.write('    { 0, 0, ORDINARY, 0 }\n};\n\n')

    f.close()

if __name__ == '__main__':
    main()
