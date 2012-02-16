# run-tests.py
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
from TestRun import TestRun
from Timer import Timer
from Config import Config
import os
import tempfile

class RunTests(Command):

    name = 'run-tests'
    usage_args = '[ options ... ] tests '
    description = 'Run tests for documents'

    def __init__(self):
        Command.__init__(self)
        parser = self._get_args_parser()
        parser.add_argument('--refs-dir',
                            action = 'store', dest = 'refs_dir', default = os.path.join(tempfile.gettempdir(), 'refs'),
                            help = 'Directory containing the references')
        parser.add_argument('-o', '--out-dir',
                            action = 'store', dest = 'out_dir', default = os.path.join(tempfile.gettempdir(), 'out'),
                            help = 'Directory where test results will be created')
        parser.add_argument('--keep-results',
                            action = 'store_true', dest = 'keep_results', default = False,
                            help = 'Do not remove result files for passing tests')
        parser.add_argument('--create-diffs',
                            action = 'store_true', dest = 'create_diffs', default = False,
                            help = 'Create diff files for failed tests')
        parser.add_argument('--update-refs',
                            action = 'store_true', dest = 'update_refs', default = False,
                            help = 'Update references for failed tests')
        parser.add_argument('tests')

    def run(self, options):
        config = Config()
        config.keep_results = options['keep_results']
        config.create_diffs = options['create_diffs']
        config.update_refs = options['update_refs']

        t = Timer()
        doc = options['tests']
        if os.path.isdir(doc):
            docs_dir = doc
        else:
            docs_dir = os.path.dirname(doc)

        tests = TestRun(docs_dir, options['refs_dir'], options['out_dir'])
        if doc == docs_dir:
            tests.run_tests()
        else:
            tests.run_test(os.path.basename(doc))
        tests.summary()
        print("Tests run in %s" % (t.elapsed_str()))

register_command('run-tests', RunTests)
