#!/usr/bin/python

import sys
import os
import shutil
import subprocess

install_dir = '/Applications/Calligra/'
if len(sys.argv) > 1:
    install_dir = sys.argv[1]

if os.path.exists(install_dir):
    print 'Deleting existing install dir ' + install_dir + '...'
    shutil.rmtree(install_dir)

os.makedirs(install_dir)
print 'Install dir ' + install_dir + ' created.'

print 'Generating folder icon...'
icns = install_dir + 'Calligra.icns'
subprocess.call(['iconutil',
                 '-c', 'icns',
                 '-o', icns,
                 'resources/CalligraDir.iconset'])
subprocess.call(['resources/set_folder_icon.py',
                 icns,
                 install_dir])
print 'Folder icon updated.'
print 'Done.'
