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
	      <h3>Updating row {{ROW}} in table {{TABLENAME}}</h3>
		  <pre>{{DEBUG_QUERY}}</pre>
	      <form action="/update/{{TABLENAME}}/{{ROW}}" method="POST">
		  	<table border="0" width="100%">
		  	{{FORMDATA}}	
			</table>
			<input type="hidden" name="dataSent" value="true"/>
			<input type="hidden" name="tableFields" value="{{TABLEFIELDS}}"/>
			<input type="submit"/>
			<input type="reset"/>
		  </form>
	  </div>
      </div>
  </body>
</html>
