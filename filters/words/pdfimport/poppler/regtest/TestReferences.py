# TestReferences.py
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
import errno
from backends import get_backend, get_all_backends
from Config import Config
from Utils import get_document_paths_from_dir, get_skipped_tests

class TestReferences:

    def __init__(self, docsdir, refsdir):
        self._docsdir = docsdir
        self._refsdir = refsdir
        self._skipped = get_skipped_tests(docsdir)
        self.config = Config()

        try:
            os.makedirs(self._refsdir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
        except:
            raise

    def create_refs_for_file(self, filename, n_doc = 1, total_docs = 1):
        if filename in self._skipped:
            print("Skipping test '%s' (%d/%d)" % (os.path.join(self._docsdir, filename), n_doc, total_docs))
            return

        refs_path = os.path.join(self._refsdir, filename)
        try:
            os.makedirs(refs_path)
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
        except:
            raise
        doc_path = os.path.join(self._docsdir, filename)

        if self.config.backends:
            backends = [get_backend(name) for name in self.config.backends]
        else:
            backends = get_all_backends()

        for backend in backends:
            if not self.config.force and backend.has_results(refs_path):
                print("Results found, skipping '%s' for %s backend (%d/%d)" % (doc_path, backend.get_name(), n_doc, total_docs))
                continue
            print("Creating refs for '%s' using %s backend (%d/%d)" % (doc_path, backend.get_name(), n_doc, total_docs))
            if backend.create_refs(doc_path, refs_path):
                backend.create_checksums(refs_path, self.config.checksums_only)

    def create_refs(self):
        docs, total_docs = get_document_paths_from_dir(self._docsdir)
        n_doc = 0
        for doc in docs:
            n_doc += 1
            self.create_refs_for_file(doc, n_doc, total_docs)
