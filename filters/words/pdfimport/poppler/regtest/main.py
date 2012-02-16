# main.py
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

import sys
import argparse
import commands
import backends
import os
from Config import Config

class ListAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string = None):
        setattr(namespace, self.dest, values.split(','))

class HelpAction(argparse.Action):
    def __call__(self, parser, namespace, values, option_string = None):
        if option_string == '--help-command':
            commands.run([values, '--help'])
            sys.exit(0)

        parser.print_help()
        commands.print_help()

        sys.exit(0)

def main(args):
    parser = argparse.ArgumentParser(
        description = 'Poppler regression tests',
        prog = 'poppler-regtest',
        usage = '%(prog)s [options ...] command [command-options ...] tests',
        add_help = False)
    parser.add_argument('-h', '--help',
                        action = HelpAction, nargs = 0)
    parser.add_argument('--help-command', metavar = 'COMMAND',
                        action = HelpAction,
                        help = 'Show help for a given command')
    parser.add_argument('--utils-dir',
                        action = 'store', dest = 'utils_dir', default = os.path.abspath("../utils"),
                        help = 'Directory of poppler utils used for the tests')
    parser.add_argument('-b', '--backends',
                        action = ListAction, dest = 'backends',
                        help = 'List of backends that will be used (separated by comma)')
    parser.add_argument('--skip', metavar = 'FILE',
                        action = 'store', dest = 'skipped_file',
                        help = 'File containing tests to skip')

    ns, args = parser.parse_known_args(args)
    if not args:
        parser.print_help()
        sys.exit(0)

    Config(vars(ns))
    try:
        commands.run(args)
    except commands.UnknownCommandError:
        sys.stderr.write("Unknown command: %s\n" % (args[0]))
        commands.print_help()
        sys.exit(1)
    except backends.UnknownBackendError as e:
        sys.stderr.write(str(e) + "\n")
        sys.stdout.write("Backends are: %s\n" % (", ".join([backend.get_name() for backend in backends.get_all_backends()])))
        sys.exit(1)

if __name__ == '__main__':
    import sys
    main(sys.argv[1:])
