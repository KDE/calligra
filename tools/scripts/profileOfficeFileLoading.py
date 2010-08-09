#! /usr/bin/python -Qwarnall

# This script profiles loading of documents in KOffice.
# It is called like this:
#  profileOfficeFileLoading.py $dir $outputfile
# The script outputs an csv file that contains the times various functions took
# for each file in directory $dir. The directory $dir is scanned recursively.
#
# Copyright 2010 Jos van den Oever <jos@vandenoever.info>

import sys, os, tempfile

applications = {
  'kword': ['odt', 'doc', 'docx'],
  'kpresenter': ['odp', 'ppt', 'pptx'],
  'kspread': ['ods', 'xls', 'xlsx']
}

def scanDirectory(rootdir, extensions):
        if os.path.isfile(rootdir):
                return [rootdir]

        filext = map(lambda e: "." + e, extensions)
        list = []
        for root, directories, files in os.walk(rootdir):
                for file in files:
                        if file.startswith('.'):
                                continue
                        if any(map(lambda e: file.endswith(e), filext)):
                                list.append(os.path.join(root, file))
        return frozenset(list)

class object:
    def __init__(self):
        pass

def profile(file):
	(path, ext) = os.path.splitext(file)
	ext = ext[1:]
	env = os.environ
	env['KDE_DEBUG'] = '0'
	(fileno, tmpfilename) = tempfile.mkstemp()
	exe = None
	for f in applications.keys():
		if ext in applications[f]:
			exe = f
	for p in env['PATH'].split(':'):
		exepath = os.path.join(p, exe)
		if os.path.exists(exepath):
			break
        r = object()
	r.returnValue = os.spawnlpe(os.P_WAIT, exepath, exe,
            "--benchmark-loading", "--profile-filename", tmpfilename, file, env)
	outfile = os.fdopen(fileno, 'r')
	r.lines = outfile.readlines()
	outfile.close()
	os.remove(tmpfilename)
	return r

def summarize(lines):
        r = object()
	r.min = {}
        r.max = {}
	for l in lines:
            (fieldname, time, percentdone) = l.split('\t')
            time = int(time)
            if not fieldname in r.min or r.min[fieldname] > time:
                r.min[fieldname] = time
            if not fieldname in r.max or r.max[fieldname] < time:
                r.max[fieldname] = time
        return r

if __name__ == '__main__':
	dir = sys.argv[1]
        output = sys.argv[2]
	exts = []
	for v in applications.values():
		for e in v:
			exts.append(e);
	officefiles = scanDirectory(dir, exts);
	results = {}
	for f in officefiles:
		result = profile(f)
		results[f] = result

        fields = {}
        for r in results.keys():
            results[r].times = summarize(results[r].lines)
            for f in results[r].times.min:
                if not f in fields:
                    fields[f] = object()
                    fields[f].count = 0
                    fields[f].totalduration = 0
                    fields[f].totaldurationsquared = 0
                    fields[f].min = 10000000
                min = results[r].times.min[f]
                duration = results[r].times.max[f] - min
                fields[f].totalduration += duration
                fields[f].totaldurationsquared += duration*duration
                fields[f].count += 1
                if fields[f].min > min:
                    fields[f].min = min
        fieldnames = fields.keys()
        def sort(keya, keyb):
            return cmp(fields[keya].min, fields[keyb].min)
        fieldnames.sort(sort)

        # collect all fieldnames
	out = open(output, 'wb')
        # write header
        out.write('filename\text\tsize\tr')
        for f in fieldnames:
            out.write('\t' + f)
        out.write('\n')
        # write average
        out.write('average\t\t\t')
        for f in fieldnames:
            f = fields[f]
            out.write('\t')
            if f.count > 0:
                out.write(str(f.totalduration/f.count))
        out.write('\n')
        # write for each analyzed file
        for file in results.keys():
            r = results[file]
            (path, ext) = os.path.splitext(file)
            stat = os.stat(file)
            out.write(file + '\t' + ext[1:] + '\t' + str(stat.st_size) + '\t' + str(r.returnValue))
            for f in fieldnames:
                out.write('\t')
                if f in r.times.min:
                    out.write(str(r.times.max[f]-r.times.min[f]))
            out.write('\n')
        out.close()
