#! /usr/bin/python -Qwarnall

# This script profiles loading of documents in KOffice.
# It is called like this:
#  profileOfficeFileLoading.py $dir $outputfile
# The script outputs an csv file that contains the times various functions took
# for each file in directory $dir. The directory $dir is scanned recursively.
#
# Copyright 2010 Jos van den Oever <jos@vandenoever.info>

import sys, os, tempfile, time, signal

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
	pid = os.spawnlpe(os.P_NOWAIT, exepath, exe,
		"--benchmark-loading", "--profile-filename", tmpfilename, file, env)
	s = os.wait4(pid, os.WNOHANG)
	waited = 0
	waitstep = 0.1
	maxwaittime = 60
	while  s[0] == 0 and s[1] == 0 and waited < maxwaittime:
		time.sleep(waitstep)
		waited += waitstep
		s = os.wait3(os.WNOHANG)
	try:
		os.kill(pid, signal.SIGKILL)
	except:
		pass
	r.utime = s[2].ru_utime
	r.stime = s[2].ru_stime
	r.returnValue = s[1]
	if waited >= maxwaittime:
		r.returnValue = -1
	outfile = os.fdopen(fileno, 'r')
	r.lines = outfile.readlines()
	outfile.close()
	os.remove(tmpfilename)
	return r

def getGlobalMinMax(times):
	keys = times.min.keys()
	min = times.min[keys[0]]
	max = times.max[keys[0]]
	for k in keys:
		if times.min[k] <= min:
			min = times.min[k]
		if times.max[k] >= max:
			max = times.max[k]
	return (min, max)

def getGlobalTask(times):
	keys = times.min.keys()
	if len(keys) == 0:
		return None
	(min, max) = getGlobalMinMax(times)
	name = None
	for k in keys:
		if times.min[k] == min and times.max[k] == max:
			name = k
	return name

def getFurthestEnd(start, times, excludedkey):
	end = start
	for k in times.min.keys():
		if times.min[k] <= start and times.max[k] > end and k != excludedkey:
			end = times.max[k]
	if end == start:
		end = None
	return end

def getClosestStart(start, times, excludedkey):
	s = 10000000
	for k in times.min.keys():
		if times.min[k] > start and times.min[k] < s and k != excludedkey:
			s = times.min[k]
	return s

# if there is one task that starts before all other tasks and ends after all
# other tasks, then count the time that there is no other task running as a sub-
# task of this global task
# if there is no global task, then count the time between the first and last
# timepoint where there is no task running
def getUnaccountedTime(lines, times):
	globaltask = getGlobalTask(times)
	(min, max) = getGlobalMinMax(times)
	unaccounted = 0
	currentmax = min
	currentmin = min
	while currentmax != max:
		s = getFurthestEnd(currentmax, times, globaltask)
		if s == None:
			s = getClosestStart(currentmax, times, globaltask)
			if s > max:
				s = max
			unaccounted += s - currentmax
		currentmax = s
	return unaccounted

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
		results[r].unaccounted = getUnaccountedTime(results[r].lines,
			results[r].times)
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
	out.write('filename\text\tsize\tr\tutime\tstime\tunaccounted')
	for f in fieldnames:
		out.write('\t' + f)
	out.write('\n')
	# write average
	out.write('average\t\t\t\t\t\t')
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
		out.write('\t' + str(r.utime) + '\t' + str(r.stime))
		out.write('\t' + str(r.unaccounted))
		for f in fieldnames:
			out.write('\t')
			if f in r.times.min:
				out.write(str(r.times.max[f]-r.times.min[f]))
		out.write('\n')
	out.close()
