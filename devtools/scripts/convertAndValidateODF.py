#! /usr/bin/python -Qwarnall
#
# -*- coding: latin-1 -*-
#
# This script is run by ./validateODF.py
# Author: unknown
# Maintainer: Hanna Skott
#
# This file contains a script that converts documents to ODF format using calligraconverter
# it also validates the content of the resulting ODF file against RelaxNG XML using jing.
#
# EXAMPLE of use: $ ./convertAndValidateODF.py validate_or_not file_directory filename
# validate_or_not values are: yes, no
# NOTE! Jing jar has to be in the same directory as this script for this to work, and it must be named jing.jar
#  

import sys, os, os.path, tempfile, subprocess, lxml.etree, zipfile, urllib, hashlib, shlex, shutil, re, getopt

# this function gets jing, then for each file in fileList it converts the file, and validates its XML against RelaxNG
def convertAndValidateFilesInDir(dir):
	print "dir is "+dir
        # insert the path to the directory of interest here
	for root, dirs, files in os.walk(dir):
		for name in files:
			singleFileConvertAndValidate(name, root)

#This function converts a file using calligraconverter and then tries to validate the resulting ODF file against RelaxNG using jing
def singleFileConvertAndValidate(filename, validate):
	
	filepath = filename
        # Create a filename for the output odt file
        filename, extension = os.path.splitext(filepath)

        src_extension = getExtByMime(filepath)
        if src_extension is not None:
        	# Create filename for where conversion is stored
        	dst_extension = getConvertExtension(src_extension)
                convertedfile = filepath + "." + dst_extension
        	
        	if "." + dst_extension == src_extension:

                    applicationname = getApplicationName(dst_extension)
                    # Do the conversion
                    args = [applicationname, "--roundtrip-filename", convertedfile, filepath]
                else:
                    args = ["calligraconverter", "--batch", filepath, convertedfile]
                    
                print args
                fnull = open(os.devnull, 'w')
                p = subprocess.call(args, stdout = fnull, stderr = fnull)
                

               	# validate out.odt
                if not os.path.exists(convertedfile):
                        print "converted file "+convertedfile+" does not EXIST - Conversion failed!"
			return 1
                else:	
			if validate:
				# Get jing and validate the odf file according to RelaxNG
                        	jingjar = newGetJing()
                        	validator = jingodfvalidator(jingjar)
                        	e = validator.validate(convertedfile)
                        	if not e:
					print "file "+convertedfile+" did not validate against RelaxNG - validation failed!"
					removeFileCommand = ["rm", "-f", convertedfile]
                			p = subprocess.call(removeFileCommand)
						
                                	return 1
                        	else:
					removeFileCommand = ["rm", "-f", convertedfile]
                			p = subprocess.call(removeFileCommand)
					return 0
			else:	
				removeFileCommand = ["rm", "-f", convertedfile]
				p = subprocess.call(removeFileCommand)
				return 0
	else:
        	print "file "+filename+" is not of a file format that can be converted to ODF"
               	return 1


	
#def getConversionResultingMime(file):
#        file_extension = file.split(".")[-1]
#        if file_extension:
#                if file_extension == ".doc" or file_extension == ".docx" or file_extension == ".txt":
#			textMime = "odt"
#			return textMime
#                if file_extension == ".ppt" or file_extension == ".pptx":
#			pptMime = "odp"
#			return pptMime
#                if file_extension == ".csv" or file_extension == ".xls" or file_extension == ".xlsx":
#			spreadsheetMime = "ods"
#			return spreadsheetMime
#		else:
#			return None

#This function gets the input file formats closest match in ODF format for version
def getExtByMime(filename):
        (path, pathext) = os.path.splitext(filename)
        return pathext

def getConvertExtension(extension):
    if extension == ".odt" or extension == ".docx" or extension == ".doc" or extension == ".txt":
        return "odt"
    if extension == ".odp" or extension == ".ppt" or extension == ".pptx":
        return "odp"
    if extension == ".ods" or extension == ".xls" or extension == ".xlsx":
        return "ods"


def getApplicationName(extension):
    if extension == "odt":
        return "calligrawords"
    if extension == "odp":
        return "calligrastage"
    if extension == "ods":
        return "calligrasheets"


def getODFVersion(zip):
	content = lxml.etree.parse(zip.open("content.xml", "r"))
	return content.getroot().get(
		"{urn:oasis:names:tc:opendocument:xmlns:office:1.0}version")

def newGetJing():
	currentdir = os.curdir
	jingjar = os.path.join(currentdir, "jing.jar")
	path = os.path.join(sys.path[0], jingjar)
	if os.path.isfile(path):
		return path
		

def getJing():
	jingjar = "jing-20091111/bin/jing.jar"
	# jingjar = "jing-20081028/bin/jing.jar"
	path = os.path.join(sys.path[0], jingjar)
	if os.path.isfile(path):
		return path
	print "Downloading jing.jar"
	z = "jing-20091111.zip"
	
	# VALID 2011-03-29
	urllib.urlretrieve("http://code.google.com/p/jing-trang/downloads/detail?name=jing-20091111.zip", z);
	
	# urllib.urlretrieve("http://jing-trang.googlecode.com/files/" + z, z);
	zip = zipfile.ZipFile(z, "r");
	zip.extract(jingjar, sys.path[0])
	os.unlink(z)
	f = open(path, "rb")
	h = hashlib.sha1()
	h.update(f.read())
	f.close()
	hash = h.hexdigest()
	
	# hash for jing 2011-03-29 
	if hash != "2e8eacf399249d226ad4f6ca1d6907ff69430118":
	# if hash != "60197956be7f8f2e29e1941ca42273abe7315293":
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
		zip = zipfile.ZipFile(odfpath, 'r')
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
		return None

	def validateFile(self, zip, filepath, schema):
		schema = os.path.join(sys.path[0], schema)
		suffix = "_" + filepath.replace("/", "_")
		tmp = tempfile.NamedTemporaryFile(suffix = suffix)
		tmp.write(zip.open(filepath, "r").read())
		tmp.flush()
		args = ["java", "-jar", self.jingjar, "-i", schema, tmp.name]
		r = subprocess.call(args)
		tmp.close()
		if r:
			return filepath + " is not valid."

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

#
if __name__ == '__main__':

	if sys.argv[1] and sys.argv[2] and sys.argv[3]:
		if not os.path.exists(sys.argv[2]):
			print "Wrong use of script: missing path to directory in which file resides"
			sys.exit(1)
		else:
			#NOTE! sys.argv[1] is hardcoded to "no" in CMakeList.txt under /calligra/tools/scripts/
			if sys.argv[1] == "yes":
				jingjar = newGetJing()
                                if jingjar:
                                       validator = jingodfvalidator(jingjar)
                                else:
                                       validator = odfvalidator()
			
				filepath = os.path.abspath(os.path.join(sys.argv[2], sys.argv[3]))
                		if os.path.exists(filepath):
                                	ret = singleFileConvertAndValidate(filepath, True)
					sys.exit(ret)
			elif sys.argv[1] == "no":
				filepath = os.path.abspath(os.path.join(sys.argv[2], sys.argv[3]))
				print filepath
                		if os.path.exists(filepath):
                                	ret = singleFileConvertAndValidate(filepath, False)
                                	sys.exit(ret)
			else:
				print "Wrong use of script: validation neither yes or no"
				sys.exit(1)
	else:
		print "Wrong use of script: parameters missing:" 
		sys.exit(1)
