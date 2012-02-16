# text.py
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

class Text(Backend):

    def __init__(self, name):
        Backend.__init__(self, name)
        self._pdftotext = os.path.join(self._utilsdir, 'pdftotext');

    def create_refs(self, doc_path, refs_path):
        out_path = os.path.join(refs_path, 'text')
        p = subprocess.Popen([self._pdftotext, doc_path, out_path + '.txt'], stderr = subprocess.PIPE)
        return self._check_exit_status(p, out_path)

    def _create_diff(self, ref_path, result_path):
        import difflib

        ref = open(ref_path, 'r')
        result = open(result_path, 'r')
        diff = difflib.unified_diff(ref.readlines(), result.readlines(), ref_path, result_path)
        ref.close()
        result.close()

        diff_file = open(result_path + '.diff', 'w')
        diff_file.writelines(diff)
        diff_file.close()

register_backend('text', Text)

