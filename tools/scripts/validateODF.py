#! /usr/bin/python -Qwarnall

import sys, os, tempfile, time, signal, subprocess, re, lxml.etree, zipfile

class odfvalidator:
	def __init__(self):
		path = sys.path[0]
		self.relaxNGValidator = lxml.etree.RelaxNG( \
				lxml.etree.parse(open(os.path.join(path, \
				'OpenDocument-v1.2-cd05-schema.rng'), 'r')))
		self.relaxNGManifextValidator = lxml.etree.RelaxNG( \
				lxml.etree.parse(open(os.path.join(path, \
				'OpenDocument-v1.2-cd05-manifest-schema.rng'), \
				'r')))
	# returns error string on error, None otherwise
	def validate(self, odtpath): 
		zip = zipfile.ZipFile(odtpath, 'r')
		err = self.validateFile(zip, 'content.xml',
				self.relaxNGValidator)
		if (err):
			return err
		err = self.validateFile(zip, 'styles.xml',
				self.relaxNGValidator)
		if (err):
			return err
		err = self.validateFile(zip, 'META-INFO/manifest.xml',
				self.relaxNGManifextValidator)
		if (err):
			return err
		return None

	def validateFile(self, zip, file, validator):
		try:
			xml = lxml.etree.XML(zip.read(file));
		except lxml.etree.XMLSyntaxError as e:
			return file + ':' + str(e)
		if not validator.validate(xml):
			return file + ':' + str(validator.error_log.last_error)

if __name__ == '__main__':
	validator = odfvalidator()
	for f in sys.argv[1:]:
		if os.path.isfile(f):
			e = validator.validate(f)
			if e:
				print str(e)
