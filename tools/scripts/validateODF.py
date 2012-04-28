#! /usr/bin/python -Qwarnall

import sys, os, tempfile, subprocess, lxml.etree, zipfile, urllib, hashlib

def getODFVersion(zip):
	content = lxml.etree.parse(zip.open("content.xml", "r"))
	return content.getroot().get(
		"{urn:oasis:names:tc:opendocument:xmlns:office:1.0}version")

def getJing():
	jingjar = "jing-20091111/bin/jing.jar"
	path = os.path.join(sys.path[0], jingjar)
	if os.path.isfile(path):
		return path
	print "Downloading jing.jar"
	z = "jing-20091111.zip"
	urllib.urlretrieve("http://jing-trang.googlecode.com/files/" + z, z);
	zip = zipfile.ZipFile(z, "r");
	zip.extract(jingjar, sys.path[0])
	os.unlink(z)
	f = open(path, "rb")
	h = hashlib.sha1()
	h.update(f.read())
	f.close()
	hash = h.hexdigest()
	if hash != "daa0cf7b1679264f8e68171f7f253255794773f7":
		print "Wrong hash code: wrong file."
		os.unlink(path)
		return
	return path

schemas = {
	"1.0": ["OpenDocument-schema-v1.0-os.rng",
		"OpenDocument-manifest-schema-v1.0-os.rng"],
	"1.1": ["OpenDocument-schema-v1.1.rng",
		"OpenDocument-manifest-schema-v1.1.rng"],
	"1.2": ["OpenDocument-v1.2-cs01-schema-calligra.rng",
		"OpenDocument-v1.2-cs01-manifest-schema.rng"]
}

class jingodfvalidator:
	def __init__(self, jingjar):
		self.jingjar = jingjar;

	def validate(self, odfpath):
		try:
			zip = zipfile.ZipFile(odfpath, 'r')
		except:
			self.validateFlatXML(odfpath)
			return
		odfversion = getODFVersion(zip)
		if not odfversion in schemas:
			return "Document has no version number"
		err = self.validateFile(zip, 'content.xml',
				schemas[odfversion][0])
		if (err):
			return err
		err = self.validateFile(zip, 'styles.xml',
				schemas[odfversion][0])
		if (err):
			return err
		err = self.validateFile(zip, 'META-INF/manifest.xml',
				schemas[odfversion][1])
		if (err):
			return err
		err = self.validateFile(zip, 'meta.xml',
				schemas[odfversion][0])
		if (err):
			return err
		err = self.validateFile(zip, 'settings.xml',
				schemas[odfversion][0])
		if (err):
			return err
		return None

	def validateFlatXML(self, filepath):
		schema = schemas["1.2"][0]
		schema = os.path.join(sys.path[0], schema)
		r = self.validateXML(schema, filepath)
		if r:
			return filepath + " is not valid."

	def validateFile(self, zip, filepath, schema):
		schema = os.path.join(sys.path[0], schema)
		suffix = "_" + filepath.replace("/", "_")
		tmp = tempfile.NamedTemporaryFile(suffix = suffix)
		tmp.write(zip.open(filepath, "r").read())
		tmp.flush()
		r = self.validateXML(schema, tmp.name)
		tmp.close()
		if r:
			return filepath + " is not valid."

	def validateXML(self, schema, xmlpath):
		args = ["java", "-jar", self.jingjar, "-i", schema, xmlpath]
		return subprocess.call(args)

def createValidator(name):
	xml = lxml.etree.parse(open(os.path.join(sys.path[0], name), "rb"))
	return lxml.etree.RelaxNG(xml)

class odfvalidator:
	def __init__(self):
		path = sys.path[0]
		self.validators = {}
		for key in schemas.keys():
			self.validators[key] = [
				createValidator(schemas[key][0]),
				createValidator(schemas[key][1])
			]
	# returns error string on error, None otherwise
	def validate(self, odfpath): 
		zip = zipfile.ZipFile(odfpath, 'r')
		odfversion = getODFVersion(zip)
		if not odfversion in schemas:
			return "Document has no version number"
		err = self.validateFile(zip, 'content.xml',
				self.validators[odfversion][0])
		if (err):
			return err
		err = self.validateFile(zip, 'styles.xml',
				self.validators[odfversion][0])
		if (err):
			return err
		err = self.validateFile(zip, 'META-INF/manifest.xml',
				self.validators[odfversion][1])
		if (err):
			return err
		return None

	def validateFile(self, zip, file, validator):
		try:
			xml = lxml.etree.XML(zip.read(file));
		except lxml.etree.XMLSyntaxError as e:
			return file + ':' + str(e)
		except KeyError as e:
			return e
		if not validator.validate(xml):
			return file + ':' + str(validator.error_log.last_error)

if __name__ == '__main__':
	jingjar = getJing()
	if jingjar:
		validator = jingodfvalidator(jingjar)
	else:
		validator = odfvalidator()
	for f in sys.argv[1:]:
		if os.path.isfile(f):
			e = validator.validate(f)
			if e:
				print str(e)
