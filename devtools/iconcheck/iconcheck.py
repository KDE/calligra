#!/usr/bin/python -Qwarnall

# This is a simple script to check which mark-up'ed icon names are backed with real icons
# Call it from the toplevel source dir of Calligra.

import sys, subprocess, polib

class MissingIcon:
    def __init__(self, iconName):
        self.iconName = iconName
        self.fileLocations = []

    def addPoFileLocations(self, poFileLocations, context):
        for filePath, lineNumber in poFileLocations:
            self.fileLocations.append((filePath, lineNumber, context))

    def addDesktopFileLocation(self, filePath):
        self.fileLocations.append((filePath, None, None))

# TODO: make path a variable
def createCalligraIconFile(calligraIconsFile):
    r = subprocess.call("find . \( -name '*.png' -o -name '*.svg' -o -name '*.svgz' \) -printf '%%f\n' | sed -e 's/\.png$//' -e 's/\.svg$//' -e 's/\.svgz$//' | sort -u > %s" % (calligraIconsFile), shell=True)
    return r


# TODO: make path a variable
def createDotDesktopIconFile():
    r = subprocess.call("GREP_OPTIONS= grep 'Icon=' . -R --include=\*.desktop > dotDesktopIconFiles.list", shell=True)
    return r

# TODO: make path a variable
def createPotFile():
    r = subprocess.call("find . -name '*.cpp' -o -name '*.cc' -o -name '*.h' -o -name '*.c' | sort > koIconFiles.list", shell=True)
    if r:
        return r
    r = subprocess.call("xgettext --from-code=UTF-8 -k " +
             "-kkoIcon:1 " +
             "-kkoIconName:1 " +
             "-kkoIconNameCStr:1 " +
             "-kkoSmallIcon:1 " +
             "-kkoDesktopIcon:1 " +

             "-kkoIconNeeded:1c,2 " +
             "-kkoIconNeededWithSubs:1c,2 " +
             "-kkoIconNameNeeded:1c,2 " +
             "-kkoIconNameNeededWithSubs:1c,2 " +
             "-kkoIconNameCStrNeeded:1c,2 " +
             "-kkoIconNameCStrNeededWithSubs:1c,2 " +

             "-kkoIconWanted:1c,2 " +
             "-kkoIconNameWanted:1c,2 " +

             "-D . --files-from=koIconFiles.list -o koIconNames.po -n", shell=True)
    return r
    #TODO: use Python pipes and/or clean-up helper files

themePrefixes = (
    "hi16-", "hi22-", "hi32-", "hi48-", "hi64-", "hi128-", "hisc-",
    "ox16-", "ox22-", "ox32-", "ox48-", "ox64-", "ox128-", "oxsc-"
)
groupPrefixes = [
    # KDE 3 compatibility
    "mime-", "filesys-", "device-", "app-", "action-",
    # KDE 4 / icon naming specification compatibility
    "mimetypes-", "places-", "devices-", "apps-", "actions-", "categories-",
    "status-", "emblems-", "emotes-", "animations-", "intl-"
]

def readIcons(fileName):
    iconNames = []
    with open(fileName, "r") as f:
        for line in f:
            iconName = line.strip().lower()
            if iconName:
                if iconName.startswith(themePrefixes):
                    iconName = iconName.split('-',2)[2]
                iconNames.append(iconName)
    return set(iconNames)


def readDotDesktopIcons(missingIconData):
    with open("dotDesktopIconFiles.list", "r") as f:
        for line in f:
            line = line.strip().lower()
            if line:
                fileName, entry = line.split(':', 1)
                iconName = entry.split('=', 1)[1]
                if iconName:
                    if iconName in missingIconData:
                        missingIconData[iconName].addDesktopFileLocation(fileName)
                    else:
                        missingIcon = MissingIcon(iconName)
                        missingIcon.addDesktopFileLocation(fileName)
                        missingIconData[iconName] = missingIcon


def main():
    #if len(sys.argv) < 1:
        #print >> sys.stderr, "usage: %s directory" % sys.argv[0]
        #sys.exit(1)

    oxygenIcons = readIcons('tools/iconcheck/oxygen-icons-4.6.0.list')

    r = createCalligraIconFile('calligra-icons.list')
    if r:
        return r

    calligraIcons = readIcons('calligra-icons.list')

    r = createPotFile()
    if r:
        return r

    r = createDotDesktopIconFile()
    if r:
        return r

    po = polib.pofile('koIconNames.po')

    # collect icons and their occurrences
    missingIconData = {}

    for entry in po:
        iconName = entry.msgid
        if iconName in missingIconData:
            missingIconData[iconName].addPoFileLocations(entry.occurrences, entry.comment)
        else:
            missingIcon = MissingIcon(iconName)
            missingIcon.addPoFileLocations(entry.occurrences, entry.msgctxt)
            missingIconData[iconName] = missingIcon

    readDotDesktopIcons(missingIconData)

    # output missing icons
    for iconName, missingIcon in missingIconData.iteritems():
        if not iconName in oxygenIcons:
            if not iconName in calligraIcons:
                print iconName
                for filePath, lineNumber, comment in missingIcon.fileLocations:
                    if comment:
                        print '    # %s' % (comment)
                    if lineNumber:
                        print '    %s:%s' % (filePath, lineNumber)
                    else:
                        print '    %s' % (filePath)

    return 0

if __name__ == '__main__':
    main()
