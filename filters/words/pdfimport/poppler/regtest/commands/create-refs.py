# create-refs.py
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

from commands import Command, register_command
from TestReferences import TestReferences
from Timer import Timer
from Config import Config
import os
import tempfile

class CreateRefs(Command):

    name = 'create-refs'
    usage_args = '[ options ... ] tests '
    description = 'Create references for tests'

    def __init__(self):
        Command.__init__(self)
        parser = self._get_args_parser()
        parser.add_argument('--refs-dir',
                            action = 'store', dest = 'refs_dir', default = os.path.join(tempfile.gettempdir(), 'refs'),
                            help = 'Directory where the references will be created')
        parser.add_argument('-f', '--force',
                            action = 'store_true', dest = 'force', default = False,
                            help = 'Create references again for tests that already have references')
        parser.add_argument('-c', '--checksums-only',
                            action = 'store_true', dest = 'checksums_only', default = False,
                            help = 'Leave only checksum files in references dir, other files will be deleted')
        parser.add_argument('tests')

    def run(self, options):
        config = Config()
        config.force = options['force']
        config.checksums_only = options['checksums_only']

        t = Timer()
        doc = options['tests']
        if os.path.isdir(doc):
            docs_dir = doc
        else:
            docs_dir = os.path.dirname(doc)

        refs = TestReferences(docs_dir, options['refs_dir'])
        if doc == docs_dir:
            refs.create_refs()
        else:
            refs.create_refs_for_file(os.path.basename(doc))
        print("Refs created in %s" % (t.elapsed_str()))

register_command('create-refs', CreateRefs)
