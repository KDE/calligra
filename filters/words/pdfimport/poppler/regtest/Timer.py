# Timer.py
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

from time import time, strftime, gmtime

class Timer:

    def __init__(self, start = True):
        self._stop = None
        if start:
            self.start()
        else:
            self._start = None

    def start(self):
        self._start = time()

    def stop(self):
        self._stop = time()

    def elapsed(self):
        if self._start is None:
            return 0

        if self._stop is None:
            return time() - self._start

        return self._stop - self._start

    def elapsed_str(self):
        h, m, s = [int(i) for i in strftime('%H:%M:%S', gmtime(self.elapsed())).split(':')]
        retval = "%d seconds" % (s)
        if h == 0 and m == 0:
            return retval

        retval = "%d minutes and %s" % (m, retval)
        if h == 0:
            return retval

        retval = "%d hours, %s" % (h, retval)
        return retval


if __name__ == '__main__':
    from time import sleep

    t = Timer()
    sleep(5)
    print("Elapsed: %s" % (t.elapsed_str()))
    sleep(1)
    print("Elapsed: %s" % (t.elapsed_str()))

    t.start()
    sleep(2)
    t.stop()
    print("Elapsed: %s" % (t.elapsed_str()))
    sleep(2)
    print("Elapsed: %s" % (t.elapsed_str()))
