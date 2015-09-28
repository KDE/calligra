#! /usr/bin/python -Qwarnall

# This script profiles loading of documents in Calligra.
# It is called like this:
#  profileOfficeFileLoading.py $dir $outputfile
# The script outputs an csv file that contains the times various functions took
# for each file in directory $dir. The directory $dir is scanned recursively.
#
# Copyright 2010 Jos van den Oever <jos@vandenoever.info>

import sys, os, tempfile, time, signal, subprocess, re, lxml.etree, zipfile

applications = {
  'calligrawords': ['odt', 'doc', 'docx'],
  'calligrastage': ['odp', 'ppt', 'pptx'],
  'calligrasheets': ['ods', 'xls', 'xlsx']
}
extensions = {
  'odt': 'odt',
  'doc': 'odt',
  'docx': 'odt',
  'odp': 'odp',
  'ppt': 'odp',
  'pptx': 'odp',
  'ods': 'ods',
  'xls': 'ods',
  'xlsx': 'ods'
}

# limit how many backtraces are recordes, since it takes a lot of time
maxbacktraces = 50

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

def getExtensions(list):
	extensions = []
	for file in list:
		(path, ext) = os.path.splitext(file)
		extensions.append(ext)
	return frozenset(extensions)

class object:
	def __init__(self):
		pass

# functions for logging, in this case to TeamCity
class logger:
	def __init__(self):
		self.suitename = None
	def escape(self, text):
		def escape(m):
			if m.group(0) == '\n':
				return '|n'
			if m.group(0) == '\r':
				return '|r'
			return '|'+m.group(0)
		return re.sub('[\'\n\r\|\]]', escape, text)
	def startTestSuite(self, name):
		self.suitename = name
		print "##teamcity[testSuiteStarted name='" + self.suitename \
			+ "']"
		sys.stdout.flush()
	def endTestSuite(self):
		if not self.suitename: return
		print "##teamcity[testSuiteFinished name='" + self.suitename \
			+ "']"
		sys.stdout.flush()
		self.suitename = None
	def startTest(self, name):
		if not self.suitename: return
		self.testname = name
		print "##teamcity[testStarted name='" + self.testname + "']"
		sys.stdout.flush()
	# fail the current test
	def failTest(self, backtrace):
		if not self.suitename or not self.testname: return
		bt = ''
		for l in backtrace:
			bt = bt + self.escape(l)
		print "##teamcity[testFailed name='" + self.testname \
			+ "' details='" + bt + "']"
		sys.stdout.flush()
	# end test, pass duration as integer representing the milliseconds
	def endTest(self, duration):
		if not self.suitename or not self.testname: return
		print "##teamcity[testFinished name='" + self.testname \
			+ "' duration='" + str(duration) + "']"
		sys.stdout.flush()
		self.testname = None

def containsRealError(err):
	# some errors reported by libxml2 are false positives, we filter them
	# out
	if str(err).find("ERROR:RELAXNGV:RELAXNG_ERR_CONTENTVALID: Element styles failed to validate content") != -1:
		return None
	if str(err).find("ERROR:RELAXNGV:RELAXNG_ERR_CONTENTVALID: Element automatic-styles failed to validate content") != -1:
		return None
	return err

class odfvalidator:
	def __init__(self):
		path = sys.path[0]
		self.relaxNGValidator = lxml.etree.RelaxNG( \
				lxml.etree.parse(open(os.path.join(path, \
				'OpenDocument-v1.2-cs01-schema-calligra.rng'),
				'r')))
		self.relaxNGManifextValidator = lxml.etree.RelaxNG( \
				lxml.etree.parse(open(os.path.join(path, \
				'OpenDocument-v1.2-cs01-manifest-schema.rng'), \
				'r')))
	# returns error string on error, None otherwise
	def validate(self, odtpath): 
		try:
			zip = zipfile.ZipFile(odtpath, 'r')
		except zipfile.BadZipfile as e:
			return str(e)
		err = self.validateFile(zip, 'content.xml',
				self.relaxNGValidator)
		if (containsRealError(err)):
			return err
		err = self.validateFile(zip, 'styles.xml',
				self.relaxNGValidator)
		if (containsRealError(err)):
			return err
		err = self.validateFile(zip, 'META-INF/manifest.xml',
				self.relaxNGManifextValidator)
		if (containsRealError(err)):
			return err
		return None

	def validateFile(self, zip, file, validator):
		try:
			data = zip.read(file)
			xml = lxml.etree.XML(data);
		except lxml.etree.XMLSyntaxError as e:
			return e
		except KeyError as e:
			return e
		if len(data) > 1000000:
			# if the xml file is larger than 1M, the validator may hang
			return
		if not validator.validate(xml):
			return validator.error_log.last_error

def getExecutablePath(exe):
	exepath = None
	env = os.environ
	for p in env['PATH'].split(':'):
		exepath = os.path.join(p, exe)
		if os.path.exists(exepath):
			break
	return exepath

def runCommand(exepath, arguments, captureStdOut):
	env = os.environ
	stdout = None
	if captureStdOut:
		(fileno, tmpfilename) = tempfile.mkstemp()
		stdout = os.fdopen(fileno, 'w')

	args = 'ulimit -v 1000000 -t 60 -c 0;' + exepath
	for s in arguments:
		args += ' "' + s + '"'

	process = subprocess.Popen(['/bin/bash', '-c', args], env=env,
		close_fds=True, stdout=stdout, stderr=None)
	s = os.wait4(process.pid, os.WNOHANG)
	waited = 0
	waitstep = 0.1
	maxwaittime = 65
	while  s[0] == 0 and s[1] == 0 and waited < maxwaittime:
		# wait a bit
		time.sleep(waitstep)
		waited += waitstep
		s = os.wait4(process.pid, os.WNOHANG)
	if waited >= maxwaittime:
		# if the process did not end nicely, kill it
		try:
			os.killpg(process.pid, signal.SIGKILL)
		except:
			pass
		try:
			s = os.wait4(process.pid, 0)
		except:
			pass
	r = object()
	r.utime = s[2].ru_utime
	r.stime = s[2].ru_stime
	r.returnValue = s[1]
	r.stdout = []
	if captureStdOut:
		stdout.close()
		stdout = open(tmpfilename, 'r')
		r.stdout = stdout.readlines()
		stdout.close()
		os.remove(tmpfilename)
	return r

def profile(dir, file, logger, validator):
	global maxbacktraces
	logger.startTest(file)
	file = os.path.join(dir, file)
	(path, ext) = os.path.splitext(file)
	ext = ext[1:]
	exe = None
	for f in applications.keys():
		if ext in applications[f]:
			exe = f
	exepath = getExecutablePath(exe)

	# profile
	(fileno, tmpfilename) = tempfile.mkstemp()
	roundtripfilename = None
	args = []
	# in case of ODF file, do a roundtrip
	m = re.match('.*(\.od.)$', file)
	(roundtripfd, roundtripfilename) = tempfile.mkstemp("." + extensions[ext])
	if m:
		isOdfFile = True
	else:
		isOdfFile = False
	if isOdfFile:
		args += ["--roundtrip-filename", roundtripfilename]
	args += ["--benchmark-loading", "--profile-filename", tmpfilename,
		"--nocrashhandler", file]
	r = runCommand(exepath, args, False)
	outfile = os.fdopen(fileno, 'r')
	r.lines = outfile.readlines()
	outfile.close()
	os.close(roundtripfd)
	r.backtrace = None
	if r.returnValue != 0:
		if maxbacktraces > 0:
	                maxbacktraces -= 1
			# generate a backtrace
			args = ["--batch", "--eval-command=run",
				"--eval-command=bt", "--args"] + [exepath] + args
			exepath = getExecutablePath("gdb")
			debugresult = runCommand(exepath, args, True)
			r.backtrace = debugresult.stdout
			for l in r.backtrace:
				print l.rstrip()
				sys.stdout.flush()
			logger.failTest(r.backtrace)
		else:
			logger.failTest("Crash, no backtrace: limit reached.")
	else:
		if not isOdfFile:
			# convert ms office file to odf
			exepath = getExecutablePath("calligraconverter")
			args = [file, roundtripfilename]
			cr = runCommand(exepath, args, False)

		err = validator.validate(roundtripfilename);
		if err != None:
			logger.failTest(str(err))

	# remove the roundtripfile and the temporary file
	os.remove(tmpfilename)
	os.remove(roundtripfilename)

	logger.endTest(int((r.utime + r.stime)*1000))
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
	if len(times.min.keys()) == 0:
		return 0

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

def profileAll(dir, loggername):
	exts = []
	for v in applications.values():
		for e in v:
			exts.append(e);
	officefiles = scanDirectory(dir, exts);
	usedExts = getExtensions(officefiles)
	results = {}
	log = logger()
	validator = odfvalidator()
	for ext in usedExts:
		if loggername:
			log.startTestSuite(loggername + '-' + ext[1:])
		for f in officefiles:
			(path, pathext) = os.path.splitext(f)
			if pathext == ext:
				relf = os.path.relpath(f, dir)
				result = profile(dir, relf, log, validator)
				results[f] = result
		log.endTestSuite()
	return results

def addMapEntry(map, start, end):
	if start in map:
		a = map[start]
		if end in a:
			a[end] = a[end] + 1
		else:
			a[end] = 1
	else:
		a = {}
		a[end] = 1
	map[start] = a
	return map

def createStackTraceGraph(results):
	exepath = getExecutablePath('dot')
	if not exepath:
		return
	edges = {}
	nodes = {}
	n = 0
	for file in results.keys():
		r = results[file]
		if not r.backtrace:
			continue
		name = os.path.basename(file)
		nodes[name] = file
		for l in r.backtrace:
			l = l.rstrip()
			n += 1
			m = re.search('/calligra/.*/([^/]+:\d+)$', l)
			if m != None:
				key = m.group(1)
				nodes[key] = l
				edges = addMapEntry(edges, key, name)
				name = key

	(fileno, tmpfilename) = tempfile.mkstemp()
	out = os.fdopen(fileno, 'w')
	out.write('digraph {')
	svn = 'http://websvn.kde.org/trunk'
	for a in nodes:
		m = re.search('(/calligra/.*):(\d+)$', nodes[a])
		n = '"' + a + '" [URL = "'
		if m:
			out.write(n + svn + m.group(1) + '?view=markup#l'
				+ m.group(2) + '"];')
		else:
			m = re.search('(/calligratests/.*)', nodes[a])
			if m:
				out.write(n + svn + '/tests' + m.group(1)
					+ '"];')
	for a in edges.keys():
		for b in edges[a].keys():
			out.write('"' + a + '" -> "' + b + '" [penwidth='
				+ str(edges[a][b]) + '];')
	out.write('}')
	os.fdatasync(fileno)
	out.close()

	args = ["-Tsvg", "-ostacktraces.svg", tmpfilename]
	r = runCommand(exepath, args, False)
	os.remove(tmpfilename)

if __name__ == '__main__':
	dir = sys.argv[1]
	output = sys.argv[2]
	loggername = None
	if len(sys.argv) > 3:
		loggername = sys.argv[3]
	results = profileAll(dir, loggername)

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

	createStackTraceGraph(results)
