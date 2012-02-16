# postscript.py
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

from backends import Backend, register_backend
import subprocess
import os

class PostScript(Backend):

    def __init__(self, name):
        Backend.__init__(self, name)
        self._pdftops = os.path.join(self._utilsdir, 'pdftops');

    def create_refs(self, doc_path, refs_path):
        out_path = os.path.join(refs_path, 'postscript')
        p = subprocess.Popen([self._pdftops, doc_path, out_path + '.ps'], stderr = subprocess.PIPE)
        return self._check_exit_status(p, out_path)

register_backend('postscript', PostScript)

