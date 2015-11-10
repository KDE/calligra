#!/usr/bin/python -Qwarnall

# This is a simple script to check which icon files are not referenced
# from either mark-up'ed icon names or desktop files.
# Call it from the toplevel source dir of Calligra.

import sys, subprocess, polib

dotDesktopIconsFileName = "dotDesktopIconFiles.list"
calligraIconsFileName = "calligra-icons.list"


# TODO: make path a variable
def createCalligraIconFile(calligraIconsFile):
    r = subprocess.call( ("find . " +
            "-type d \( -name 'doc' -o " +
                       "-name 'docs' -o " +
                       "-name 'tests' \) -prune -o " +
            "! -path './braindump/data/states/states/*' " +
            "! -path './flow/*' " + # not ported for 3.x currently
            "! -path './gemini/themes/*' " +
            "! -path './karbon/stencils/*' " +
            "! -path './kexi/main/status/*' " +
            "! -path './kexi/pics/*_newobj.png' " + # kexi calculates iconname here, needs more complex checl
            "! -path './plan/about/*' " +
            "! -path './plugins/karbonplugins/tools/CalligraphyTool/tutorial/*' " +
            "! -path './sheets/data/sheetstyles/*' " +
            "! -path './stage/pics/animations/*' " + # names are calculated when used, needs more complex check
            "! -path './words/templates/*' " + # find out where gemini style templates refer icon used
            "\( -name '*.png' -o -name '*.svg' -o -name '*.svgz' \) -printf '%%f\n' | " +
            "sed -e 's/\.png$//' -e 's/\.svg$//' -e 's/\.svgz$//' | " +
            "sort -u > %s") % (calligraIconsFile), shell=True)
    return r


# TODO: make path a variable
def createDotDesktopIconFile():
    r = subprocess.call("grep 'Icon=' . -R --exclude-dir='karbon/stencils' --include=\*.desktop > " + dotDesktopIconsFileName, shell=True)
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

sizePrefixes = (
    "16-", "22-", "24-", "32-", "48-", "64-", "128-", "256-", "512-", "1024-", "sc-"
)
groupPrefixes = [
    # KDE 3 compatibility
    "mime-", "filesys-", "device-", "app-", "action-",
    # KDE 4 / icon naming specification compatibility
    "mimetypes-", "places-", "devices-", "apps-", "actions-", "categories-",
    "status-", "emblems-", "emotes-", "animations-", "intl-"
]

elsewhereUsedIcons = (
    "application-x-vnd.kde.plan",
    "application-x-vnd.kde.plan.work",
    "application-x-vnd.kde.kplato",
    "application-x-vnd.kde.kplato.work",
    "application-x-kexi-connectiondata",
    "application-x-kexiproject-shortcut",
    "application-x-kexiproject-sqlite",
    "application-x-kexiproject-sqlite2",
    "application-x-kexiproject-sqlite3",
    "application-x-sqlite2",
    "application-x-sqlite3",
    "calligra-logo-black-glow",
    "calligra-logo-white-glow",
    "tableview_pen", "tableview_plus", "tableview_pointer", # kexi xpm bases
    "questionmark", # for unknown shapes
    "cursor_shear", "cursor_rotate", "cursor_connect", "zoom_out_cursor", "zoom_in_cursor" # cursor images
)


# returns map of iconnames with used filenames
def readIcons(fileName):
    iconNames = {}
    with open(fileName, "r") as f:
        for line in f:
            iconFileName = line.strip().lower()
            if iconFileName:
                iconName = iconFileName;
                if iconName.startswith(sizePrefixes):
                    iconName = iconName.split('-',2)[2]

                if iconName in iconNames:
                    iconNames[iconName].append(iconFileName)
                else:
                    iconNames[iconName] = [iconFileName];
    return iconNames


def readDotDesktopIcons():
    iconNames = set()
    with open(dotDesktopIconsFileName, "r") as f:
        for line in f:
            line = line.strip().lower()
            if line:
                fileName, entry = line.split(':', 1)
                iconName = entry.split('=', 1)[1]
                if iconName:
                    iconNames.add(iconName)
    return iconNames


def main():

    r = createCalligraIconFile(calligraIconsFileName)
    if r:
        return r

    calligraIcons = readIcons(calligraIconsFileName)

    r = createPotFile()
    if r:
        return r

    r = createDotDesktopIconFile()
    if r:
        return r

    po = polib.pofile('koIconNames.po')

    # collect icons and their occurrences
    codeUsedIcons = set()

    for entry in po:
        codeUsedIcons.add(entry.msgid)

    desktopFileUsedIcons = readDotDesktopIcons()

    # output unused icons
    for iconName, iconFileNames in sorted(calligraIcons.iteritems()):
        if not iconName in codeUsedIcons and not iconName in desktopFileUsedIcons and not iconName in elsewhereUsedIcons:
            print iconName
            for filename in iconFileNames:
                print '    %s' % (filename)

    return 0

if __name__ == '__main__':
    main()
