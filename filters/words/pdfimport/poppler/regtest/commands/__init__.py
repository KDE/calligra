# commands
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

import argparse

__all__ = [ 'register_command',
            'print_help',
            'run',
            'UnknownCommandError',
            'Command' ]

class UnknownCommandError(Exception):
    '''Unknown command'''

class Command:

    name = None
    usage_args = '[ options ... ]'
    description = None

    def __init__(self):
        self._parser = argparse.ArgumentParser(
            description = self.description,
            prog = 'poppler-regtest %s' % (self.name),
            usage = 'poppler-regtest %s %s' % (self.name, self.usage_args))

    def _get_args_parser(self):
        return self._parser

    def execute(self, args):
        ns = self._parser.parse_args(args)
        self.run(vars(ns))

    def run(self, options):
        raise NotImplementedError

_commands = {}
def register_command(command_name, command_class):
    _commands[command_name] = command_class

def _get_command(command_name):
    if command_name not in _commands:
        try:
            __import__('commands.%s' % command_name)
        except ImportError:
            pass

    if command_name not in _commands:
        raise UnknownCommandError('Invalid %s command' % command_name)

    return _commands[command_name]

def run(args):
    command_class = _get_command(args[0])
    command = command_class()
    command.execute(args[1:])

def print_help():
    import os
    thisdir = os.path.abspath(os.path.dirname(__file__))

    for fname in os.listdir(os.path.join(thisdir)):
        name, ext = os.path.splitext(fname)
        if not ext == '.py':
            continue
        try:
            __import__('commands.%s' % name)
        except ImportError:
            pass

    print("Commands are:")
    commands = [(x.name, x.description) for x in _commands.values()]
    commands.sort()
    for name, description in commands:
        print("  %-15s %s" % (name, description))

    print
    print("For more information run 'poppler-regtest --help-command <command>'")
