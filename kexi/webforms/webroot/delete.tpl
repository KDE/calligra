<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html 
	  PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
	  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
    <head>
	<title>Kexi Web Forms - {{TITLE}}</title>
	<link rel="stylesheet" type="text/css" href="/style.css"/>
    </head>
    <body>
	<div id="wrapper">
	    <div id="logo"></div>
	    <div id="dbname"><h2>{{TITLE}}</h2></div>
	    <div style="clear: both;"></div>
	    <div id="content">
		<h3>Deleting row {{PKEY}} in table {{TABLENAME}}</h3>
		<a href="/read/{{TABLENAME}}">Table contents</a>

		{{#SUCCESS}}<h4 style="color: green;">{{MESSAGE}}</h4>{{/SUCCESS}}
		{{#ERROR}}<h4 style="color: red;">{{MESSAGE}}</h4>{{/ERROR}}
	    </div>
	</div>
    </body>
</html>
