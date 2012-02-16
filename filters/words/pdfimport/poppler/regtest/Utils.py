# Utils.py
#
# Copyright (C) 2011 Carlos Garcia Campos <carlosgc@gnome.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import os

def get_document_paths_from_dir(docsdir):
    paths = []
    n_paths = 0
    for root, dirs, files in os.walk(docsdir, False):
        for entry in files:
            if not entry.lower().endswith('.pdf'):
                continue

            test_path = os.path.join(root[len(docsdir):], entry)
            paths.append(test_path.lstrip(os.path.sep))
            n_paths += 1
    paths.sort()
    return paths, n_paths

def get_skipped_tests(docsdir):
    from Config import Config
    config = Config()
    if config.skipped_file:
        skipped_file = config.skipped_file
    elif os.path.exists(os.path.join(docsdir, 'Skipped')):
        skipped_file = os.path.join(docsdir, 'Skipped')
    else:
        return []

    skipped = []
    f = open(skipped_file, 'r')
    for line in f.readlines():
        line = line.rstrip('\n \t\b\r')
        if not line or line[0] == '#':
            continue
        skipped.append(line)
    f.close()
    return skipped


