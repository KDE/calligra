PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE kexi__db (db_property Text(32), db_value CLOB);
INSERT INTO "kexi__db" VALUES('kexidb_major_ver','1');
INSERT INTO "kexi__db" VALUES('kexidb_minor_ver','2');
INSERT INTO "kexi__db" VALUES('kexiproject_major_ver','1');
INSERT INTO "kexi__db" VALUES('kexiproject_minor_ver','0');
CREATE TABLE kexi__fields (t_id UNSIGNED Integer, f_type UNSIGNED Byte, f_name Text(200), f_length Integer, f_precision Integer, f_constraints Integer, f_options Integer, f_default Text(200), f_order Integer, f_caption Text(200), f_help CLOB);
INSERT INTO "kexi__fields" VALUES(1,3,'id',0,0,119,1,NULL,1,'ID',NULL);
INSERT INTO "kexi__fields" VALUES(1,3,'age',0,0,0,1,NULL,2,'Age',NULL);
INSERT INTO "kexi__fields" VALUES(1,11,'name',200,0,0,0,NULL,3,'Name',NULL);
INSERT INTO "kexi__fields" VALUES(1,11,'surname',200,0,0,0,NULL,4,'Surname',NULL);
INSERT INTO "kexi__fields" VALUES(53,11,'a',200,0,0,200,'1',0,NULL,NULL);
INSERT INTO "kexi__fields" VALUES(53,11,'b',200,0,0,200,'2',1,NULL,NULL);
INSERT INTO "kexi__fields" VALUES(2,3,'id',0,0,119,1,NULL,0,'ID',NULL);
INSERT INTO "kexi__fields" VALUES(2,11,'model',200,0,0,0,NULL,1,'Car model',NULL);
INSERT INTO "kexi__fields" VALUES(103,4,'id',0,0,119,1,NULL,0,'Id',NULL);
INSERT INTO "kexi__fields" VALUES(103,3,'owner',0,0,0,0,NULL,1,'Owner',NULL);
INSERT INTO "kexi__fields" VALUES(103,3,'car',0,0,0,0,NULL,2,'Car',NULL);
INSERT INTO "kexi__fields" VALUES(103,3,'since',0,0,0,0,NULL,3,'Since',NULL);
CREATE TABLE kexi__objectdata (o_id UNSIGNED Integer NOT NULL, o_data CLOB, o_sub_id Text(200));
INSERT INTO "kexi__objectdata" VALUES(4,'<!DOCTYPE UI>
<UI version="3.1" stdsetdef="1">
<kfd:customHeader version="2"/>
<pixmapinproject/>
<class>QWidget</class>
<widget class="QWidget">
<property name="name">
<string>formularz1</string>
</property>
<property name="geometry">
<rect>
<x>0</x>
<y>0</y>
<width>500</width>
<height>230</height>
</rect>
</property>
<property name="paletteBackgroundColor">
<color>
<red>97</red>
<green>147</green>
<blue>207</blue>
</color>
</property>
<property name="dataSource">
<string>persons</string>
</property>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel1</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>40</y>
<width>152</width>
<height>36</height>
</rect>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="shadowEnabled">
<bool>true</bool>
</property>
<property name="font">
<font>
<family>Verdana</family>
<pointsize>14</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>0</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="text">
<string>Name</string>
</property>
<property name="dataSource">
<string>name</string>
</property>
</widget>
<widget class="Line">
<property name="name">
<string>Line1</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>80</y>
<width>450</width>
<height>11</height>
</rect>
</property>
<property name="lineWidth">
<number>3</number>
</property>
<property name="frameShadow">
<enum>Plain</enum>
</property>
<property name="orientation">
<enum>Horizontal</enum>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel2</string>
</property>
<property name="geometry">
<rect>
<x>180</x>
<y>40</y>
<width>182</width>
<height>36</height>
</rect>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="shadowEnabled">
<bool>true</bool>
</property>
<property name="font">
<font>
<family>Verdana</family>
<pointsize>14</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>0</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="text">
<string>Surname</string>
</property>
<property name="dataSource">
<string>surname</string>
</property>
</widget>
<widget class="KexiDBImageBox">
<property name="name">
<string>obrazek2</string>
</property>
<property name="geometry">
<rect>
<x>380</x>
<y>10</y>
<width>90</width>
<height>70</height>
</rect>
</property>
<property name="alignment">
<set>AlignRight|AlignTop</set>
</property>
<property name="storedPixmapId">
<number>6</number>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>textBox</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>170</y>
<width>70</width>
<height>30</height>
</rect>
</property>
<property name="alignment">
<set>AlignRight|AlignVCenter</set>
</property>
<property name="dataSource">
<string>age</string>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>textBox2</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>90</y>
<width>340</width>
<height>30</height>
</rect>
</property>
<property name="dataSource">
<string>name</string>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>textBox3</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>130</y>
<width>340</width>
<height>30</height>
</rect>
</property>
<property name="dataSource">
<string>surname</string>
</property>
</widget>
</widget>
<includehints>
<includehint>klineedit.h</includehint>
</includehints>
<layoutDefaults spacing="6" margin="11"/>
<tabstops>
<tabstop>TextLabel1</tabstop>
<tabstop>TextLabel2</tabstop>
<tabstop>obrazek2</tabstop>
<tabstop>textBox</tabstop>
<tabstop>textBox2</tabstop>
<tabstop>textBox3</tabstop>
</tabstops>
</UI>
',NULL);
INSERT INTO "kexi__objectdata" VALUES(65,'<!DOCTYPE UI>
<UI version="3.1" stdsetdef="1">
<kfd:customHeader version="2"/>
<pixmapinproject/>
<class>QWidget</class>
<widget class="QWidget">
<property name="name">
<string>form1</string>
</property>
<property name="geometry">
<rect>
<x>0</x>
<y>0</y>
<width>480</width>
<height>220</height>
</rect>
</property>
<property name="paletteBackgroundColor">
<color>
<red>203</red>
<green>238</green>
<blue>185</blue>
</color>
</property>
<property name="focusPolicy">
<enum>NoFocus</enum>
</property>
<property name="dataSource">
<string>cars</string>
</property>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel1</string>
</property>
<property name="geometry">
<rect>
<x>30</x>
<y>20</y>
<width>266</width>
<height>54</height>
</rect>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="shadowEnabled">
<bool>true</bool>
</property>
<property name="font">
<font>
<family>Verdana</family>
<pointsize>20</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>0</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="text">
<string>Cars</string>
</property>
<property name="dataSource">
<string>model</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel2</string>
</property>
<property name="geometry">
<rect>
<x>30</x>
<y>100</y>
<width>76</width>
<height>30</height>
</rect>
</property>
<property name="text">
<string>ID:</string>
</property>
</widget>
<widget class="KexiPushButton">
<property name="name">
<string>PushButton1</string>
</property>
<property name="geometry">
<rect>
<x>310</x>
<y>100</y>
<width>130</width>
<height>40</height>
</rect>
</property>
<property name="onClickAction">
<string>form:cars</string>
</property>
<property name="onClickActionOption">
<string>close</string>
</property>
<property name="text">
<string>Close Window</string>
</property>
</widget>
<widget class="KexiPushButton">
<property name="name">
<string>PushButton2</string>
</property>
<property name="geometry">
<rect>
<x>310</x>
<y>150</y>
<width>133</width>
<height>37</height>
</rect>
</property>
<property name="focusPolicy">
<enum>NoFocus</enum>
</property>
<property name="onClickAction">
<string>currentForm:data_save_row</string>
</property>
<property name="text">
<string>Save Changes</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel4</string>
</property>
<property name="geometry">
<rect>
<x>30</x>
<y>140</y>
<width>76</width>
<height>34</height>
</rect>
</property>
<property name="text">
<string>Model:</string>
</property>
</widget>
<widget class="KexiDBImageBox">
<property name="name">
<string>obrazek2</string>
</property>
<property name="geometry">
<rect>
<x>330</x>
<y>20</y>
<width>110</width>
<height>70</height>
</rect>
</property>
<property name="alignment">
<set>AlignRight|AlignTop</set>
</property>
<property name="storedPixmapId">
<number>8</number>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>idEdit</string>
</property>
<property name="geometry">
<rect>
<x>110</x>
<y>140</y>
<width>180</width>
<height>31</height>
</rect>
</property>
<property name="focusPolicy">
<enum>WheelFocus</enum>
</property>
<property name="text">
<string>id</string>
</property>
<property name="dataSource">
<string>id</string>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>modelEdit</string>
</property>
<property name="geometry">
<rect>
<x>110</x>
<y>100</y>
<width>180</width>
<height>31</height>
</rect>
</property>
<property name="focusPolicy">
<enum>WheelFocus</enum>
</property>
<property name="dataSource">
<string>model</string>
</property>
</widget>
</widget>
<includehints>
<includehint>kpushbutton.h</includehint>
<includehint>klineedit.h</includehint>
</includehints>
<layoutDefaults spacing="6" margin="11"/>
<tabstops>
<tabstop>idEdit</tabstop>
<tabstop>modelEdit</tabstop>
<tabstop>TextLabel1</tabstop>
<tabstop>TextLabel2</tabstop>
<tabstop>PushButton1</tabstop>
<tabstop>PushButton2</tabstop>
<tabstop>TextLabel4</tabstop>
<tabstop>obrazek2</tabstop>
</tabstops>
</UI>
',NULL);
INSERT INTO "kexi__objectdata" VALUES(101,'<!DOCTYPE script>
<script language="python" ># This is Technology Preview (BETA) version of scripting
# support in Kexi. The scripting API may change in details
# in the next Kexi version.
# For more information and documentation see
# http://www.kexi-project.org/scripting/

# Saves the contents of &quot;persons&quot; table, without the &quot;id&quot; column
# to a &quot;SimpleReport54321.txt&quot; text file in the current user''s home
# directory. Totals about average, minimum, maximum age are appended.

import krosskexidb, krosskexiapp, os, codecs

keximainwindow = krosskexiapp.get(&quot;KexiAppMainWindow&quot;)
connection = keximainwindow.getConnection()
table = connection.tableSchema(&quot;persons&quot;)
if not table:
	raise(&quot;No table ''persons''&quot;)
query = table.query()

cursor = connection.executeQuerySchema(query)
if not cursor:
	raise(&quot;Query failed&quot;)

sum = 0
max = 0
min = 1000
count = 0

# Walk through all items in the table.
filename = os.path.expanduser(&quot;~&quot;)+os.sep+&quot;SimpleReport54321.txt&quot;
f = codecs.open(filename, &quot;wt&quot;, &quot;utf-8&quot;)
if not f:
	raise(&quot;Opening file failed&quot;)
f.write(&quot;%s\t%s\t%s\n-------------------------\n&quot; 
	% (query.fieldlist().field(1).caption(),
	query.fieldlist().field(2).caption(),
	query.fieldlist().field(3).caption()))
while cursor.moveNext():
	count += 1
	sum += cursor.value(1)
	if max &lt; cursor.value(1):
		max = cursor.value(1)
	if min > cursor.value(1):
		min = cursor.value(1)
	f.write( &quot;%s\t%s\t%s\n&quot; % (cursor.value(1), cursor.value(2),cursor.value(3)))

f.write( &quot;\nAverage age:\t%.2f\n&quot; % (sum / count) )
f.write( &quot;Minimum age:\t%.2f\n&quot; % min)
f.write( &quot;Maximum age:\t%.2f\n&quot; % max)
f.close()
</script>
',NULL);
INSERT INTO "kexi__objectdata" VALUES(104,'SELECT persons.name, persons.surname, persons.age, cars.model, ownership.since FROM persons, ownership, cars WHERE cars.id = ownership.car AND persons.id = ownership.owner','sql');
INSERT INTO "kexi__objectdata" VALUES(104,'<query_layout><table name="persons" x="380" y="54" width="110" height="132"/><table name="ownership" x="180" y="49" width="110" height="132"/><table name="cars" x="9" y="57" width="110" height="92"/><conn mtable="cars" mfield="id" dtable="ownership" dfield="car"/><conn mtable="persons" mfield="id" dtable="ownership" dfield="owner"/></query_layout>','query_layout');
INSERT INTO "kexi__objectdata" VALUES(103,'<!DOCTYPE EXTENDED_TABLE_SCHEMA>
<EXTENDED_TABLE_SCHEMA version="1" >
 <field name="owner" >
  <lookup-column>
   <row-source>
    <type>table</type>
    <name>persons</name>
   </row-source>
   <bound-column>
    <number>0</number>
   </bound-column>
   <visible-column>
    <number>3</number>
   </visible-column>
  </lookup-column>
 </field>
 <field name="car" >
  <lookup-column>
   <row-source>
    <type>table</type>
    <name>cars</name>
   </row-source>
   <bound-column>
    <number>0</number>
   </bound-column>
   <visible-column>
    <number>1</number>
   </visible-column>
  </lookup-column>
 </field>
</EXTENDED_TABLE_SCHEMA>
','extended_schema');
INSERT INTO "kexi__objectdata" VALUES(105,'<!DOCTYPE UI>
<UI version="3.1" stdsetdef="1">
<kfd:customHeader version="2"/>
<pixmapinproject/>
<class>QWidget</class>
<widget class="QWidget">
<property name="name">
<string>form1</string>
</property>
<property name="geometry">
<rect>
<x>0</x>
<y>0</y>
<width>514</width>
<height>611</height>
</rect>
</property>
<property name="paletteBackgroundColor">
<color>
<red>154</red>
<green>168</green>
<blue>198</blue>
</color>
</property>
<property name="dataSource">
<string>ownership</string>
</property>
<widget class="KexiDBComboBox">
<property name="name">
<string>comboBox</string>
</property>
<property name="geometry">
<rect>
<x>100</x>
<y>120</y>
<width>121</width>
<height>26</height>
</rect>
</property>
<property name="dataSource">
<string>owner</string>
</property>
</widget>
<widget class="KexiDBComboBox">
<property name="name">
<string>comboBox2</string>
</property>
<property name="geometry">
<rect>
<x>100</x>
<y>160</y>
<width>121</width>
<height>26</height>
</rect>
</property>
<property name="dataSource">
<string>car</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>label</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>120</y>
<width>55</width>
<height>29</height>
</rect>
</property>
<property name="text">
<string>Owner:</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>label2</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>160</y>
<width>69</width>
<height>29</height>
</rect>
</property>
<property name="text">
<string>Car:</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>label3</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>80</y>
<width>59</width>
<height>29</height>
</rect>
</property>
<property name="text">
<string>Since:</string>
</property>
</widget>
<widget class="KexiDBLineEdit">
<property name="name">
<string>textBox</string>
</property>
<property name="geometry">
<rect>
<x>100</x>
<y>80</y>
<width>172</width>
<height>26</height>
</rect>
</property>
<property name="dataSource">
<string>since</string>
</property>
</widget>
<widget class="KexiDBLabel">
<property name="name">
<string>TextLabel1</string>
</property>
<property name="geometry">
<rect>
<x>20</x>
<y>20</y>
<width>306</width>
<height>54</height>
</rect>
</property>
<property name="alignment">
<set>AlignLeft|AlignTop</set>
</property>
<property name="shadowEnabled">
<bool>true</bool>
</property>
<property name="font">
<font>
<family>Verdana</family>
<pointsize>20</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>0</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="text">
<string>Ownership</string>
</property>
<property name="dataSource">
<string></string>
</property>
</widget>
<widget class="KexiDBImageBox">
<property name="name">
<string>image</string>
</property>
<property name="geometry">
<rect>
<x>350</x>
<y>20</y>
<width>145</width>
<height>119</height>
</rect>
</property>
<property name="alignment">
<set>AlignRight|AlignTop</set>
</property>
<property name="keepAspectRatio">
<bool>true</bool>
</property>
<property name="storedPixmapId">
<number>7</number>
</property>
</widget>
</widget>
<includehints>
<includehint>kcombobox.h</includehint>
<includehint>klineedit.h</includehint>
</includehints>
<layoutDefaults spacing="6" margin="11"/>
<tabstops>
<tabstop>comboBox</tabstop>
<tabstop>comboBox2</tabstop>
<tabstop>label</tabstop>
<tabstop>label2</tabstop>
<tabstop>label3</tabstop>
<tabstop>textBox</tabstop>
<tabstop>TextLabel1</tabstop>
<tabstop>image</tabstop>
</tabstops>
</UI>
',NULL);
INSERT INTO "kexi__objectdata" VALUES(106,'<!DOCTYPE macros>
<macro xmlversion="1" >
  <item action="open" >
    <variable name="name" >cars</variable>
    <variable name="object" >form</variable>
  </item>
  <item action="navigate" >
    <variable name="record" >last</variable>
  </item>
</macro>
',NULL);
INSERT INTO "kexi__objectdata" VALUES(107,'SELECT name, surname, age FROM persons WHERE persons.age > [Enter minimum person''s age] ORDER BY age','sql');
INSERT INTO "kexi__objectdata" VALUES(107,NULL,'query_layout');
INSERT INTO "kexi__objectdata" VALUES(108,'<!DOCTYPE kexireport>
<kexireport>
 <report:content xmlns:report="http://kexi-project.org/report/2.0" xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0">
  <report:title>Report</report:title>
  <report:script report:script-interpreter="javascript"></report:script>
  <report:grid report:grid-divisions="4" report:grid-snap="1" report:page-unit="cm" report:grid-visible="1"/>
  <report:page-style report:print-orientation="portrait" fo:margin-bottom="1.00cm" fo:margin-top="1.00cm" fo:margin-left="1.00cm" fo:margin-right="1.00cm" report:page-size="A4">predefined</report:page-style>
  <report:body>
   <report:section svg:height="2.75cm" fo:background-color="#ffffff" report:section-type="header-page-first">
    <report:label report:name="report:label16" report:horizontal-align="left" svg:x="11cm" svg:width="4.5cm" svg:y="1.75cm" report:caption="Model" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
     <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
    <report:label report:name="report:label15" report:horizontal-align="left" svg:x="15.75cm" svg:width="2.7222cm" svg:y="1.75cm" report:caption="Since" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
     <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
    <report:label report:name="report:label14" report:horizontal-align="left" svg:x="9.5cm" svg:width="1.25cm" svg:y="1.75cm" report:caption="Age" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
     <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
    <report:label report:name="report:label13" report:horizontal-align="left" svg:x="4.75cm" svg:width="4.5cm" svg:y="1.75cm" report:caption="Surname" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
     <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
    <report:label report:name="report:label12" report:horizontal-align="left" svg:x="0.25cm" svg:width="4.25cm" svg:y="1.75cm" report:caption="Name" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
     <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
    <report:label report:name="report:label11" report:horizontal-align="left" svg:x="0.25cm" svg:width="12.25cm" svg:y="0cm" report:caption="Persons &amp; Cars" report:vertical-align="center" svg:height="1.5cm" report:z-index="0">
     <report:text-style fo:font-weight="100" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="26" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:font-style="italic" fo:background-color="#ffffff" fo:background-opacity="100%"/>
     <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
    </report:label>
   </report:section>
   <report:detail>
    <report:section svg:height="0.50cm" fo:background-color="#ffffff" report:section-type="detail">
     <report:field report:item-data-source="age" report:name="field3" report:word-wrap="0" report:can-grow="0" report:horizontal-align="left" svg:x="9.5cm" svg:width="1.25cm" svg:y="0cm" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
      <report:text-style fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
      <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
     </report:field>
     <report:field report:item-data-source="surname" report:name="field2" report:word-wrap="0" report:can-grow="0" report:horizontal-align="left" svg:x="4.75cm" svg:width="4.5cm" svg:y="0cm" report:vertical-align="center" svg:height="0.5cm" report:z-index="0">
      <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
      <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
     </report:field>
     <report:field report:item-data-source="name" report:name="field1" report:word-wrap="0" report:can-grow="0" report:horizontal-align="left" svg:x="0.25cm" svg:width="4.25cm" svg:y="0cm" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
      <report:text-style fo:font-weight="bold" fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
      <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
     </report:field>
     <report:field report:item-data-source="model" report:name="field5" report:word-wrap="0" report:can-grow="0" report:horizontal-align="left" svg:x="11cm" svg:width="4.5cm" svg:y="0cm" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
      <report:text-style fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
      <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
     </report:field>
     <report:field report:item-data-source="since" report:name="field6" report:word-wrap="0" report:can-grow="0" report:horizontal-align="left" svg:x="15.75cm" svg:width="2.7608cm" svg:y="0cm" report:vertical-align="center" svg:height="0.4665cm" report:z-index="0">
      <report:text-style fo:letter-spacing="0%" style:letter-kerning="true" fo:font-size="10" fo:foreground-color="#000000" fo:font-family="DejaVu Sans" fo:background-color="#ffffff" fo:background-opacity="100%"/>
      <report:line-style report:line-style="nopen" report:line-weight="1" report:line-color="#000000"/>
     </report:field>
    </report:section>
   </report:detail>
  </report:body>
 </report:content>
 <connection type="internal" source="persons_and_cars"/>
</kexireport>
','layout');
CREATE TABLE kexi__objects (o_id INTEGER PRIMARY KEY, o_type UNSIGNED Byte, o_name Text(200), o_caption Text(200), o_desc CLOB);
INSERT INTO "kexi__objects" VALUES(1,1,'persons','Persons in our company',NULL);
INSERT INTO "kexi__objects" VALUES(2,1,'cars','Cars',NULL);
INSERT INTO "kexi__objects" VALUES(4,3,'persons','Persons in our company',NULL);
INSERT INTO "kexi__objects" VALUES(65,3,'cars','Cars',NULL);
INSERT INTO "kexi__objects" VALUES(101,5,'write_simple_text_report','Write simple text report',NULL);
INSERT INTO "kexi__objects" VALUES(103,1,'ownership','Ownership',NULL);
INSERT INTO "kexi__objects" VALUES(104,2,'persons_and_cars','Persons and cars',NULL);
INSERT INTO "kexi__objects" VALUES(105,3,'ownership','Ownership',NULL);
INSERT INTO "kexi__objects" VALUES(106,7,'open_cars_and_go_to_last','Open cars and go to the last row',NULL);
INSERT INTO "kexi__objects" VALUES(107,2,'persons_by_age','Persons by age',NULL);
INSERT INTO "kexi__objects" VALUES(108,4,'persons_and_cars','Persons and cars',NULL);
CREATE TABLE kexi__parts (p_id INTEGER PRIMARY KEY, p_name Text(200), p_mime Text(200), p_url Text(200));
INSERT INTO "kexi__parts" VALUES(1,'Tables','kexi/table','org.kexi-project.table');
INSERT INTO "kexi__parts" VALUES(2,'Queries','kexi/query','org.kexi-project.query');
INSERT INTO "kexi__parts" VALUES(3,'Formularze','kexi/form','org.kexi-project.form');
INSERT INTO "kexi__parts" VALUES(4,'Reports','kexi/report','org.kexi-project.report');
INSERT INTO "kexi__parts" VALUES(5,'Scripts','kexi/script','org.kexi-project.script');
INSERT INTO "kexi__parts" VALUES(7,'Macros','kexi/macro','org.kexi-project.macro');
CREATE TABLE kexi__querydata (q_id UNSIGNED Integer, q_sql CLOB, q_valid Boolean);
CREATE TABLE kexi__queryfields (q_id UNSIGNED Integer, f_order Integer, f_id Integer, f_tab_asterisk UNSIGNED Integer, f_alltab_asterisk Boolean);
CREATE TABLE kexi__querytables (q_id UNSIGNED Integer, t_id UNSIGNED Integer, t_order UNSIGNED Integer);
CREATE TABLE persons (id INTEGER PRIMARY KEY, age UNSIGNED Integer, name Text(200), surname Text(200));
INSERT INTO "persons" VALUES(1,32,'Georege','Foster');
INSERT INTO "persons" VALUES(2,62,'Joan','Shelley');
INSERT INTO "persons" VALUES(3,51,'William','Gates ™');
INSERT INTO "persons" VALUES(4,45,'John','Smith');
INSERT INTO "persons" VALUES(10,58,'Александр','Пушкин');
INSERT INTO "persons" VALUES(12,79,'Μιχαήλ','Στεφανόπουλος');
CREATE TABLE kexi__blobs (o_id INTEGER PRIMARY KEY, o_data BLOB, o_name Text(200), o_caption Text(200), o_mime Text(200) NOT NULL, o_folder_id Integer UNSIGNED);
INSERT INTO "kexi__blobs" VALUES(1,X'89504E470D0A1A0A0000000D4948445200000040000000400806000000AA6971DE0000000467414D410000AFC837058AE90000142349444154789CD59B79745CD57DC73FF7BD37FB48335A2DCBB2E47DB7B18D6D9684B0248436EC04DA104228344B2124E4A42181344DD2B43D49D3939ED393363D25699AD07242A0610D3B0683C166B321DE6D61C992656BD748B3BC99B7DEFEF1DE482369644B6008F99D73A5D168E6CEEFF7BDDFDF727FF70E7C10E49ACE395CDB75165CA3BEDF1F2DDEEF0F1C2797EDAF209EF85C5583F6351908360DB7F5FC2BF72FBB13C8FF41F57ACFE5DCEF6A5CDBF5A9D02D7DBBAEFE55413E724CCA1F1E93B2EE1F32924B77FD9AC4B9C9F74B95F79D727CB6F7E3346EBCEB9C0D89AF7FF3D3C959977F442312833931A8591EA42757B16AD03A731D849F67E895CC7BADCEFBE702D71D5B4F2C70C78A7981AB3FF5F1A4D8B81250C1B241FA8A0455D89E81FB7E5360FFEF0E6FE3D8F337B2EBCB87DE4BB5DE7B006EE89E4748BD7DEE6CEDC6CBCEAF8A9CBD160211306D9072B2329A0AAD06DCF75B83BDBF6DDBE71E7DE57A76DFB4F3BD52EFBD03E0FA911A22C6ADD5B5815B3E7E76B2FE431B2156094619C3278AAA408F84C71E36D8F9EB235D76D79BD7F3FB6BB7BC176A9E7A00AED916A176E9F5D10AE5AF3FBC3EB1E4C39B04893A283820DDE94FA32AD007BCBAD9E08D5F1C19D28FECFB3C7BAF7AE054AB7B0A01F8AEC22DB75E1A8A697FB366497CE339676954378221C175DED98C9A80B4023B5FB178E3A76DF95CFBE1DBD877F1CF4E9DCEA70A809B3BAA948AC47FCD69085EB96A5384F50B4117E0BC43C34B4501F21AB4EFB679ED27879DE1D623DF66DF9FFCF0DDCFECC9A949836B6F59BBA439FEC3CBAF8A2BDB02D06D422560BB60BCCB517041DA10A857A85A92540A9DDAC732E6656186EEDE0ABC6B884F0D00F3AF9BDFD05C7BDDAE55616D7D183AD3B033078E0B48705DB05C305D301C3067308AEF9336384905774925A1FED087470A97CEC2D5B752D867BC1BD5B55302803484702569171E52E0925A08F7C1AB83100E4052856A0DE22A8415080A10BEF395F341E9FFB0A4C7809C0BC3360CDAA08755E6DFD8C82235F085B7B77E2647EAFE6F02D63B55FDD4006003AE44051C090F031FAD83B380EDC3D0A3400F9ED1017F04050495F20098D25B75CB07C19578A848581A83458B15DACFAF446CD137496802DADFA9EAA70600189FDC256C16F0A17A384FC2964140783698FEC84D7B5E6FCC8BC2790DB0AE0E1E57A0DB71114E2128BD7023FC57CE584E210326A820E16560CD2CF8A880177ABC974D3BEFF8863786E1CC0658DD006A10065D68B340481033A82BA69253C4001BE996590009BB242CACF741E8F27CFA8420F886D784E1ECD9B0B8014418522E2836645DE8B2212E25722695D514720A5D00A44359321E7640AF867305BC7204462C2683200117AAC3B06E362C68041981011784098A800070C084BC0B150E27AFA9A721A706000B2F52D98CAEE044E9060A95B0711EEC6A83BE025E9503E04222042B664353232831E893200C0F27050F0017D89BC74BDE6E313ABE3B39B541B058964CA157CA816D51D8B4102287A1230BD1002C9D05CD4D6046A11FDF70E1192EFC11003A6C48994010EFB33E5031C0F10198820145D16D7851830B17C3653A1C8C40360E9D1230BCC5157800148D5785173B0E15FCB915FCCFFAA030C00CBAB84C0B0000D7869D1AACAA870379D04A0D2F198ACF0217E830BDDA00FCCF71E50787010BC4A184EA2E51B1F1349EC6C25812DED461D8828818BFEAA3D417604B18B2FDEC5114C50FB87FE82C20CFA68204373F1BF98FDBBE252E0A38D3640078795CBA9035C0564A0C2F66073FA6E6DD32D339FC616380BC06D5CCF36927C2EDEAB296D595F3AEC46E0F7B85C9C482682A5101075C0BF2627C563C29860A5ECDED6581E97E6259993100D6C55CE43ADC115CDC701E675D081B3F49A3BB0C6B8F456CAF4E7E7594BC39BDB964316E88192EA6031812B121BB4C7C71DE77E4D6CC6DFCFBE0F199DA023300207B11EBC351BE251AAAAE52CE3C47B0EE32A85B03224013697E7EB5CEE7EE31091AF5F4AE89932DBAC31422A56F88CDCCDB320A485B125BE85444CE8A5E3D100EB73851AEE79F070FCE70A6930390BF98059AC6EDA23A7A837AFAE911D67D02E66C00350A863E1A88CE6891DCFF992C9FFD1F93A03E9BFE4D0906EC13CF2D7D5F96330140F5F60066AF45405188348FD02C6A3776BA358F3A7FC7757C77F0F519CC3635F6E94BA80D2B7C494D046F5656AD98C5BA8F41F3260824BC5E9774C636EE45A7D55C8E0F06B8F6574D74D7CEA1707E0D474DCA3221A9C169517861E4445A9488026A00021D79E403C7A8E9E9E76BDF53A8B97837DFE3FB886309FA5E71BAF483BD37F13743CFBC6300E49944AC3AAE57E2EA37D4250B16B2F623D0B209C2B55E6BA76878B10891723C108ACB4856E3867BE7B3479B8FB8A88EB79DC94E9ED4604D1C5E4C9D040001220091010BE5896E22FB8F73D1E539AEB819EAEB0482206FF3363FE01FC9764B72DBC3A991D6DEBFE28E81FB6604C07DD7A05E99E31211E7DBEABCB91B587B06CCDB08D1599EF2AE3366A42CF93DF139BC7DAA9E57B9F5E1C56C359613FA443D7B853AAE83370AC010637B82899A0520967551B7F41178E3281BCE1EE2D25B0BCC5F244777E000010274D1C54FF937527D79B22F878C546BDF57F8E6C05DD306C0B892BF0FCE9FF36D56AE869675106FF434739DF1068F33BCF4799F11EE1808AE2DF8EA63CB782AB396C4A58DBC2134CFEF8164C00760B00C0001885A92F0EB43385BBA58BABC9B3FBD35C5D20D96577016638BAFBDF441181443FC4CDE457A4027BB2D28070FF7DEC9ED83FF342D00B297F17FB19B6FFC24C90630FC268BEB8E1927271A4C090872743BEB3DE7BF4F78407D67F32AFEF7F899345D3D975702412C139241585D015B074A0050210244F6A491CF77515FD5C9855FEA61C5793A8A2AB12D513EB1F85668A8E832CFDDEEAF184A8F60BE1AA5B7B5F7C77C7DE04EA6E81B8E7685BFB1943F0BAF5AB4027BC83B91181E06DBF118601963C32C3E2E788F4B875500B3E479C304D3E0FCA60ED4EC08BF7BAE9AB58BE2F4453C26CC0E4167C60320A840CD119DC0031DC4BAF6F0919B7673D11D1D542FCEA09B26BA6562B8FE90258F4BFECEBB79A4942C13CB38AE1E47AF1BA2C2AA3E3BB3D66DE62DFD5974265528E3D3A004D2235030BC68DF75000843AC06B410081F2FD7EF75E3576393D8519219A404D7E6D6459BA919DECF9DFF7D0367FEF94A8ECE8D61B9DE12CC1E30B19FEF414DB5B1E68A839C765537B16A8BACA14076428414131E4B3929880A21B8800BD8127A81CED38ED2A235DED071BB93E0B9D4177892FE130020BD667E3603410B1A6AA1F5101CDF0FC13804A21E106AC8CB49421B7B9F2C8201D82658A6C788820E852CE486B956D549CC3DC657EFBF939557AD409B1B61D65303E43BF7B3FCA3075870452BB39A24A609853493B3C344E34FF03F05C12636E106A07DE91196A80BAE38E4B62548E837F11BE34879001CD73BB0772558398FC2F317415B1BF4F700837EB0131356B88409AE3B361C076C7FBE781C16AFE713AB9BA8E87C832FFE32CD801925BE690FA9BB7E40C5C20692FD6B681FCA12D762A842C1C59DDAE7CBA54E31FEB700568A950855E1C8FC232C162DE7B72B471FB063EE67F985B5671C008EC46F6BB963AD26DB00AB079AE77AE5DAF1A3DE764D326664E9CA8FFEED7A603A2EC4E2D0DC024BD6C29C5510ABE71CB6F3E0854FF09F0F77B0EB2F9A18681AE2E96C07B998CBDC420BDB065FA32E54436DA09A8008A0A0E0FA0155965BFD09C60B84BFB516D8D226E12691A682B640B2CC5DB86ECFB15D5F05EBCB78C78E9E580E9EF19633BED7663B60F4C2EC066FF6F6C3E34198C800C75FF94814E635C1C255D0B80CA2358002D961E8ED666965378B236DECB09AA1100057E5655E677542678DB390C78F6F0301D5812A2AB50AE26A8CB01246555494220526D05EE2E24A898989EEE6483B19B24E0E4D53A8488471933643D161488BD9403DD031C60077820B4C94542FD424C15D08ADFE9EA31484A2E1A1303435C3826550BF70CC703DED05512303A96E1C53C1B0553F660870144065B7B5976C85CE658D1FE1C98E57E8CE77D1AD68201414A110101A01258022FCDC29E42800B6B4B1A485252D9050118A5193ACA43A996075640DA8F088F5045868789DC53117B065110077EA5EDBF020242A61D132D8BFD7DF13C8F18637CD87BA1688547906E7FD6826145055E83D0CF91CD80A8E8B779E60014ED1B143B45B9D64C3052E6C398397DA76D397EF0355E00A8981898139667829FDFDAAB42A52456D7525B16484399139ACD096532B6A79B6B0D9EB5D964496320C704EDC6C1C19F2FC7AD94A3878C06340C31C98D30CD58D104E805441CFF88617870A8E05BDED1EDA9683E34AA494608A12000002F433C033DA8B9C37EF43EC6A8BD091EB04B524668FFA7CB12A1524C395D4D524085629D4466A5912584215491CDBA59F7E0EE55B277592C662C028034E0200C0481A422138EF42C8668100842BC1D520972D31BAE8AB3E00831D904D7BF75F6CE9798F2B3D17B02786768D8C95E569F545CE99B789507B9043A9C31E08A52B0E5404135457C788D50589472A68D15AA822816B4B8618464365C84971BCD043A59318D74B1CCF00DBF582DE74DACDD91C5419A0443DD07259AF8D4BA9D1C5A178AE3078CC83DF9560491C47FA2E500E004F3DC332D8AC6CE3ACE6F584DC15ECEE3FE07D8E84682846635D0DA15A052DA251A7D55345126C4891F63BCB0A9AD0D8973F80659A93DAE96331C0990103C043D130A16041A1E0193969D57DE3CD3CA4FBBCB2BAD8FAB5188D01D252BCCE68D9FCA62071D92676B0B669151BC5691C49759348C6D0EA244ED426AA5690A40A610B46C8A078891021042A2ABA9BA735D70EA8632DF5890038D25346D8F6F45A8CD2674BAE00791D1465B2E1527AFB06233B5640E1FD92165E0C2806C1B20C288A0A48DE32DEE2632D1770CBF26BF8D9D0BD546A09926E358A5448DBBEE1C2371EC5FF5BE160FE1086950745F58260897DA300B82E9A342D845DAC034ED2A691AE57E5E5B2A0E72630C0FFBF6397F85BC97C1318802D3C373859674406783BD7414CC4B04C17E10A3222E79B3BB6EA45F315A130600DD063F479A62AF8E79765624057961D55AFEEBDBC6941030147F71B202750487A911CD304C328317CF4C764C34BC5612C085AD30100900235A061981679C3206D67C7567CD4706FD50582B49D65C0EA67342E150F54CA6581CBDFE297DF1F4EB55C9277FE72D9B23944ECACB782620AA5A45FEADA7EED20989EEBF8A2966C28A5C9495C600C00D702C771D10B79145529A1BC182D8EA494E86E9E825318331EC6CE13CA0541A0E73B47F8D1889D362FD7EDCFAF3DAD45AB70D2DEA5DE7220146380E54CFD9A72220045A1ADCF2693B328601293517266EEE4734805C3B418C967700D9B9C920784977C1020252E2E8E2C9E954C98AF78A85A2E08E285A2B61F77F1A394AD67AECAB77DE5CCD3E7876BB42CE40B93952B05C0724EAEBC9793E8D7054F1FCCB0B94DCFBE259D5D7BF6B5352D3CA3A51955D097EFF762C9940008A4267102124C7015AF08F2BA6C13DDAE8C3E45004A64E2B9800D74FEA2879FF4168C61DD3C7CC7D9EBE755CE8928904EFB91BE1400777A00A882B4A9F0424781275A75F38D82BD673FBC94851DFC4B4FA0F536E74BCB362E5A1792418E668F7B9A96651D485B41DAC24B9B8A52DC1F963778E29B1510528C7B71B98B9212C8B61668EDCEBABDF19191D3137535B1EA880ABAEE7788FDCD4F651286731E438A3BC2D2011852E1E5A30E3FDF99717E7B347FE839DB7DF4303C62C266E03572720F2FE5F60E34E717D736D6CFAD0F5633984B81EB97C7E306C4D518712D464FBA0F5CA5CC6BCABF4F488DA819C0EECF62B5190769E5496068AA9BA212C8755A1C3E9E955DC1C1E1B5F1BADA647D2C80D0F5B13D7F2209A91CE83E008E1C0D328ED078734870F7EE1CF71ECE1D79DA741FDF030FE5E159603BF03690010A38F4F082BE2B353B3737DE9858D81C9F4D7F26E51F802A1E18AE67484C8B8F0120D5B1FF951D8014446588E09089B577D8305ED6DFE4359EC0E0F7C0C8C9AECAEAC71DDA0FEAB485FA526B42D555B58DD5519474C607A00A86B2630C901284C2415DE3DE4305EE39943BFE58D67EF67578300B4F03DB8003C030E38F4A2CA09FEDC6EE9164A65A6B8CAC589C6C61203B826B3B63203810F701E81D3E1100000A511126AA837968D02E6CCBEC779F741E651F8F60B00DE804ACE9DC152E0CBB1CFD7D8E7DE1FE91E54462B39BE7245147867D066441CF83AA71D40DF160A7C33D07B3430FA7EC175F903C340C4F002F017B8001A6BEE06C0383ECB0F66442C361D91058BB72D6623190C9605B964F778805623E00FDE501900A61254CDC56718EA664EED5C136E749FB3176F2083ACF02AF02470003A67F59DA2840F76B05F6460733F36C119CD7D2524B30A4C1708E7E53F0D490C2DD0773D987FB8C579FB179B0171E075E04DE027A99DE7D5E071862B7B327AB8D885C95B97E5DF34A3595C962160C7005B1408C58204A5F6A3200212544A508C0E008D91DFDC7CCA78CA7799987C8F02C63EC1BA1847D33B92D6E49E8DD61B2571DC8CD2A5862496DCB6C76B467B8E740B6F0C0B1FC5B8F1BF2A10EF89D0B5B801DC0317CA467202E9066BFBBB760A4F30355231B4E5FB02A90CB9BE8D91CB16005F14094BED400B82A3882A012A22A1C45C9E5C8ECEB19349ED6B7C867E4830CF1241EFBF602839461DF4CAFCBDB40FF1E873DCA702179686FCF82C7BBB2871ECEB98FEE933C62C173C0EB4007DE971F67501B4E0221433BFBADE15CAA3739B471F5A2E561DBD4304D8B782846FF403FC14094443842188BE1D663E9FCE691EDF271F7218E8FB26F171EFBA63CA89F66F9364982C0E2597041AF77F7793FD08A77CDEF24B702662402A863259FD4AEA8F8DB0DEB37CD165614DD3068EF6F2712721939DA9B2FECCCEE661B5BD179132FD61C01D24C6301DE2900E0155155782C4A3173AA4F570450C5422EE192C0F7CE39FFDCF905C366F7EE37EDC2AEF47E5E925B19E20D3CC3DBF032CCB4BF49F26E0078BF2541031772B1F8163A515E96DBE9E4753C9AB7E2F9F88CD9F7C7040078DF0D580334E37D07A315CFC7A7792D6BB2FCB10120F04ED0C37841F65D7FCBFCFF016C6165351D8EB4D80000000049454E44AE426082','blockdevice.png','blockdevice','image/png',0);
INSERT INTO "kexi__blobs" VALUES(3,X'89504E470D0A1A0A0000000D4948445200000040000000400806000000AA6971DE0000000467414D410000D904DCB2DA020000135949444154789CD59B7B9025557DC73FE774F7EDFB98E7CE30B3B33B3BFB6477D9058145107CA06CB994C19060225652BE922ACBB24CA2FFA42A9A5425B18C1563122B188BC20A5634C45891604011214A11C00701161784957DB13BFB9CF7EBCE7DF5E39CFC714EF7ED7B7756B0E66E343DD5D3EFEEDFEF7B7EBFEFF9FD7EE75CC1C55B04E0FCC57B0AAF1FECD21FF05DF6F99EDEE638E038E04881230582783156FA450D4F2E54DC073E7C576D3F1003EA22CAD622E4C578A7F8BD9BF3EB770F7247D117EF92A9D220A5D97A9E4BCECFE3E6F2B8AE44EA081D2E134495A341C05DF73E1FDD7DEFF729739181B81800381F7E7BE1EA2D97E86F79AE1871A4696DA3B86E82E0402E972797CF93F30B787E012F57C0118AB03A4DA33A796E7E517FE2035F0CBF0E4480BE08B2E274F87DF2A6CBF21B2EDF20BE2B8533AAB5406B81D2A035986373A306B4D6081C341210201CA4EB93EB1AC6EF5EDBED8BE577DDF6FAE8D28A8C1F3D7292808B004227011040FED63DB94FE73DF97652A5CD252DECB1106884BDA6ED7589100E0803821002E9E629F46D4452BF7CF760E52619AB6FBD78861A1D06A193004860E02D977A770A210A68AB3456629D553CB10681D6CA3C2A84114708732C1D100E85DE31745C5FBF6970F9DA2367D57F4E2E76D6123A058000DCB75CEA5E75CD36E7A3911684B1304AEBA6D26414CF5A88D6809008213160242E61F6F3BDA3C4D5994DBB36349C6F3FA39EC0704247964E002000E76F3F90DB77F3D5E26BC3FDAA6B7448B37640B35C835A3D51B85DE9042050060152E5535730FB4238E4BA86508BE3D70EF5F0C833C7D4393A64059D00407E649F77D9EE51F9B074C41A8D00348EA311B2A8A7E795D02AAB745BCB6B61B8009DB67CD3129A6EE17A45A2B0EE94E4E29A07F7AB0781A003B2AF1A000114DF7FA3FBD9822FAE4DFC3D69DD2D9B2F17575F7F0B9540333335795ECB5BFD536E40E9D4EC110291708205C4F37B6894C7B7D72AF1378E4E314307AC60B5004860E0D66BDCBF914274D1E6EF97BDF9838C5EF10E46378C3170C908A74E1C230802B400682545B2AED0AEBCB50C2757A4BA3821BBFC68F2B197E2A7E80017B8AB7C5EEE19F3D649C4DA300295097AB4D4745DB29352CF20859E014A7D43AC1BDBCC03FFFECF4C9D1D27B64151F6FE5869948E40446811A0A503C205E1185165809B1FA4AB50DE0B7C11A8FDD201E82AA9E130767100854661CC4AB83EBD831B414548295933BC897CB187F77D7880EFDCF72F1C7CFE1994046581D009102AC2B8B7551A172D1C10010817E1F59077D9037401F3AC32545EAD0BE446FAE4B69DEB9DDF6D129A21BAB51BAF64C7D5EFC0B8A902ADF0FC02F9522F5BB6ED402379E5C8CB2DDD641A1F288D161264B33710492F01541626720B95E89BA7E638FBCB06C09D5DD23D375E96FB5012F1986E4D70C5F5BFC9F0E8762B5FD3D11DC7C52F76B361E3468647C678F9A73F218AE2D61E024B96A9D2D2B881ED12CB7367A9D4C5932F9E560759250FACD605542DA25EAEA9E9922F2F518EC6D120356CD9F906505186C464BAEF484977DF30575DF7662E5937CAD7FFE90EA6274EA10CF51957D291C91184E501E998D5770915F89E1EEA80FCD6A6560100505BA8EA639182303611E0F0E84EBA4ABD1087CD55256B042A44A0C8977AD8B869071FFBB3CF71FD5BDF49AC24610451240823683422EA8D887A23A0518F68D443824640180984145D1D90BF23815061C31A6FEB6097BC36F1E3EBDEF66ED6AF1F83AC5FA459814A8F85D638AE4BCEF7D97DD5B55CBEE78DCC4C4D3179EE8CED224DB224B440A188823ACB4BB3444183A945F5C39F9E523F02EAAB11BE13003871A40ADB47BCDB0442680DEF78F7C729BA3168437E2B847F49860468840029047D7D035C77E3DBB9E686BD14BB7B4138848D06E5A579824695B8510555A711A9897B7F147CA11A32CE2A23C2D5FA9006EA87A7D4E199B27A79B05BEEEA2EF53130B209E60F59FF952063EBFF4E8613EC3969CF27C98F920CAF5DC72DBFF53E732E5862FAF063DC79E71D6955E9F9F1E8C1E92AA780C62AE55F3D89605A60FAD844F4706F31B76B78F452D0B159556C14D489E22B00A125089509812D61266B58C6753D8250E068982BC707EFDF1FDD074C02E16A855F3589600A980B3F3A1A7E6FB9AA678BA55E8802438F71B0C21A36B7093126D754DB39154063917313D384114CCEAB135F7DB2FE79E008B0480772814E00A0814A23E6E4CB93D17D7D5DDD467001C40D03C64A4028BB8D32BD440B3091390EAB1C3E32CED9B2F7ECDD4FD43EBDDCE06960820ED5043A01001861A61E7F39B85F214F138526B34B156E5865DB95CF1E8719EB0820AAC1C23150217B6F7A2BCF4F16BEAA144F03C7812A1DAA07748203C0187C1938592A75ED4745A3088C82D2109B09662C1FA80C0F48C7F280D3E40715406DCA58815B40B872E1C523E70E0027810A1D2C89750A00306438373BB77812151A85750C718C61784B88B6D68770DA08D2A6BF61158265105647E932313B770C98A3832D9F2C9D0440038DA327675E21AA81F4005B0A161A7404B1B0E1B10D6B556205D2F418519D34B7B1754284C7E989B943C072F362E7964E7140B2444F1E383E4E6883B3D20820C80C0C8052C6D7A3865138AC417D09828A01210D98B0204A9E3F7AF61026E2EBF8B840A70108BFF3D491B341BDDA4029A340692D48D72AA55A8150B1B1886CA89C02A0D224E86B0FFCF800FF4F008880A5277EBCFFFB04CB360C16501C04A7D812FE1A65B380647361C0C943AE9BE9B9CAD9670F9E384B878AA0ED4BA787C6009C7FFBDE81A9DDC3955D3BC6360C4927672A1ED2332417B5056FEDA39302C8F582DFC7FCE252ED4FFFE16B5FDCFFB3D34F6148F0577A682C5922AD099E3E383EA52BE3C3BB37AD1BF6F305C7B0BA04C7B3BE1E9BBBEDD09991C687C20078459E7BE1B9D9CFDCF185EF7EE5C123F72A38C12AB3BE0B2D17038018A82F54F4E2B3476627860A4B1B370F75AD2BE68B1845ED5018A299294A17FC3EF07BA8D52A9C38FC0C0F3FFEF40FFEFE1BAFDC15C3CF8005FBDE8E2F9DEC06934563AAB527E7CACA5970471EAFAAE09A85233F6174FD16FC421769E6E795C0F5417A34EA5526C69FA3B234CDD88EDD8C6C0D0FC7EC3F0CCCD081A4E742CBC500004C3357817327CE2D1CDCB0ED661666A63872F420DD5DFDF40FAEC3F34BC4518D5A638685D9B354971718DEB091B11D6F44484957D1AF62129E8B427EC972B100001B188561B400D0373844576F3F93278FF3CAD1036865F82C972FB066782D9B77EFC2F5F2245DE1406F3ECFFFC134998B098000744D5E32787C2A715F09DD5BE9E9DA4CBD161009075C977A4E706EC9A330B483DAEC51A2A0C1B972616C74D7F503A70F3EB5C84504A2D3536492F7799FBDE39E3DC3A5C27B7A064A1FDCFBB61BD6B47F496B2897EB144B795C071CC7C1910E5114A24C2D507FE14BDF7C48C4F2B94654FDE183F7DDF583175E78219920D1B1EE70B50024CFCBF7BEF78F4AD7BEE9C61B8BF9E22D7E2E775BB15418291472144B79AE7ADD16FA7B4A2D0F8641C8F4C123B8BD3D0C6D1E6DB9A6B566A95C637A6691B9B932274E4D71F6CC7465A9B2FC6810D6FF6BE2DC9947BE7CE7A74E602C6355D6F18B02902AFCB14F7E6670F3E88E1B0ABE7FA3D2FA0DA542F11A3FEFFAC5629E829F434A81941221254858373CC0964DC3388E248E22169F7C8A358D654204B54B77D0B3751300956A83F1535368A5F0FD1C39CFB503EE9AE9D9454E9F9961727A81E9E9B9438D5AF050A55179E4F147BEF2E4FEFDFB9350F917B28ED70240728FFB579FBDE79AA1C1DEDF108E7B8B97CB5D2924E45C8762318F94D2AE2255DEB100180C247E21C7E68DC32C3FFB13D63FF303BC0DA320254B533304B7FF0E35E9313DB3603E28EC609894B8AEC4731D5CCF414A81528A4610333935CFB989594E9D99AECC2D2C3C1406F5470EBCF4CCFD8FDE7FCFFC6B05E3D50010FBF6ED2BFEFA6F7FF40FBB0AA58F7839779356B199D7E74A5CE920A563847424D249402003481380041CBF5266E7F71FC01FDB808E2296AA0D5ED97B2BC291A95422114D348514809012CF73C8792ED2DE1FC7310B0BCB1C3B7196574E9CABCCCCCE7F737A72FCAFEFF9F2E70EF32A01D4CF8B04E5EDB77F70F8D6777DE8B1357D7DEF735CFA82B081B48396D28863AADA429A995D088414F6D8D4FA85B0C7909E8FFD02B9A041D7C9E388A525C6AFDF4B58EA4208F34EB33D7F3163868A288A69042171AC701C737FA1E033B27680CD1B4772C542FECA724DBF7FCBA5AF7BFA85E79E1CE7E758C285001040E10F3EFEA9FFE8E9E9B9C171054BE56524C69F93D614422030AD2C84300ADBF332B31A1068D90E1C3F42F78E6D0821582CF610F4F5A79F5E59FFF375504A1145068424A9743D879E9E12BEE7F94BE5F0E65884F79E193F56BE1008174A87BD3FFFCB3B6F2F95BAF7160B39E6E717D1CA8CFD279CABDAB857A4FF928D68B92645F22963056EA50CAE0B9E47AEBC64EFFAF91E99CD9C9506A534511453AB05C4B1326BA4F05C97AD9BD6B1767860ED9EABF77E02C85DE89D2B012080D2BAF5639FCCFB1EE5E52A61A49AA93CED50AE2CB4485FDFFC849432BDEA2D2D9A5298E7E155CAAFF2B6B62BC617921D8230260C23945276D5388E43775791EE9EFEDF7FD34DEFDCCE05AC7D2500DC3FFE93CFBD335F28ECF03C97C5B2B59EA476814DE21241561053640E8C5B6741301772F3B3E684E3905B9833B78BF45F9304B34A277AD3DC57CAEC348290288A8922451CC5CCCE2D3235BD88EBBAB9DD97DFF07E2E60052B0190EFEBE9BFDA71244A1B7495D2166BDD543E559716E185CDF48494D6FFCD440F292552984F4A09EED222781E380EDEE282E51469794258F02EE4163A33CDD68E39C79A308C514A515EAE71F0D029FC9C872325F9427E0F507C2D0008C0F70BF92D524882204C21D770C1982BF17991CCF012591EC8B881B0E4A9344EA30EF5BAB180B959B24F8896B7B6029D42902AAFD3C42A568AE5E53A2F1D3C4114C50821701D875CAE78A905E0BC065F09805CA150DA220484616890BE60C099689A51DCF6E1494B4A5BF54E822481C02F2F98C72D00EEC25C1A33886CAF91FA9095CCCCAF6B758384943554AB0D0E1E1AA7D1884CCD55291CC7A158288E0E0DADEF7FAD00E40BC5E218421004514B9D32D96F7D40B4FC17649548CC5A368191D2B07E2E074100F53A6E18E04491A913B53C275A2D6285840A3B33AD5CAE72E8C81982466C7B084386C6052557EE79EB0E5620C27600E4CE9D57F4FBB97C9FB0C4A2B3933ADAB14A1A27D35AA915644030F140332CCE95CBE68575EB0652E22D2F810D9D130B485ED60C8CEC3643885AC3CCF412274FCFD8D0B9B54934C6FA4646C776B142FA7F1E00D7DDB077BB90E6C341985880A68571B23088260A690C9F448632131C25798290B8E525F34C06007F6ED65EA7053891809006484DB788A2883367E7989B5FB6BF4449C26D43C0666B7CB0B777601B2BF404ED88B8EB464637258AC4516CBE95718304F96C57959A6A4BE46781B04298D508E94F4F9820A85A4D08026F61CE84D742A00408AD5198A9D76678CDCC1902130095CB35E6172B8461842B2511D91CC46EB5365BA5F0F3851D806FCD286DC67600BCDEBE81312125688DD21A37496844AAAE152AB501D2F83D13CBB784C26986685AC55D9C3783A6F5BA79464A72733348298DC96AD3ED4AFB192584B542A8D7429696AB046164585E4A22A96D786E15962A553C21D762A1B40DC8B70390750101785DA5EEED00511CA7AD98155ECA24DE6F3E95F8BCCC9A7DA278F6BC15C69D9B37114CB50AB51A08813337DB34E14C4E9180DA68842C2DD5A8D6CCB42029256E46A6D6ADB10221A5E984A52097F3FB77EFBE611D6D44D80E80EF170A631288C23855C2DAEE792D2B853419600B3D5B9F4DACA6252DB600CC4E9B5BAB55B36A8D3735D97A9F90680D611053A90684A1196697D2A4DD6EFB7B316C9F286E08151B7A1AC2DEBEFB8A5DAF0640BE902FAC431A0B9036C595584553563EBFF8916687ED00655AC5B1AB776A1CA2C8B47EB50A4AE14E4F993800815226AA0BC39858693BA33C03604B6B93569E921F5A98F02BF92305746878C34EC0BB1000F2A69B6E59E779B922C83492123209669A7EDCB48A0CBB5BE564B6EB92E75B80E3489C99290340A5024B4BB0BC8CFBC28134BCD50978494B3B59E5B3AED82CB909DBE2ADC0D87B6CEB168BDD5B69EB09B224E86CDDBE6B9330EF238EA34C5C9E35FF4CCBDBF3D9FE3A2B9CB48593340E1012D108101313A6E51312540AE7CC696BE2021741E4821B692207241A2935AE84484A8452B6875119802DE10965C531D3EFD298444AFC9CBFD1029012614B1C70F797FEEEC59FBDF4C2A348181CE837A52FD1EC9B9320DFC67B6D7C90B4481B21A5AD6713A3E949F07DB37677434F0F944A88FE3EE4C2BC2538D1F471A7F92E6BCFD6DC699AB875D3A42B6E56A4486392A05E2F3FFC9DAFDF4D5B24D31E1ACAFF7EECA1D3F9BC2FB76CD9BEA9AFAF3717C5315A83E3485CC73126EC3A4847E2380EAE63CD3AB996EEB7AED2A482E48E1FA5FBA92760DB36E8EF3720E4F350ABB1FCE6BD285B191222F3BBA2C435B4466BD5B2AFECBED2CAE62DC9BEB916C7913A71FCE583FFFA95CFFFE3E143077E089C2333DCD646DFF8C008B0BDD4DD7DD9BE7DB7BD7EE3C6AD234A930BC30021AC82D2413A8E9DBA6A8190B24559C711489900D0FC8C77E2387DDFBE17B66E8562D1B07418C299332CECFD35C2D1B1F45E93D0C4C4B1B25B5300554A11C73161AC88558C8A35B132152165AEEB58C59C39736AEAF903FFF3F2E993475F020E63A6D82D922994B6E79902C3923DC0303084F969CA054B4ABFA28BCD60A8607E563305CC6246AD5BF2DAFF055B3FD5E723B21B8E0000000049454E44AE426082','kuser.png','kuser','image/png',0);
INSERT INTO "kexi__blobs" VALUES(4,X'89504E470D0A1A0A0000000D4948445200000080000000800806000000C33E61CB0000000467414D410000AFC837058AE90000001974455874536F6674776172650041646F626520496D616765526561647971C9653C0000380F4944415478DA62FCFFFF3F03232323C328A00B60026266208605F83F244C77008A7B800062198D13FA447CAF1D83A88938433E3323831D28CF7DF9C570DC7323431B50EE0710FF02E23F03E1308000621C2D01689EE3590E84321472B1305402D9FCC0E06680E1B73F1836F86C62C8018A7F04E2EF40FC97DE25004000318DC6116D321628E2770733849D8A64B8C3CDC2D0018A7C0660A4F36A3A30B08B2A80980C42EC0C01B9FA0C3640260F10B322550D74030001345A05503FE299370730188B7032F4B2323258832B7A606CB38B28302866CC67E0D57260F8F3F503C3E944417022D01761F00252E7A125C06F2CA50023B4246182B2FF23B51BFE53EA6080001A4D00D403CCABBC195464F918EAD89818A220452C30D6380518E4E3FA1944EC121081CE2DC0C0074C081FAE1E6010E36430020A7103313B107F434B00CC6BFD8066F232D401A35AFED73F862757DE30CCCCD8C37016DA76F843690312208046130015EAF9E9CE0C22BAA20C859CCC0C59403E1F2CF225831A18C43DF31998B904A04A1F43E3589D814F1B920004D8197480025C40CC068D0F5029C034CB8D41444F14DC76A80099052A4558806580B1188357963E83CFB48B0C5780EABE40D5935D120004D06802A0B04B77388221898795A1131805FCFFA1112F649BC0201554CFC006ACEB21E00D10CF03E29B50BE2CB0043003AB05455DB5098373EB198665D076C0DF43110C41FCEC0C5D400B64FF411B8C3C1A0E0C9FAE1D00D501BC9E0A0C65C00490072D01FE52D2780408A0D10440663DBF3794C119987B7B80DD3A1D683C8223491218F13CC0861E048072FB2620DE8D66C463067E6D6B06983E453E7035B0B1C69241335815D8766062B086E57A6EA09952D1FD0C9C72060C77FB0319DE9FDEC000949703AAE705E2CF94F61E00026834019016F14C2B7D1854550519660323DE06D6C0631356609000463C28E723C06E68E47FC361DC4D067E603BE0FD95030C125C0CFA5B8319AA15F919426111CF04AC36A4A3FA19046D2066FEF9FAECE77FE62FBF401D89BFFFC0A50F27B4C460865603640180001A4D004436F02603EB794B29863E766003EF1FB4C60535F0C4DC0B182402EB9194821AF42BA0C53E3E004C00C076C08FDBC086A02003A8AE50808D0F08BB16308805D483DB0EFF7E7FFEF3FEF2DC976F4EB73EFFFEE08D04B0C8E015E660D086463E0BA55D4780001A4D0004EAF9D9C0C698BE18432107B081070C693E586B4BC4AD80413CB01EA98107AADF3722D5F3F8C1F79BF7804A7731F07343221D94A8B8D41D18A493E733B08A40DA0E5F1EED7DFFE240CE833F1F6E7C676464F8CFF99781F927349140733E33B42D42360008A0D10480A78177229A21898B95A11AD4188335F0780D03C075329B0872030F54D41F25CAE03F6FBF30BCDD7C9EE1D3D1DB0CB0061E0BB00A118FEC079B0D023FDFDDFCF6F270C983AF0FB67C646262F8C70CC4A004C0F58F91E903B00800B9255683416DF10D86279496000001349A00B034F080AD7067010E48030F56CF73C81A304802EB6450A30CD1C0DB03ADEBBF1165F8FB2DE719DEEFB9CAF0F7DB2F702482EA7941970206113F4815022AEEDF9CE97DFAF654E33368C4FF872500903CC72F060E5835C1CF06EE6EB2209502648D070004D06802406AE06D0C645051E067E8656362F086D5F3A0DC29E65FCF206083DCC03B0AADE7898BF86F171F32BC597592E1F79B2FF05CCF6795C020EC5B0F2FEEBF9D9DC0F0F454CB8D3FBFDE7E0646F85F78CE47EAE3733322E6115404185491AA01B24B0180001A4D00A0811C5706510B29867A4E16864C7000834215983B85808D3121D77CB47A7E1E110D3C4471FF66D161866F379FC3238E4DC6804134BC1F5CDF83078CDE5F6478B73981E1F7AB0B0CC048E7646265F8881CF9A0A21FA48EED2364808909C8072622466022E545EA05909D00000268242700F04CDDC5448606606066C31A7820CC0FCC9DA03A1975046F39D10DBC7FDF7F317CDC7A9EE1E3BEABB0061B0323B0C7201AD60FCEF9908A1E587D3F6A61607C7F9DE1DFA7DB608BD9FE33F0013BF4CFC1118F14F9E016DF2FF04821030730617C063606A5B81874D1AA00B20040008DC404009EA93B13C790C4CD0669E0C1EA792E350706C924442B1C52C4AF20BA8107029FF75F0547FE1F503D0F2BEE9D8025894F3DB8CE67F8FB8981E1C502209E0F610333361BBF18C3F7EFF71998FE831321D6DCCCFC93113457001A04FA07349319584280E60FD8284D000001C432C2229EF9580C83333F3B432D30D75BC3EA7950B12C16812896C969E0FDBCFD82E1C3DA130C3F1FBF830FF172A83A3088C6CF07B723201D86B50C0C4F2741723F1C7C02260071866F2FC009809DE92F03CF7F26F0FA0038002DD960FF068E70067666705A65E0656550A446090010402C2324E299B68630A82809808759BD61F53C2B306284812D7078B10C6FE06D22BA9EFFFBEE0B30E24F327C0736F4600D3C6621050691B8F90C1C6AD004F5ED3AB0B86F06C6F549AC66708870C16B17E6BF0CFC7F59183E814A02E40620F30FF08411030F0BE35FD0420E905D91AA0C1ACB6F83BB8264270080001AEE098069960783A895346A030F5414F33B1730083AE7438A6578036F39B4BE27AE9EFF7AE02AC3E7EDE721110F1D19E4712C6010F0868E0C828AF8C76D0C0CAF8139FF3FEE5E1A23CB0F06561E4186DF5FDE83DA0142DFFE333CF9F70F980098C08BB5FE33FF646063FE0F2A20C03382FF60258C003BB861C84A4957102080580622374269585D87BCC081AA0DBCAB290C0DC02213DCC08359C46B99006E8C21221E7DA68E8851BC53B7C1110F6AE5C3EA792EF30406C1907E70220083D78B18189E4D0676053E126122B01A101063F8FDF93DBC1D80DC1660FFCCC00BAA06C05D4116480200614D410643A0D00E4A7A02000144AF04008E90BB790C4E3C6C0C0EA0C90C2066FCFA9BE1D1E9670C8763D733DC65804C68C0A637FF5390C058CE2430040B7030743143A753C18108AC8F458011CF2E6B407603EFF79D170C9F779C67F875FB39BCB8675371601008EE676095819AFBE534B0B8AF82D4F3FF89F5C637060E6129862F8F218990F90F83C87F568657E04400B2E30723274A95016C077CFBC3C0C4C3C220CE805847C0C440C6AC204000D13A0130AF0B6710B15760280016C1D1C03EAC2CF2A24841A0B76480BDD97BB90CEBD6DD60E82AD90DCE863F18B02F8D22D8C03B9D009EA2AD854DA7FE870EE4801A629C6AE437F0FE038BFBCF1B4E82733E7C9C0058CFF37AD683733E24753C0316F735C004700A5896FD432ADC18884ACF6CC014CBC4C2C6F0EFCF2F06D67F0C423FFF33BC86C9B17E01AF196480950AA00400CC3C4C5C9004C04A494310208068950098D783225E91A100E8C82C60C4F3FD83D65BA00064145460F8F7EE01840F1406B68182520D19825C1519A6E8CF62686580AC74F9416469C0BC2B023C8257072CEEA3601104EA770B87F6838B7C721B78E0E4B2F33CC3B7435781590ED2AD63E41060E0B62F60E076C88714F77F3F3330BC5BCAC0F0723AF61C4F7459F61ED8189466F80AEC0D001B828240A35881A5E43FB65F0C2CACBF1838918DE16265F8FBE63B038B201B78A89A15A9042019000410B51300D38608065150C403EB2A94E5514C0A0E0C4C46F10C4C9A01E0400437A49E5F60F8756422C3AF330BC0E124C7CF90F32097C1A2740F43D2EAEBF0F553B8D6BD31CDF36610B59406DAC50A5936058E2060A4F003FBDD7C4EF988FA98C4061E08FCBAF290E1EBC6930C7FDF22866FD9750218F882FAC1B91F0C3E6E06F6E77BA0FD792C114E5245F609980064C00900D8006061F9C320FC8F89E13972FD0F1F126661FCF71F3A29E428C520B7FF19C333682940320008206A2500701DFFB98AA181830569540D1421C0886776AC67605274C0D42429CCC0111AC5C06AADC8F07DD54486DF4FDF31F0B133984CF160386625C31053B89BE118B434F8C58058050BB6EB7A1A4321371B432530B0F861F53CB74502B8050EEF7783733AA89E3F4FB447FE3E7BC7F00D18F1BFEE3C874FD3B2481930F004F683EB7B482BF01C03C3AB5E207D8301DE05C088F0FF24A682BFC004C0CDC0C4CCC6F017540DFC6510FBF19FE105DB57465E7495E0B10068A234106150032680B3E4F604000288D204006E74BDAB6048E2058DAA3122A64D198011CF028C785002406FF04022045114334B01232FDD8BE1DBA6930C3F4FDF66606664E089D165D8004C04D3279C6298B2FC2AC373502DDBE1C8C0EFA9CCE027C8C150099EA285863D68C04500D8026793416EE0615B8A85BF9EFFB1FB3CC38FC357E1F53C03B004E1F3EF67E0308356237F80CE783D0198240F20D5F3E44436EE6A80534291E1CB939B0C4CFF80A5E75F064E8ECFF01E0CBC140086F55F580290E1064F0AB1919B00000288DC9D41E0EEDCFD420613491E865E3666C4FA770671030626CF7E1C114FB8F1054A00A0A2F71F74CA14E4C95F7F189E8298CC4C0C32F04624C8C7C0B68450EC7C700B1F013691D4C003DB79E42AC38F5DE7C17D7B98F91CB6050C5CEEF5E02A85E11FB010FAB88A81E1C30A48710FCEF5FF2018CEFE0F4914B8D8C4359D18FEFED064787E7C1384F78DE19DE04B46AC99F4DCCBFFDC1F7E3230BFFCC67022E920787711A87EFB002D2D894BF440B7010410392500D3CE3806111B39867A6063240BD6EEF9CFAFC0C0E4000C3083042C5A888F14765355608920CCF06503A2186661669046EE3D805BE05EF50CDCE69435F0FEDE7BC1F06DD52160831451CFB3283930F044CC676082D5F35FB60333E63C482BFFFF3F3C39FD3F15DA017F199839BE4106853E034B83AF8C02D02A1073F410341EF083819997954109AD2B48120008201612733DF3AB328660214E866E606E9485CBD837303059E4035D2580A685F448013B4A5A884120DB93E10FB04D004A04FFBE6B0323470EE2715D7F449F1BDEC0237E2916384EDE7F61F8BEFA30C39FBB88695A266069C21D3E9F8145D901D6E907B6EEA700FB22E7A0451B214391E9FF1455033C32EA0CEFAE9F6000A60526681C616C1CE56565FCFBECFF7F5676260631684F80959C04001040C42600A68D510CA2EECA0C73D95918BCE17E96776060F49FCFC020A080A69CF448C1951040180240E90D54D30843CD7D034D602444FC8F5F0C7F8E5E65F8B9E73CA29E07265A4ED77A067660910FE99A808AFB69909CFFFF3FFE98A656D58FDE0E10D566E03ACFC6C0F8EF37C868566C0900D80086F5AC1992D4184CE7DD82F70418497111400011930098DF563004031B5EB3804D057E0668A03100239E513D004B3D4FDAE81AF1E02845E6FE3D779BE1E7969328F53C9B710203876F3FA49E0717F78B19183EAF0526824F0472387210935DDEE3CE6D2C1F817D3F01606B0E3C16C40A34F507239AE1C80D41791E784390E411418000229400983F543224F37330CC848B80221D94EB318A7B42EBE00706FCBBFF82E1F7D613E0EE1DBC0D01ACE7D97D80FD79296855F2F33830E2A7038BFD17689336FF518B74A2E2F73FE53D836F2F1858BFFD65F8893450087225B212562686FFA0EEE0F7DFC0E60A3B830AB909002080F025002694C887E67A068C5C4FFA640A3DC0FF0F5F18FE6C3DC9F0F7FA4344710FACE7398111CFA205F5C3BF970C0C9FFA81EDE68B8888C799A1FF138EE4FFE48E02A281EB6F1918817D612666604BEFEF1F5099CE8A9E0020ED006067099800F8D8C00D4176721A820001843301805AFA7CEC0C5DF0C88FDBCFC0206180A575BF9161B081BFFBCF33FC3D761551DCB30B30B05A1730B03943A769FF7F6560F8BE1C98D33640231E5B4E273107A32716721B83BF817A1E7F07CD113330B3730163F813C81826602AC0680CF2B233FC7DF19581859B19BC38848D9C86204000E14A004CD6B20C05F03A3F7C3D96C89F47A3BA9E02006CE4FD5D7F98E1EFB58788E2DE3081811518F1A0F9074871BF1912F9A0317C92732A5A02818F7A5111DCFD0A65FC616066E366F8F39D11DC5F07DAC50A4C042809408813B23804241DAFC260BAF00E78C08CA486204000E14A00CC2C4C0C76601628E2E5D10775560CBEC807E57CA4C807B99BDDBB1F3104FDF72A30C74F02D22F91BA75FF891BCD23AA9D47858400ACF719EE7D4374BA81F1CDC40ACCE63F7F606D0C223704E5B818D4C86907000410CE0400ECEE59C31B7D185DBCDD832EF2FF1D0716F9A0FA1E147692060CACC9FB21934EA0E2FEE702603DBF8FC0400E5ACBFE3F7A51FE9F8841A0FF484AC928216E01DDFA1BB911FA9B8185830B960040E601FB8688B6001B33C37FD080D0B75F0C4C829086203B52354054020008209C5500EE52F008C3009D6A86BFE8DF0FE9DB330A2830B001231FDE4BF9D100CC48F7918A6FA47969184689706C91CF801499E8FA70241652DB009F80A5FB931FA8EBA418FF116C0C0A7380371A3109B1819789B393DA0E000820D2E790FFBF460DBC4180FF5D7F049ECC010FE5062175517F2D04E68307C883DF588A74B4C8C7DAFDFB8F96BB195073F77F1C098881047F5CFA8C39EE0A2D059839B8E0F1051D1882033E0EC6BFE02DEA4C0C622ABC0C420C88052244018000C299007EFE61B806E94E3D409F4180D4A18308C38A7ED08824BCCEFF0774F7EFADD8732C8A1803163603167D68918A5EC463338FD802E02130E77FC6714C20E32F606390153161F71F3501F0B3038B63A87516220C9A0C889941A266F80002085702F80FAC8A1E83FDF3E000660280CD840D12FCF7C663C878BE26527BE5F736B4DC88946BB115E5C8399DE13FE1DC8D2D4130A0D7F944E47C50D17FF73B6A5B8211AD1460FA0B6C947122AAEDFF88225E9087E117CC59CABCE045A224750701020897A27F6FBF311C874C9C0073D28F0F48529C83ABF87FF0021ECECC5AFE485D82D3B8733C4A44FD47ADDF19D07235D6D202475DFF1F8F7DD8C01FA0FCF56F101ABDE847C9BFBFC0D500928BD9E0AD756062E0E563F80EB24A849D411F346A434A350010403813C0EDB70C87607EFD8F5C0A306A0EAA04F0FFF93B78D58CE8F2016BAF7F5FB1D7D7B8EAFAFFC825040396A29F014711FF1FB3C8FF8FD61BC085EF008BFE2F7F117C462CBD0BB018A831082C05D8D86169831559B508B0A30052CAC7025E23C84E4A020008205C09E0AFEB2286F370FF3DBF88A443735015FFFF5EBC87B4FE9107AA407D7E0652EA7A6CC5393671B49202AB389612021B7EF29B81E1C56F6CAD3E2426234697109E2CFE33C07B70A2DC8C7F605E8B526030872602A2DA010001C4846754E3CFD75F0C27310A3146F9C15505C01200F29434A80188B5B186ABE186DE68C355EFA399F11F5BC980A337808C5F0123FEDE4FB49C8FA51460440E7DD0A010330323133C63B3C318E23C0C7F61A62B71331893D21D040820BCB3811C2C0C9A581BB3A06A009CCB0678C2E7E76F86BF2F3E401A809206C85380F873E67F022D79949201AD9D801EC1FFB1951C58D60CC047FB8025D79D5F840711C183B98C88440136FA17032B1737C3AF2F9F6099173C3FC0F28781950FD80E78FF818153880DDC0EE044EA0DE01D100208205C2984F94D39430D6C4B15A3843E969EC0606800BE82E72B66257BC444CFBF57F8FBE90C045AF2B85AEFD8DA10B8E606FEA3250A100645FEF55F98B1CEF81FC718C07F8C2E2113B01D00EB12028D87970262DC906161503B40998741042901E0050001C48425DDB1BC2D672806F62FCB61730128DD2B5000FFBD372812C09F9B4F2161CC21009EE38717FFFFB1B4EEFF13E8DA61D0D86607B1F5F5B1941AFFB1D4FF6F80917FE337A4C54FCAB4315A2DCEC8F81BD125FC0F8C606897504A90F127CC287B51703B8083986A0020805890AC613A95C6A0AA2DCA3007B4CA17EC0776010696C0F968C3AEF320CBA606BAF8FFF88DE1EFAD67E0B066D5424AA07F4EE16ECCE1EA0DA017DFC4B41D70F62EB0F41EDE01E9877F1193508CC834721B00691E8F11C90EE4E161869F0C2C9C3C0C7F7E7C8309B103A5BF8B7080561231FCF9FB8B8145818BC11628B51E2901E01CBB0708207802F859CBB09F9989C116EE7F60A38A25723D786205517F4D023646F70F8AE1FF9F5BCE32FCFB016945B318C7232580D398DD316C7534B6563DA1BEFE7F227B03C891FF0CD4E8FB87A38E67C01C00FA4F6832F73FB021081918FAFBF33B786410D854F8016A078809317C7FF29C81979F95410FAD1D80F336128000822500E6DFFF183E33434FA162B22C00EFE6818FA9FFB9028CFC39C0A2FFFEC0CFFABDFAC4F073FB7986BF2F3F4272BF7501782937BCFB07AAFF7135C4FE136A08624B280CD88B7E9CBD01280635BD1E02E92FB89688FD276E6A19393130C2ECF8C1C0024C007F800900DC1CF8078C6826869F62C0DE00E8B408D0BC80970483CEB6170CAFA071FC0B579202082078FDF0EA0BC30EB83D0AF6A891FFA90A480F7CBDFFE7D84D861F0B0F30FC83463EA8E5CFEE82B4CA0734ED8B51AFFFC7D3DD63C0BEE60FDF183F038E993F6473416B3A6E3320964732E2A8F41919708C0032A29EA0C088DE16F80B6C0C323130B3B2C19CC406AA48980C53B960CED5E6637064409C278CB31D0010403089FF4A131996FD01960220CD7FCF2F442A237480DA450776B4EFD507869F8B0F32FC3A7A13DEA662736960E0CE3F0F59D10B8A7CD0B42FACFB876B79177ABF1CD7F42EBEE1619CED0BE8E4D44B20F7112362CF20FAF80203B6D149061C2381781A85FFBF33B0707083F99FF854194FD82CE079A291C0C622AE0A3656940DBC9E830B6950082B00082058020055523F5F7F6158033EAEE3DA06D4F17F56F38119E9FBF193E1EFF19B0C3F971C0617FDE0EA11D82BE1CA3B8FC8F9A031FFEF399069DFFF84266970B4F8B10D0A616B24FEC7D56BF80F39B41D14F1EF1971AC0740AB0218518A1B3C0B507195127FC0AB76EEABA7309CB49BCFF0994F052CCA69E005D6CDC9CCA06821085E2B085B2C8A35790104107202F87DF605C332985BFE9C5B803422E44FFF5CFFE42DC3AF6547187E9FB80D1766756E00473E64D00758BEFEEA0526DB1E480980739106AE451FD8FAF50CD847F718F00C1B8342EE1530189F32211DDA8E1EF178DA0128918B76380C7A3580A4FE13B70AC341DD1E86BB2A092811CAA9610BF79EB1008333B414C0792115400021D70D7F0257309CF9F687E10648F39FA313918685C42155013D221FB4BA67CF2586DF6B4F02BB7ADF2175BDA2030347EE79C4AADE7F6780EAF2A0337EF8867919708CFB33E068BDE3E80DFCC7313CFC0918A68F81A5EB674622D616FEC7E4626CCA256E93EE6DD9048643FAB3C1890063044F4092815502520D487330B813AA06000208390180D2F2AFFBEF19E68039EF1F30FCBD87340BC8E10AD9E54A43FCFFFA13863F8B0E32FCBBFE1412EEC08628AB773F037BCA7E48AE07AD46FAD50CC9F90C5F71CCDAFDC733AF8F6D3918038EAA02D7D231E8D2CCA7C052F51D1303628316B6F57F78169F3262197D64F88F3D2D40C70B3EF1A8301C359CC3705B2E019C76201833D17083AA81FF906AC04C00BC67800357290010404C682EFBBDE002C3465863F0CF39A4C6202730013072D126D77FFAC6F06FD36986BFFBAE30FCFFF9077263864600037BF67906162BE89EBD3FDB81C57D0564AA1763848FC0824C9C8D39061CF53A03F67AFC1730FC5E0323FE3533969E35B646220391437EFF71E77E46D0603F2FC35DB944866306F380B95E956089C1A5690BF78E313FB81AE0C49500000208BD7BF077F2298697EFBE33EC0619003EBA05B931082A05A8DCD0FB7FE62EC3BF352718FE3F7D0F1F80628D5ACFC01ABD1EB296FFDF4348C4FF5E08A9EB19080CDCFCC7B142075BC30D6B5B00CB481E28B2DF0323FD1530F27F32E218246220AEE8C7EC6F227239236663EF3DBF21C309C3790C77E59350723D0433C031CAE08EA024031BB41A9062075703DCB8AA018000424F00E0C6E0C9270C3360E1F2FB0C52639027887AB91EB4576F1DB09E3F7B0F9CEB610350AC59E721730FA0C8FEBD0618E0E5D0E95D06CC0929065C43BBF85AF5F856F2A0252650687C0286D96B60E6F9C68C656A17476412B5841C7FB5FF878587E1B6723EC339FDC90CDF39249122195BBB01D3101E234835C0C1C4A068C28FBB1A0008206C03047FC2D7329CF90E6D0CFE3C82DC18940076C0F5286CE4019BCA276E31306C3BCFC0003A68113AE1C49CB49F81D9B31F3200F50F58CCFF0046FCEFD5B8CDC157D7631CD68063AE1FD77A3E501DF41518E16FD88034139A1A2C918975E3087AC2F88F7DB5CF7FCCF8FC2060C470DA7821C363E930E80410440235D7E32F05B8B56CE1B69A0B30F83320EE264489738000C296004083983F1F7C6098031E1378F780E1CF5DA4C620973BF90D3D60AE67D8086CB95F7D02DFB3C7E8D0C0C09C799E017CA40C78340FD8ADFBD10869F0E19D0DC253D7FF475EBC8AAF7B88C58C1FC020F9C0064900E8CBC7D123F23F9EE55E589B01F8CF14F8C30AACEB550A182E1A4C65F8C929098D7C440E27A514600556039C8A8660278AB332B8E1AA060002880947D0FE5E789161E35F506310941ACE203506B93D8076719396EB7F02733DB081C7B0E32203C3971F907093776060CA0016F70ED0AEDD9F0390011DD06C1E3166E25BBB8F2D0270CD02C2F06F60507C0446FC17603DFF97117BA4FFC7D5B7C73198C388A3AEC7D220FD2868C470DE6431C3539908A4A95F44C493530AF01943AA01564606313F51F0C820466310208070AE099C7606DA1804C5DFE905E06E214A2220B6A1F7E23D03C35A60A43E7A0B6D480A804F15614CD80F39590494D34139FEE73468238FD8F9E0FF78E6E4FF6319F2C5D1D70745F63760987C06E23F8CB8733B7AA4FF67C0DDF523663F00B4D1F7879587E1BE6A11C315C319E05C8F1EB1949402BCDA767097AB7031F8602B05000208E7AA60502970EA19C30CF83280D348A5005F2871B9F405B007B1F332F8982F3000ED33CCBBCFC0003B480AB471E35B2964C289E4F604039E717A06EC43B7E863FC3F81E1F005181EBF98317B02D88688B14D15E31BE1C3D6F340029F8075FD2593A50CCF652371462225A50013270FBC14E06766B092E76090803606E14B01010208DF6A913FC09ED8994F3F194E834B815348BD0116A0399C368473FFF567F05C0F3E5C02B4CD1CDCC87B008978D0EC1D29B99E01CB281E313B7DD05BFEA05C0F8AF81F2CD0E327B1ADE3C7359E8FEB34301CD5022352830F1A397F812DFC87CA850CD70D6630FC42C9F50C18118B9A10088D1CA2AAF9FCF92BC36B016D480F0B98FB6D04C06D012EE419428000C29700C08DC1A79F185682BCF217D818FC79650352F9E24538973E7E07516B5EC0C0A00FCBF5C0BAFE5B3D64E60EDF9A797C18EB942E03EE153CC891F90B18E95F8191FF97094B4EC6B6A297017B1BE0FF7FEC2DFCFFFFF10E067EE63762B86ABC84E1A50CAE5C8FCA471EEDC39DEB51F97FFFFE65B87DFB21C3F5EBF7187E092933FC67856C2A0136066DD1978A010410138176F66FEB850C0B618DC16F0791BA84DC76906E21AE3D7B9F7F20D42AD8232DE5990F9DBC213FFEB12FDFFA8F7B34103669F31D18F13F59B177CFFEE3199AFD8FAD5D80A5AB89AB7108CDF54F940A196EE94F67F805EED733E2C8F50C28753FA9A5C0EBD71F182E5EBCC5F0FEFD2706C6DFDF19584F02ABEE5FDFC0CE7FF58BE12A03E2FC00B046800022B46E1C540AFC78F685613EF8C4CEDB07C025011CF087E32EFEBFFCC0CC05A0153BFFBF305016FBB8A674F1CC078072FB770E08CD80A781C7C880A37EC76237AE36079659BE2FC0BAFEA6D1128657D21128F53ABE061C6AC4132E057EFDFA0DCCF1F719EEDF7F022E01985FDF6260DFD1C2C07CEF38A423F697E1F5DEF7E0CB257E3320CE5D660008204209003C41B4FF01C31298B7BE6C6F444A00DEB88B7F1626DCF3239462062C4534B6153FE0EE1D30C7FF60C76C40FE67C03F29C480A5E1F69FD0C81F6A2F0094EB9F291632DCD545CDF59811C98025E7632F05B0E5FA172FDE325CB972075CE783723DFB91E90CEC7BFB8055DD5BB0533EFE65B8BEFE0D43D5F35FE02377910FDF6600082062CE09FC5BBC87E1969B22C37A114E86C0EF973730F07E875E8DC2C40BEC110013C1C72D98BA0410975C80E61328BAE016E72010B66A0029E27EB341FAF738876D719CFD82B33AC0362E80A5FD00ECE27DE5336278A25CCBF08B1D5A4D825BEFFFD13204FA823FE84D515045E00B83E06C06F4E5C10C3F7FFE66B87BF731C3C78F5FC0EA989F5E60603D3E9FE13FA8C807B5E2FF317CBBFD8D61CDB2970C2B416B93413504283D30400E99002700800022260180570BDD7DCFB04C189800FE7FFBC0F0FDE402066E07E82C9D20B031F37E13DEAE3AC38B8B0C8C1A01543C5409CBA24FF4C59B3F39A0D74DFCC7B249034BBF1D47570D6BA39301C72110E05CCFCBF05A2685E1AD4438F47027D861DC90C847CDCDFFB12D11465287D0F31F2DB1BD7CF996E1F1E3970C7FFEFC01E77A96637319981E9F879F79FCF617C35960C4F7BFFC053E41F43D34017C6240DCC100060001C442647EFB13B08661DF832C86A7ACCC0CD29FF74F4424000E7560C7C20874F92DA6467E4E86FFEFBF03AD87B61B18B9A85402E05BBE0D2AE0D8B137CC709DDC81AF01F79F0175EF1EC6FC012262BE0173FD732548AE874DF73342F5429C8C63F09F885200966E20B9FE09C3A74F905C0F8A746660E4FFFF0969E88172FDE94F0CD3B6BC61D8CB00393DFC1D34D77F65C0720B0B4000117B5630B84B78FF2343BFAA2043CF9FB70F18BE5FDAC0C0A907DD9021E007B92C0963629A1592009E5F804E26293250ED605DACEBF4404D1C58E4FFC31EA9FF896898603BE401CFBABD7FC0BAFEAD5432C33B7158AE87C83332228C82E568CC3A1C7F29809C104075FD93272F197EFF06E6FA5FDF19988ECE62607C740EEEFD37BF19CE2E7D0ECEF54FA1110FCAF9A0B367BE2335FE50004000319150EBFE725CCAB0E8CF7F4897F0F33EA42E212801B04A623604F939203D30580200FB448C0A0D416C7301A0720A18F9FF18712FDCC43979836D860FCBC20E46CCA2FF3B9F21C323DDC50C1F2423B0B4D6195146EE70CFFF628E03209B036AE1DFB8F10058E4BF00B6F0FF0173FD3906E67545F0C807E5FAE31F187AFA1E32D40023FF0E501BA8D87F01C46FD11B7DE800208048392E1EDC257CF19961BE240F43DE0F6097105412C0AF67118E015ADB89AA039A00C089E0FE01C8010EA025E6A0235AA9D5108445D61F3648E433FCC3334FCF80E52008061C43CCF80787FE31F332BC974962F820168694EB91733E2345A5004CCFB3676F8039FF0D38D7337C79C3C074623103C3C333F0BAFE0DB0AE5F02CCF52F7E129FEB9101400091724A18B83178E031B44B08B4FCC356A42EA1A03F3072B951C6029880AD465850C24B01D0E2528AC600B00CFFFE0546FE7F263C753B039EC51C84267D304B8E1FBC860CCFB417327C0236F4700DE8602B05F0AF024195FFFEFD27C3B56BF78109E03538D7335EDBC1C0B8B1121CF9602FFF67F8760C98EB7B1E30D400239FA45C8F0C000288D463E2FE56EC67B8F5E61BC37AF098C0F1050CFFBE43978C3183BA844EA811C00A349E9315E2E07B0711ED008AC70090EAE97FCC10FC1FDBB66D6C1331384A068C133D3067F5FE3103EB7A851A8697EA5381B58D24DEB178E46E1BA24F8F7BCC1F99FFFCF96BF030EEF7EF3F1818BF007B6EDB9A18184E2C8234F480E63CFCCEB063F24386944DAF18B642231E845F411B7B3FA0AD7CA21A5B0001446A02009702675E30CC8085D5C7BD486D01896C8C45208C225C90121AB6C298E21200B9EB06DA81C386B9AEFF3FAE7E3B9EB580FFF18D02024395DF96E185EE5A866F225E04C7E2D14B0158E22034A083C8F56F207257B631FC5F57CEF0FFD935D868DE9B3D6F181AA63E62E87FFE137CDB2A682BE07352733D3200082026326ADE3FE93B184E7FFE059925FCB07702C3BF6FD052804D9A814128002590994438214CA01A7035005A4C42494310B995FE978DC0916F04A66F314A06CCE21F94EBDF29B533BC55EE00B3499991C33EB2872BD7BF050FE5FEF8F1131895AF19FE6DAE67F8770C58C2FEFC0A76E20360AE9FFA90216FD71B86FD48B9FE3539B91E1900041039B74D82BB84773F30CC06738011FBE502D22CA170204A026016E680072B7C6919AB2E858D3F70DF0BFBD02CB67DFC8C783678E0593BF053C096E18DF61A304D6CAE471DD685D5FFB84B0150AEBF7EFD01B8A10792FF7B6933C3DFD5C50CFF9E5D053BE5E73F8637BB81B97ECA0370AE07D5F54FA175FD3B68AEFFCD40C1D9BD0001C4446614FC0E5CC7B0FEC71F602A048D3A6D466A0CF29A3330F098C21B828C5CCC0C8C9C2C906AE0EE412A5503A0A95C6606AC9B3C701EFA8863DF1F321B366803CCE91F15DA193E28B633FC67E1255874E39AD2C53DBE8FE8D783BA77A05CFFFFF32B863F1B6B18FE1E9987C8F5DF8075FD0386BC9DAF3172FD2706E2AFD6C50B000288894C7D208BBF3FF8C8D00F4E0D6F1E307C3A86B460442A0F6B35F01B5602B05950D610FCC78ABBA58FF5806706069C9741A025845FC0BAFE9DD66A865FD05C8F3E23474ADD8FAB14F8FEFD1738E241C53E78E5CD854D0CBF561430FC7D72055ED7EF7ACDD030E93E30D7FF40C9F5EFA1237A14E57A64001040E42600F09231EF350C0B7FFE01A74A860F4791978C014B0136297824B14842BA83A01E03B81A00B503E0A382A4E67E661CFBFB712C17C738D10BDBC20D48AEFFA2D0C6F045B10DCCC6B5268FF8BA1F7B2900CAF5376F3E04E67A607B0D98EB7FAEAB62F87D6836C3FF1F885C0F8CF8BC1DAF6897EB910140003151A017D4E8F8FEE63BC31A90C3BFDD38C0F0EDE60184AC743EBC27C022CC8E58207C6523A21D4072FC3342EA7E5CC53ACE5543FF19F09D0BF09BCF86E193C62A86DFFC7618F534A9AB71709502A05C0F8A78D0240E88FFFBFC0686EFCB7280B9FE32D8553F8075FDCE570C0D13EE31F43FA371AE4706000144490200AF15987389612A6C78F8F546A4B6806808A457006A0700E38C450492087EDF41AA06C8C9FD384FEEC673B2378ECB1FFE3373337C576805E236A00379B1CECB93BB260FB91478F9F21DC3AD5B8FC0B9FEFFA7570CDF579732FC3C30139EEBEF7F65D831E12E43DE763AE57A640010404C14EAFFB3F41AB054FBC2B000E491AFC0520084E140B6001E31AC12906AE0F7D30B905545E09E003709F18FDEF0C333CDCF8067A93854EC2F30D77F535FC5F087CF96E0583C29A500B21E50E3EED6ADC7E00400E2FF3ABB9EE1EBA24C863F8F2F41364901EBFA1D2F191AFAEFD237D723038000A23401800786E65E6698F2F73F6447F1AB0D48A5805828B414F8CFC02681540D5C86761BD974C9C8FD045AF788D54E58CD01D5EFBFE45B197ECAC3723D23CE3579E494023035A048BF7DFB31A485FFE925C39715450CDFF74F03B6F02147ECDD07D6F5FD7718F2B6BDA47FAE47060001C4440533FE2CBF0E2D05401D53F45240AE08DC0E60E2640262664802B803ED0EB259129FFB1998F09CDA85A5958FBE440C3421C56BC3F04B7D2530F7DB12188B67C0B27C8B502980C8F5B76F3F6178F5EA3DD8CC9F67D6327C9A9FCAF0FBD145C84932C05CBFED054343EF6D86FEA70394EB91014000917B7D3C3A608FD46090AF306338031A09E0D6706050AE443A4FF0A419304D3F66F87CF913C3D73B90850BE21DEF19983880DDB9D72144180F3DFD1C360506BBB6FD1F321BE9AAF67F4834F8A2286E86BF52150C7F79ADA1A5D07F248C9BFFEFDF7FF87C3C8C8D5D0F440C94EB5FBC7807E6FF79FF8CE1D3A636865F0F2FC04BBEBB5F18762C7FC2B0E8E9778C993BD8481E5D2F6302B913208098A864D69FE537185E3CFFCA001E0CF872FD0018C3817C313804386511D3C33F2E6D807407599408A55148E4639CC58FE73838A4AAE13F30D7FF05E6FA7F7CB6440EE8E02B0518B0D6FD3F7E4056E980723D48FCFBA9550CEFE724C1231F94EBB702737D0F28D77F1FF85C8F0C0002885A09003C3CBCE00AB02D00DD43F0623D525B40229C8181DF9281858F19D82380AC6FFB711B5A0D800F9DC0B7C18409CB26CDFF0CF8177682B6C2F030FC976D65F82FD70A9EA92434168F7D4D3EA1D13EC83AFCBB779F825BF8FF3EBE6078BF2887E1F3AE490CFF7E40EAFA7BC0167ECF2D86BC2DCF07B6AEC705000288898A66FD59711358027E039602A0535FAEA195028A2590E3AD25D8C0BEFD7611DA1064B724C28978CEF1C5B6F50B98EB1954573230405BF8A8AB71089702E80D3F6C75FDCF9FBF18EEDD7B064E0020FEB793AB18DECC8C63F8F500727DDDF73F0C6FB63C6368E8BA39F8723D320008206A26007029B0F02AA447001278303311212B600D2C05AC18382421A75B826610C189007C0299128ED61F138E85190CD8176DB002CD926E66609069019ACB833547133BA083DE03404E40A04807453E2811FCFDF09CE1ED824C868FDBFB19FE7DFF02AFEBBB6F32E46D1EA4B91E190004101395CDFBB312580A3CFAC43009DCDA7FFD80E1CD21A43902B516064E09167835F0EDE24644298075EA9709AD94C7B24207C6160C06B635663130F0D8608D34724B01E4C4018AF0070F9E33BC7903C9F55F8E2D6778352D9AE1E7FDB360D780EAFACDC05CDF7983A1FFC920CEF5C8002080A8D50B40E91180F2FBB1088673AC8C0C526C220A0CDA1DE71998B9A0670F5FC96278BB652EC3E787BFC0C7BCCAF7BD074D133230BCC9C4E23A76D47C026FD9FF47B4F499C51818444B191838F5F1B67609B5F8B1F1FFFD43F40040B91ED2B50366E1A7D719DE6DEE61F87EEF0C3C3D5EFFC8B076C5238635C0887F36185AF8C4F6020002888906E68252F9B7071F192680E7B381A5C08B6D48AB8634DA19386584C1F10A5A4BF015B49680451998D9C5B18CFCE1B8CD0B96EBF90381C5FD0CBC918F6FBA969852E0DBB79F0CF7EF3F6378FBF61398FF61CF0C86E79322187E00231FE484CFBF191E2D79C050D2739361E650C9F5C8002080689100C0A38351DB19167CF9C570061455CFB74D60F8FB15BA6A88859F81CB22175E0D7CB900AD0638AC50CB7FF80A5A2C4BB440B95EBC9B81412813DCDA2758CCE159B481AB2DF00F58BA80723D681DFE9F3F7F197E3DBFC9F06C6218C3FBDDD311B9FE13C3DAFC730CD9FB5F319C8446FCA0ADEB7101800062A291B9A022EFDBB5770C3DE08520C0C8BFBFB01021AB52C9C0A5A8044900E7A1BD014E374CA7619BD4E101E67AC969C004A3476A6D477429005AA5F3E8D14B86F7EFBF80C5DEED9AC6F0A43F98E1E7B31B60F9CF7F181E2D06E6FAAEEBE013549E4323FF2552AEFF3598733D320008205A2500F04C61CE7E863D1F7F32EC05CF141E58C0F0F12AA25BC86D5B0BAF06C0898015560DFC874CFBA2CFF2318B02EBFA0E60632F9DA85C4F4E2900CAF56FDE7C6478FAF43530D7FF03E6FA1B0C8F7B8318DEED9C0A77C635605D9F7B86217BDF4B70AE7F064D006FD072FD90010001C44443B3C1EB05B63D60A88575D11FAF460C0E719BC6831B81E085A5B0C5245C6ED85BFFDC01C0C89F0A6C5EEA51E8247CB9FE17B0B87FCDF0E1C357706278BB7332C3C3EE006083EF06E4345B605DBFE83E4349E735945C0F5B9BF70D5AD7FF671862002080689900C0DBC9265E60B8FEF42BC35470445F39C0F0F200A25BC863100009DC731BC02501380120D7F7A0BA5E1898EBF9D3209B4E288D7E2CB9FEFFFF7FE006DEB3676F197EFF06E6FA67D719EE77FA31BCD936199E0641B93EE73443F6DE1718B91ED4CAFF39D4723D32000820261A9B0FDE4EB6EC26C36460D882AE4F62B837AF10DC26000141977C78747F3EB70172E40C2BF40874DE6860436F0115723DEE5200347CFBF4E91B864F9FBE8213C59BED1319EE76F8307C7F721D721AFC2F86470BEF3294B45F1D5EB91E19000410AD1300780DC8FABB0CCF810DC26610EF3730F29F6E81740BD9650DC07B0B41A5C0FB23D06A802F1E58DC031B79BC31B419F8806EBD7EF7EE13C3F3E7EFC0C7A9FC78728DE16E9B17C3ABAD13E0CD8EAB1F18D6669D62C8DE330C733D320008205A0C046103A0336AF9B7F9326CE36261300105B0C5CCFB0C1C620A0CAF363632BC5ADF001ED7D1E8BDCF001A38A22500E5FAD7AF3F32FCFAF5075CFC3FDFD407C6B046DE4760AE5FFB8861D2EEE70C171910072B7C44CAF1C322E26103410001C44427BBC0DDC2532F810D4268B1707D12649E40D03A1E5E86BE3BBC90660E008DE6BD7FFF99E1E5CBF7D05C7F95E166B33BC38BCD7DF0A20A94EB334E326403237F58E77A64001040F44A00E0C1A186530CA79E7D61980A0AEDF7C006E1F37D0B185881399E4BDD015C02BC459E37A022009DAA015AA8F1F9F37730FFF9A65E86EB8DAE0CDF1E5D85344281B97EDE6D869296CBC3B7AEC705000288898E768172CFB765B719A6FC0496BCA080BF3507D22014B489879CDCFAFA01C367E429642AE4FA0F1FBE30BC7AF5019CEBBF3DBAC270ADC185E1D9C61E78917FE53DC3DAD4E3232BD723038000A26702003708B73C607876EB3D431348E0D7970F0C77973532F01B07303041278BDE1E5A48B55C0F8AF8AF5F7F801B7EA048BFD6E00C4E04FFA163F87381B9BEF9D2C8CBF5C8002080E8D508C468106EF0803608811C93B6FD0C9F8F2D044F1D838E9FD39B741F317B4846AE071D9B061AD801F90D14E1F7E6E4327C7B78053E9108CAF58D17C0CBD76087287D60409DB91BF6110F6B04020410D300D80B6E106E7FCC90FFE73FC3175084DC9855C8206417CF009B37787F7A0399B9FE17780207D4D2FFFBED23C3E3E5350C57EB1D21B99E0192EBE7DC62280146FE88CEF5C8002080062201801B8433AF325C7FF98D015CDE7FBA7781E1E5B983E02E2028113C479E3E26322583EA7AD0302E88FDF9C631866BC0887FB96B26643712B0FABFFC8E616DE26186EC9D4F47665D8F0B0004D0405401B0E138F0C291F5EE0C07D89919D44111A51956C0F06AC7047051ADD7799E815BC180A8BAFE13E8DAB97FFFC0B9FEC9FA4E86973B67200E4CFCC1707DE60D8649A7DF80AF737E0FC5C3B25F4F4E150010404C0365370364CAF4CBD1170CC5B0D1B7A7670EC027019F6D9D48D0F1A088076110FBCBCDA30C576BEDE1910FCAF5A75E332C4A39C250028CFC4B0CA8F3F5233AD7230380001AA8120006403B3EF866DB31344972316481229F9B135834B030807B0526D3EE33B0700B60CDF55FBF7E0737F8FE7CFDC8F0747D07C38B9DD31147A78172FD758689C004709B01F598D4EF0C4368AE9E1E25004000310DB01BC00DC2B5F71926FFFCCBF01CBC4DFA17B4BF086C0CBE3DB501AD85FF8FE1F3E76FE0C80777E56E1C01E67A5B70E4C3EAFA93AF1816251F62280146FE65A4BAFE2D52AE1F8D7C24001040035D02800068D727779D11838F8908C352502E16E0814CDA80DA00C6BDE7E1B91ED2B5FB07CEF577676530BC3FB31525D7CF00E67A600218CDF52494000001341812006C6C806F8E1D439F083B432C3BB062E0841EF1AFDBB097815DD912BC2E0F04DE9DD9C2700F18F9BF81890096EB0F3D6758D87799610B52230FD4AFFFC23044D6E50D64020008209641E2167055B0F22E437BAA3A830B30B62439A12BC29FEC99CFA0A060CEF0EBCD2386874BCAC1090096EB9F7D6538DB7A9EA1FFC167F891E8EF47733D69002080064B0900AF0A6A0C187C8C8419967271005B882C9088960CAC6278B17D2A4AAEDFF69861DA8C6B2847A283E8AF48ADFBD15C4F440900104083290130C27A0533AD19FAC438196279916EAB87E5FAFB9F190E755D609809CDF5241F8E3C0A50130040000DA60400024CD00122C129160CDD0A020C516CD013E13EFE6478B8E521C3DC45B7C023791F9006746047A48EE67A32120040000DB60400AB0A40170E81060084402502B491F80F9ACB3F43EB7958AEFF339AEBC94F0000013418130008B0404B025042805D75FA1F5AC4FF402AEE47733D8509002080066B028055072CD01201E6C0BF50FC6F34D7532701000410E3FFFFA319682403800003004042FE192CC3EC400000000049454E44AE426082','folder_yellow.png','folder yellow','image/png',0);
INSERT INTO "kexi__blobs" VALUES(5,X'89504E470D0A1A0A0000000D4948445200000080000000800806000000C33E61CB0000000467414D410000AFC837058AE90000001974455874536F6674776172650041646F626520496D616765526561647971C9653C0000380F4944415478DA62FCFFFF3F03232323C328A00B60026266208605F83F244C77008A7B800062198D13FA447CAF1D83A88938433E3323831D28CF7DF9C570DC7323431B50EE0710FF02E23F03E1308000621C2D01689EE3590E84321472B1305402D9FCC0E06680E1B73F1836F86C62C8018A7F04E2EF40FC97DE25004000318DC6116D321628E2770733849D8A64B8C3CDC2D0018A7C0660A4F36A3A30B08B2A80980C42EC0C01B9FA0C3640260F10B322550D74030001345A05503FE299370730188B7032F4B2323258832B7A606CB38B28302866CC67E0D57260F8F3F503C3E944417022D01761F00252E7A125C06F2CA50023B4246182B2FF23B51BFE53EA6080001A4D00D403CCABBC195464F918EAD89818A220452C30D6380518E4E3FA1944EC121081CE2DC0C0074C081FAE1E6010E36430020A7103313B107F434B00CC6BFD8066F232D401A35AFED73F862757DE30CCCCD8C37016DA76F843690312208046130015EAF9E9CE0C22BAA20C859CCC0C59403E1F2CF225831A18C43DF31998B904A04A1F43E3589D814F1B920004D8197480025C40CC068D0F5029C034CB8D41444F14DC76A80099052A4558806580B1188357963E83CFB48B0C5780EABE40D5935D120004D06802A0B04B77388221898795A1131805FCFFA1112F649BC0201554CFC006ACEB21E00D10CF03E29B50BE2CB0043003AB05455DB5098373EB198665D076C0DF43110C41FCEC0C5D400B64FF411B8C3C1A0E0C9FAE1D00D501BC9E0A0C65C00490072D01FE52D2780408A0D10440663DBF3794C119987B7B80DD3A1D683C8223491218F13CC0861E048072FB2620DE8D66C463067E6D6B06983E453E7035B0B1C69241335815D8766062B086E57A6EA09952D1FD0C9C72060C77FB0319DE9FDEC000949703AAE705E2CF94F61E00026834019016F14C2B7D1854550519660323DE06D6C0631356609000463C28E723C06E68E47FC361DC4D067E603BE0FD95030C125C0CFA5B8319AA15F919426111CF04AC36A4A3FA19046D2066FEF9FAECE77FE62FBF401D89BFFFC0A50F27B4C460865603640180001A4D004436F02603EB794B29863E766003EF1FB4C60535F0C4DC0B182402EB9194821AF42BA0C53E3E004C00C076C08FDBC086A02003A8AE50808D0F08BB16308805D483DB0EFF7E7FFEF3FEF2DC976F4EB73EFFFEE08D04B0C8E015E660D086463E0BA55D4780001A4D0004EAF9D9C0C698BE18432107B081070C693E586B4BC4AD80413CB01EA98107AADF3722D5F3F8C1F79BF7804A7731F07343221D94A8B8D41D18A493E733B08A40DA0E5F1EED7DFFE240CE833F1F6E7C676464F8CFF99781F927349140733E33B42D42360008A0D10480A78177229A21898B95A11AD4188335F0780D03C075329B0872030F54D41F25CAE03F6FBF30BCDD7C9EE1D3D1DB0CB0061E0BB00A118FEC079B0D023FDFDDFCF6F270C983AF0FB67C646262F8C70CC4A004C0F58F91E903B00800B9255683416DF10D86279496000001349A00B034F080AD7067010E48030F56CF73C81A304802EB6450A30CD1C0DB03ADEBBF1165F8FB2DE719DEEFB9CAF0F7DB2F702482EA7941970206113F4815022AEEDF9CE97DFAF654E33368C4FF872500903CC72F060E5835C1CF06EE6EB2209502648D070004D06802406AE06D0C645051E067E8656362F086D5F3A0DC29E65FCF206083DCC03B0AADE7898BF86F171F32BC597592E1F79B2FF05CCF6795C020EC5B0F2FEEBF9D9DC0F0F454CB8D3FBFDE7E0646F85F78CE47EAE3733322E6115404185491AA01B24B0180001A4D00A0811C5706510B29867A4E16864C7000834215983B85808D3121D77CB47A7E1E110D3C4471FF66D161866F379FC3238E4DC6804134BC1F5CDF83078CDE5F6478B73981E1F7AB0B0CC048E7646265F8881CF9A0A21FA48EED2364808909C8072622466022E545EA05909D00000268242700F04CDDC5448606606066C31A7820CC0FCC9DA03A1975046F39D10DBC7FDF7F317CDC7A9EE1E3BEABB0061B0323B0C7201AD60FCEF9908A1E587D3F6A61607C7F9DE1DFA7DB608BD9FE33F0013BF4CFC1118F14F9E016DF2FF04821030730617C063606A5B81874D1AA00B20040008DC404009EA93B13C790C4CD0669E0C1EA792E350706C924442B1C52C4AF20BA8107029FF75F0547FE1F503D0F2BEE9D8025894F3DB8CE67F8FB8981E1C502209E0F610333361BBF18C3F7EFF71998FE831321D6DCCCFC93113457001A04FA07349319584280E60FD8284D000001C432C2229EF9580C83333F3B432D30D75BC3EA7950B12C16812896C969E0FDBCFD82E1C3DA130C3F1FBF830FF172A83A3088C6CF07B723201D86B50C0C4F2741723F1C7C02260071866F2FC009809DE92F03CF7F26F0FA0038002DD960FF068E70067666705A65E0656550A446090010402C2324E299B68630A82809808759BD61F53C2B306284812D7078B10C6FE06D22BA9EFFFBEE0B30E24F327C0736F4600D3C6621050691B8F90C1C6AD004F5ED3AB0B86F06C6F549AC66708870C16B17E6BF0CFC7F59183E814A02E40620F30FF08411030F0BE35FD0420E905D91AA0C1ACB6F83BB8264270080001AEE098069960783A895346A030F5414F33B1730083AE7438A6578036F39B4BE27AE9EFF7AE02AC3E7EDE721110F1D19E4712C6010F0868E0C828AF8C76D0C0CAF8139FF3FEE5E1A23CB0F06561E4186DF5FDE83DA0142DFFE333CF9F70F980098C08BB5FE33FF646063FE0F2A20C03382FF60258C003BB861C84A4957102080580622374269585D87BCC081AA0DBCAB290C0DC02213DCC08359C46B99006E8C21221E7DA68E8851BC53B7C1110F6AE5C3EA792EF30406C1907E70220083D78B18189E4D0676053E126122B01A101063F8FDF93DBC1D80DC1660FFCCC00BAA06C05D4116480200614D410643A0D00E4A7A02000144AF04008E90BB790C4E3C6C0C0EA0C90C2066FCFA9BE1D1E9670C8763D733DC65804C68C0A637FF5390C058CE2430040B7030743143A753C18108AC8F458011CF2E6B407603EFF79D170C9F779C67F875FB39BCB8675371601008EE676095819AFBE534B0B8AF82D4F3FF89F5C637060E6129862F8F218990F90F83C87F568657E04400B2E30723274A95016C077CFBC3C0C4C3C220CE805847C0C440C6AC204000D13A0130AF0B6710B15760280016C1D1C03EAC2CF2A24841A0B76480BDD97BB90CEBD6DD60E82AD90DCE863F18B02F8D22D8C03B9D009EA2AD854DA7FE870EE4801A629C6AE437F0FE038BFBCF1B4E82733E7C9C0058CFF37AD683733E24753C0316F735C004700A5896FD432ADC18884ACF6CC014CBC4C2C6F0EFCF2F06D67F0C423FFF33BC86C9B17E01AF196480950AA00400CC3C4C5C9004C04A494310208068950098D783225E91A100E8C82C60C4F3FD83D65BA00064145460F8F7EE01840F1406B68182520D19825C1519A6E8CF62686580AC74F9416469C0BC2B023C8257072CEEA3601104EA770B87F6838B7C721B78E0E4B2F33CC3B7435781590ED2AD63E41060E0B62F60E076C88714F77F3F3330BC5BCAC0F0723AF61C4F7459F61ED8189466F80AEC0D001B828240A35881A5E43FB65F0C2CACBF1838918DE16265F8FBE63B038B201B78A89A15A9042019000410B51300D38608065150C403EB2A94E5514C0A0E0C4C46F10C4C9A01E0400437A49E5F60F8756422C3AF330BC0E124C7CF90F32097C1A2740F43D2EAEBF0F553B8D6BD31CDF36610B59406DAC50A5936058E2060A4F003FBDD7C4EF988FA98C4061E08FCBAF290E1EBC6930C7FDF22866FD9750218F882FAC1B91F0C3E6E06F6E77BA0FD792C114E5245F609980064C00900D8006061F9C320FC8F89E13972FD0F1F126661FCF71F3A29E428C520B7FF19C333682940320008206A2500701DFFB98AA181830569540D1421C0886776AC67605274C0D42429CCC0111AC5C06AADC8F07DD54486DF4FDF31F0B133984CF160386625C31053B89BE118B434F8C58058050BB6EB7A1A4321371B432530B0F861F53CB74502B8050EEF7783733AA89E3F4FB447FE3E7BC7F00D18F1BFEE3C874FD3B2481930F004F683EB7B482BF01C03C3AB5E207D8301DE05C088F0FF24A682BFC004C0CDC0C4CCC6F017540DFC6510FBF19FE105DB57465E7495E0B10068A234106150032680B3E4F604000288D204006E74BDAB6048E2058DAA3122A64D198011CF028C785002406FF04022045114334B01232FDD8BE1DBA6930C3F4FDF66606664E089D165D8004C04D3279C6298B2FC2AC373502DDBE1C8C0EFA9CCE027C8C150099EA285863D68C04500D8026793416EE0615B8A85BF9EFFB1FB3CC38FC357E1F53C03B004E1F3EF67E0308356237F80CE783D0198240F20D5F3E44436EE6A80534291E1CB939B0C4CFF80A5E75F064E8ECFF01E0CBC140086F55F580290E1064F0AB1919B00000288DC9D41E0EEDCFD420613491E865E3666C4FA770671030626CF7E1C114FB8F1054A00A0A2F71F74CA14E4C95F7F189E8298CC4C0C32F04624C8C7C0B68450EC7C700B1F013691D4C003DB79E42AC38F5DE7C17D7B98F91CB6050C5CEEF5E02A85E11FB010FAB88A81E1C30A48710FCEF5FF2018CEFE0F4914B8D8C4359D18FEFED064787E7C1384F78DE19DE04B46AC99F4DCCBFFDC1F7E3230BFFCC67022E920787711A87EFB002D2D894BF440B7010410392500D3CE3806111B39867A6063240BD6EEF9CFAFC0C0E4000C3083042C5A888F14765355608920CCF06503A2186661669046EE3D805BE05EF50CDCE69435F0FEDE7BC1F06DD52160831451CFB3283930F044CC676082D5F35FB60333E63C482BFFFF3F3C39FD3F15DA017F199839BE4106853E034B83AF8C02D02A1073F410341EF083819997954109AD2B48120008201612733DF3AB328660214E866E606E9485CBD837303059E4035D2580A685F448013B4A5A884120DB93E10FB04D004A04FFBE6B0323470EE2715D7F449F1BDEC0237E2916384EDE7F61F8BEFA30C39FBB88695A266069C21D3E9F8145D901D6E907B6EEA700FB22E7A0451B214391E9FF1455033C32EA0CEFAE9F6000A60526681C616C1CE56565FCFBECFF7F5676260631684F80959C04001040C42600A68D510CA2EECA0C73D95918BCE17E96776060F49FCFC020A080A69CF448C1951040180240E90D54D30843CD7D034D602444FC8F5F0C7F8E5E65F8B9E73CA29E07265A4ED77A067660910FE99A808AFB69909CFFFF3FFE98A656D58FDE0E10D566E03ACFC6C0F8EF37C868566C0900D80086F5AC1992D4184CE7DD82F70418497111400011930098DF563004031B5EB3804D057E0668A03100239E513D004B3D4FDAE81AF1E02845E6FE3D779BE1E7969328F53C9B710203876F3FA49E0717F78B19183EAF0526824F0472387210935DDEE3CE6D2C1F817D3F01606B0E3C16C40A34F507239AE1C80D41791E784390E411418000229400983F543224F37330CC848B80221D94EB318A7B42EBE00706FCBBFF82E1F7D613E0EE1DBC0D01ACE7D97D80FD79296855F2F33830E2A7038BFD17689336FF518B74A2E2F73FE53D836F2F1858BFFD65F8893450087225B212562686FFA0EEE0F7DFC0E60A3B830AB909002080F025002694C887E67A068C5C4FFA640A3DC0FF0F5F18FE6C3DC9F0F7FA4344710FACE7398111CFA205F5C3BF970C0C9FFA81EDE68B8888C799A1FF138EE4FFE48E02A281EB6F1918817D612666604BEFEF1F5099CE8A9E0020ED006067099800F8D8C00D4176721A820001843301805AFA7CEC0C5DF0C88FDBCFC0206180A575BF9161B081BFFBCF33FC3D761551DCB30B30B05A1730B03943A769FF7F6560F8BE1C98D33640231E5B4E273107A32716721B83BF817A1E7F07CD113330B3730163F813C81826602AC0680CF2B233FC7DF19581859B19BC38848D9C86204000E14A004CD6B20C05F03A3F7C3D96C89F47A3BA9E02006CE4FD5D7F98E1EFB58788E2DE3081811518F1A0F9074871BF1912F9A0317C92732A5A02818F7A5111DCFD0A65FC616066E366F8F39D11DC5F07DAC50A4C042809408813B23804241DAFC260BAF00E78C08CA486204000E14A00CC2C4C0C76601628E2E5D10775560CBEC807E57CA4C807B99BDDBB1F3104FDF72A30C74F02D22F91BA75FF891BCD23AA9D47858400ACF719EE7D4374BA81F1CDC40ACCE63F7F606D0C223704E5B818D4C86907000410CE0400ECEE59C31B7D185DBCDD832EF2FF1D0716F9A0FA1E147692060CACC9FB21934EA0E2FEE702603DBF8FC0400E5ACBFE3F7A51FE9F8841A0FF484AC928216E01DDFA1BB911FA9B8185830B960040E601FB8688B6001B33C37FD080D0B75F0C4C829086203B52354054020008209C5500EE52F008C3009D6A86BFE8DF0FE9DB330A2830B001231FDE4BF9D100CC48F7918A6FA47969184689706C91CF801499E8FA70241652DB009F80A5FB931FA8EBA418FF116C0C0A7380371A3109B1819789B393DA0E000820D2E790FFBF460DBC4180FF5D7F049ECC010FE5062175517F2D04E68307C883DF588A74B4C8C7DAFDFB8F96BB195073F77F1C098881047F5CFA8C39EE0A2D059839B8E0F1051D1882033E0EC6BFE02DEA4C0C622ABC0C420C88052244018000C299007EFE61B806E94E3D409F4180D4A18308C38A7ED08824BCCEFF0774F7EFADD8732C8A1803163603167D68918A5EC463338FD802E02130E77FC6714C20E32F606390153161F71F3501F0B3038B63A87516220C9A0C889941A266F80002085702F80FAC8A1E83FDF3E000660280CD840D12FCF7C663C878BE26527BE5F736B4DC88946BB115E5C8399DE13FE1DC8D2D4130A0D7F944E47C50D17FF73B6A5B8211AD1460FA0B6C947122AAEDFF88225E9087E117CC59CABCE045A224750701020897A27F6FBF311C874C9C0073D28F0F48529C83ABF87FF0021ECECC5AFE485D82D3B8733C4A44FD47ADDF19D07235D6D202475DFF1F8F7DD8C01FA0FCF56F101ABDE847C9BFBFC0D500928BD9E0AD756062E0E563F80EB24A849D411F346A434A350010403813C0EDB70C87607EFD8F5C0A306A0EAA04F0FFF93B78D58CE8F2016BAF7F5FB1D7D7B8EAFAFFC825040396A29F014711FF1FB3C8FF8FD61BC085EF008BFE2F7F117C462CBD0BB018A831082C05D8D86169831559B508B0A30052CAC7025E23C84E4A020008205C09E0AFEB2286F370FF3DBF88A443735015FFFF5EBC87B4FE9107AA407D7E0652EA7A6CC5393671B49202AB389612021B7EF29B81E1C56F6CAD3E2426234697109E2CFE33C07B70A2DC8C7F605E8B526030872602A2DA010001C4846754E3CFD75F0C27310A3146F9C15505C01200F29434A80188B5B186ABE186DE68C355EFA399F11F5BC980A337808C5F0123FEDE4FB49C8FA51460440E7DD0A010330323133C63B3C318E23C0C7F61A62B71331893D21D040820BCB3811C2C0C9A581BB3A06A009CCB0678C2E7E76F86BF2F3E401A809206C85380F873E67F022D79949201AD9D801EC1FFB1951C58D60CC047FB8025D79D5F840711C183B98C88440136FA17032B1737C3AF2F9F6099173C3FC0F28781950FD80E78FF818153880DDC0EE044EA0DE01D100208205C2984F94D39430D6C4B15A3843E969EC0606800BE82E72B66257BC444CFBF57F8FBE90C045AF2B85AEFD8DA10B8E606FEA3250A100645FEF55F98B1CEF81FC718C07F8C2E2113B01D00EB12028D87970262DC906161503B40998741042901E0050001C48425DDB1BC2D672806F62FCB61730128DD2B5000FFBD372812C09F9B4F2161CC21009EE38717FFFFB1B4EEFF13E8DA61D0D86607B1F5F5B1941AFFB1D4FF6F80917FE337A4C54FCAB4315A2DCEC8F81BD125FC0F8C606897504A90F127CC287B51703B8083986A0020805890AC613A95C6A0AA2DCA3007B4CA17EC0776010696C0F968C3AEF320CBA606BAF8FFF88DE1EFAD67E0B066D5424AA07F4EE16ECCE1EA0DA017DFC4B41D70F62EB0F41EDE01E9877F1193508CC834721B00691E8F11C90EE4E161869F0C2C9C3C0C7F7E7C8309B103A5BF8B7080561231FCF9FB8B8145818BC11628B51E2901E01CBB0708207802F859CBB09F9989C116EE7F60A38A25723D786205517F4D023646F70F8AE1FF9F5BCE32FCFB016945B318C7232580D398DD316C7534B6563DA1BEFE7F227B03C891FF0CD4E8FB87A38E67C01C00FA4F6832F73FB021081918FAFBF33B786410D854F8016A078809317C7FF29C81979F95410FAD1D80F336128000822500E6DFFF183E33434FA162B22C00EFE6818FA9FFB9028CFC39C0A2FFFEC0CFFABDFAC4F073FB7986BF2F3F4272BF7501782937BCFB07AAFF7135C4FE136A08624B280CD88B7E9CBD01280635BD1E02E92FB89688FD276E6A19393130C2ECF8C1C0024C007F800900DC1CF8078C6826869F62C0DE00E8B408D0BC80970483CEB6170CAFA071FC0B579202082078FDF0EA0BC30EB83D0AF6A891FFA90A480F7CBDFFE7D84D861F0B0F30FC83463EA8E5CFEE82B4CA0734ED8B51AFFFC7D3DD63C0BEE60FDF183F038E993F6473416B3A6E3320964732E2A8F41919708C0032A29EA0C088DE16F80B6C0C323130B3B2C19CC406AA48980C53B960CED5E6637064409C278CB31D0010403089FF4A131996FD01960220CD7FCF2F442A237480DA450776B4EFD507869F8B0F32FC3A7A13DEA662736960E0CE3F0F59D10B8A7CD0B42FACFB876B79177ABF1CD7F42EBEE1619CED0BE8E4D44B20F7112362CF20FAF80203B6D149061C2381781A85FFBF33B0707083F99FF854194FD82CE079A291C0C622AE0A3656940DBC9E830B6950082B00082058020055523F5F7F6158033EAEE3DA06D4F17F56F38119E9FBF193E1EFF19B0C3F971C0617FDE0EA11D82BE1CA3B8FC8F9A031FFEF399069DFFF84266970B4F8B10D0A616B24FEC7D56BF80F39B41D14F1EF1971AC0740AB0218518A1B3C0B507195127FC0AB76EEABA7309CB49BCFF0994F052CCA69E005D6CDC9CCA06821085E2B085B2C8A35790104107202F87DF605C332985BFE9C5B803422E44FFF5CFFE42DC3AF6547187E9FB80D1766756E00473E64D00758BEFEEA0526DB1E480980739106AE451FD8FAF50CD847F718F00C1B8342EE1530189F32211DDA8E1EF178DA0128918B76380C7A3580A4FE13B70AC341DD1E86BB2A092811CAA9610BF79EB1008333B414C0792115400021D70D7F0257309CF9F687E10648F39FA313918685C42155013D221FB4BA67CF2586DF6B4F02BB7ADF2175BDA2030347EE79C4AADE7F6780EAF2A0337EF8867919708CFB33E068BDE3E80DFCC7313CFC0918A68F81A5EB674622D616FEC7E4626CCA256E93EE6DD9048643FAB3C1890063044F4092815502520D487330B813AA06000208390180D2F2AFFBEF19E68039EF1F30FCBD87340BC8E10AD9E54A43FCFFFA13863F8B0E32FCBBFE1412EEC08628AB773F037BCA7E48AE07AD46FAD50CC9F90C5F71CCDAFDC733AF8F6D3918038EAA02D7D231E8D2CCA7C052F51D1303628316B6F57F78169F3262197D64F88F3D2D40C70B3EF1A8301C359CC3705B2E019C76201833D17083AA81FF906AC04C00BC67800357290010404C682EFBBDE002C3465863F0CF39A4C6202730013072D126D77FFAC6F06FD36986BFFBAE30FCFFF9077263864600037BF67906162BE89EBD3FDB81C57D0564AA1763848FC0824C9C8D39061CF53A03F67AFC1730FC5E0323FE3533969E35B646220391437EFF71E77E46D0603F2FC35DB944866306F380B95E956089C1A5690BF78E313FB81AE0C49500000208BD7BF077F2298697EFBE33EC0619003EBA05B931082A05A8DCD0FB7FE62EC3BF352718FE3F7D0F1F80628D5ACFC01ABD1EB296FFDF4348C4FF5E08A9EB19080CDCFCC7B142075BC30D6B5B00CB481E28B2DF0323FD1530F27F32E218246220AEE8C7EC6F227239236663EF3DBF21C309C3790C77E59350723D0433C031CAE08EA024031BB41A9062075703DCB8AA018000424F00E0C6E0C9270C3360E1F2FB0C52639027887AB91EB4576F1DB09E3F7B0F9CEB610350AC59E721730FA0C8FEBD0618E0E5D0E95D06CC0929065C43BBF85AF5F856F2A0252650687C0286D96B60E6F9C68C656A17476412B5841C7FB5FF878587E1B6723EC339FDC90CDF39249122195BBB01D3101E234835C0C1C4A068C28FBB1A0008206C03047FC2D7329CF90E6D0CFE3C82DC18940076C0F5286CE4019BCA276E31306C3BCFC0003A68113AE1C49CB49F81D9B31F3200F50F58CCFF0046FCEFD5B8CDC157D7631CD68063AE1FD77A3E501DF41518E16FD88034139A1A2C918975E3087AC2F88F7DB5CF7FCCF8FC2060C470DA7821C363E930E80410440235D7E32F05B8B56CE1B69A0B30F83320EE264489738000C296004083983F1F7C6098031E1378F780E1CF5DA4C620973BF90D3D60AE67D8086CB95F7D02DFB3C7E8D0C0C09C799E017CA40C78340FD8ADFBD10869F0E19D0DC253D7FF475EBC8AAF7B88C58C1FC020F9C0064900E8CBC7D123F23F9EE55E589B01F8CF14F8C30AACEB550A182E1A4C65F8C929098D7C440E27A514600556039C8A8660278AB332B8E1AA060002880947D0FE5E789161E35F506310941ACE203506B93D8076719396EB7F02733DB081C7B0E32203C3971F907093776060CA0016F70ED0AEDD9F0390011DD06C1E3166E25BBB8F2D0270CD02C2F06F60507C0446FC17603DFF97117BA4FFC7D5B7C73198C388A3AEC7D220FD2868C470DE6431C3539908A4A95F44C493530AF01943AA01564606313F51F0C820466310208070AE099C7606DA1804C5DFE905E06E214A2220B6A1F7E23D03C35A60A43E7A0B6D480A804F15614CD80F39590494D34139FEE73468238FD8F9E0FF78E6E4FF6319F2C5D1D70745F63760987C06E23F8CB8733B7AA4FF67C0DDF523663F00B4D1F7879587E1BE6A11C315C319E05C8F1EB1949402BCDA767097AB7031F8602B05000208E7AA60502970EA19C30CF83280D348A5005F2871B9F405B007B1F332F8982F3000ED33CCBBCFC0003B480AB471E35B2964C289E4F604039E717A06EC43B7E863FC3F81E1F005181EBF98317B02D88688B14D15E31BE1C3D6F340029F8075FD2593A50CCF652371462225A50013270FBC14E06766B092E76090803606E14B01010208DF6A913FC09ED8994F3F194E834B815348BD0116A0399C368473FFF567F05C0F3E5C02B4CD1CDCC87B008978D0EC1D29B99E01CB281E313B7DD05BFEA05C0F8AF81F2CD0E327B1ADE3C7359E8FEB34301CD5022352830F1A397F812DFC87CA850CD70D6630FC42C9F50C18118B9A10088D1CA2AAF9FCF92BC36B016D480F0B98FB6D04C06D012EE419428000C29700C08DC1A79F185682BCF217D818FC79650352F9E24538973E7E07516B5EC0C0A00FCBF5C0BAFE5B3D64E60EDF9A797C18EB942E03EE153CC891F90B18E95F8191FF97094B4EC6B6A297017B1BE0FF7FEC2DFCFFFFF10E067EE63762B86ABC84E1A50CAE5C8FCA471EEDC39DEB51F97FFFFE65B87DFB21C3F5EBF7187E092933FC67856C2A0136066DD1978A010410138176F66FEB850C0B618DC16F0791BA84DC76906E21AE3D7B9F7F20D42AD8232DE5990F9DBC213FFEB12FDFFA8F7B34103669F31D18F13F59B177CFFEE3199AFD8FAD5D80A5AB89AB7108CDF54F940A196EE94F67F805EED733E2C8F50C28753FA9A5C0EBD71F182E5EBCC5F0FEFD2706C6DFDF19584F02ABEE5FDFC0CE7FF58BE12A03E2FC00B046800022B46E1C540AFC78F685613EF8C4CEDB07C025011CF087E32EFEBFFCC0CC05A0153BFFBF305016FBB8A674F1CC078072FB770E08CD80A781C7C880A37EC76237AE36079659BE2FC0BAFEA6D1128657D21128F53ABE061C6AC4132E057EFDFA0DCCF1F719EEDF7F022E01985FDF6260DFD1C2C07CEF38A423F697E1F5DEF7E0CB257E3320CE5D660008204209003C41B4FF01C31298B7BE6C6F444A00DEB88B7F1626DCF3239462062C4534B6153FE0EE1D30C7FF60C76C40FE67C03F29C480A5E1F69FD0C81F6A2F0094EB9F291632DCD545CDF59811C98025E7632F05B0E5FA172FDE325CB972075CE783723DFB91E90CEC7BFB8055DD5BB0533EFE65B8BEFE0D43D5F35FE02377910FDF6600082062CE09FC5BBC87E1969B22C37A114E86C0EF973730F07E875E8DC2C40BEC110013C1C72D98BA0410975C80E61328BAE016E72010B66A0029E27EB341FAF738876D719CFD82B33AC0362E80A5FD00ECE27DE5336278A25CCBF08B1D5A4D825BEFFFD13204FA823FE84D515045E00B83E06C06F4E5C10C3F7FFE66B87BF731C3C78F5FC0EA989F5E60603D3E9FE13FA8C807B5E2FF317CBBFD8D61CDB2970C2B416B93413504283D30400E99002700800022260180570BDD7DCFB04C189800FE7FFBC0F0FDE402066E07E82C9D20B031F37E13DEAE3AC38B8B0C8C1A01543C5409CBA24FF4C59B3F39A0D74DFCC7B249034BBF1D47570D6BA39301C72110E05CCFCBF05A2685E1AD4438F47027D861DC90C847CDCDFFB12D11465287D0F31F2DB1BD7CF996E1F1E3970C7FFEFC01E77A96637319981E9F879F79FCF617C35960C4F7BFFC053E41F43D34017C6240DCC100060001C442647EFB13B08661DF832C86A7ACCC0CD29FF74F4424000E7560C7C20874F92DA6467E4E86FFEFBF03AD87B61B18B9A85402E05BBE0D2AE0D8B137CC709DDC81AF01F79F0175EF1EC6FC012262BE0173FD732548AE874DF73342F5429C8C63F09F885200966E20B9FE09C3A74F905C0F8A746660E4FFFF0969E88172FDE94F0CD3B6BC61D8CB00393DFC1D34D77F65C0720B0B4000117B5630B84B78FF2343BFAA2043CF9FB70F18BE5FDAC0C0A907DD9021E007B92C0963629A1592009E5F804E26293250ED605DACEBF4404D1C58E4FFC31EA9FF896898603BE401CFBABD7FC0BAFEAD5432C33B7158AE87C83332228C82E568CC3A1C7F29809C104075FD93272F197EFF06E6FA5FDF19988ECE62607C740EEEFD37BF19CE2E7D0ECEF54FA1110FCAF9A0B367BE2335FE50004000319150EBFE725CCAB0E8CF7F4897F0F33EA42E212801B04A623604F939203D30580200FB448C0A0D416C7301A0720A18F9FF18712FDCC43979836D860FCBC20E46CCA2FF3B9F21C323DDC50C1F2423B0B4D6195146EE70CFFF628E03209B036AE1DFB8F10058E4BF00B6F0FF0173FD3906E67545F0C807E5FAE31F187AFA1E32D40023FF0E501BA8D87F01C46FD11B7DE800208048392E1EDC257CF19961BE240F43DE0F6097105412C0AF67118E015ADB89AA039A00C089E0FE01C8010EA025E6A0235AA9D5108445D61F3648E433FCC3334FCF80E52008061C43CCF80787FE31F332BC974962F820168694EB91733E2345A5004CCFB3676F8039FF0D38D7337C79C3C074623103C3C333F0BAFE0DB0AE5F02CCF52F7E129FEB9101400091724A18B83178E031B44B08B4FCC356A42EA1A03F3072B951C6029880AD465850C24B01D0E2528AC600B00CFFFE0546FE7F263C753B039EC51C84267D304B8E1FBC860CCFB417327C0236F4700DE8602B05F0AF024195FFFEFD27C3B56BF78109E03538D7335EDBC1C0B8B1121CF9602FFF67F8760C98EB7B1E30D400239FA45C8F0C000288D463E2FE56EC67B8F5E61BC37AF098C0F1050CFFBE43978C3183BA844EA811C00A349E9315E2E07B0711ED008AC70090EAE97FCC10FC1FDBB66D6C1331384A068C133D3067F5FE3103EB7A851A8697EA5381B58D24DEB178E46E1BA24F8F7BCC1F99FFFCF96BF030EEF7EF3F1818BF007B6EDB9A18184E2C8234F480E63CFCCEB063F24386944DAF18B642231E845F411B7B3FA0AD7CA21A5B0001446A02009702675E30CC8085D5C7BD486D01896C8C45208C225C90121AB6C298E21200B9EB06DA81C386B9AEFF3FAE7E3B9EB580FFF18D02024395DF96E185EE5A866F225E04C7E2D14B0158E22034A083C8F56F207257B631FC5F57CEF0FFD935D868DE9B3D6F181AA63E62E87FFE137CDB2A682BE07352733D3200082026326ADE3FE93B184E7FFE059925FCB07702C3BF6FD052804D9A814128002590994438214CA01A7035005A4C42494310B995FE978DC0916F04A66F314A06CCE21F94EBDF29B533BC55EE00B3499991C33EB2872BD7BF050FE5FEF8F1131895AF19FE6DAE67F8770C58C2FEFC0A76E20360AE9FFA90216FD71B86FD48B9FE3539B91E1900041039B74D82BB84773F30CC06738011FBE502D22CA170204A026016E680072B7C6919AB2E858D3F70DF0BFBD02CB67DFC8C783678E0593BF053C096E18DF61A304D6CAE471DD685D5FFB84B0150AEBF7EFD01B8A10792FF7B6933C3DFD5C50CFF9E5D053BE5E73F8637BB81B97ECA0370AE07D5F54FA175FD3B68AEFFCD40C1D9BD0001C4446614FC0E5CC7B0FEC71F602A048D3A6D466A0CF29A3330F098C21B828C5CCC0C8C9C2C906AE0EE412A5503A0A95C6606AC9B3C701EFA8863DF1F321B366803CCE91F15DA193E28B633FC67E1255874E39AD2C53DBE8FE8D783BA77A05CFFFFF32B863F1B6B18FE1E9987C8F5DF8075FD0386BC9DAF3172FD2706E2AFD6C50B000288894C7D208BBF3FF8C8D00F4E0D6F1E307C3A86B460442A0F6B35F01B5602B05950D610FCC78ABBA58FF5806706069C9741A025845FC0BAFE9DD66A865FD05C8F3E23474ADD8FAB14F8FEFD1738E241C53E78E5CD854D0CBF561430FC7D72055ED7EF7ACDD030E93E30D7FF40C9F5EFA1237A14E57A64001040E42600F09231EF350C0B7FFE01A74A860F4791978C014B0136297824B14842BA83A01E03B81A00B503E0A382A4E67E661CFBFB712C17C738D10BDBC20D48AEFFA2D0C6F045B10DCCC6B5268FF8BA1F7B2900CAF5376F3E04E67A607B0D98EB7FAEAB62F87D6836C3FF1F885C0F8CF8BC1DAF6897EB910140003151A017D4E8F8FEE63BC31A90C3BFDD38C0F0EDE60184AC743EBC27C022CC8E58207C6523A21D4072FC3342EA7E5CC53ACE5543FF19F09D0BF09BCF86E193C62A86DFFC7618F534A9AB71709502A05C0F8A78D0240E88FFFBFC0686EFCB7280B9FE32D8553F8075FDCE570C0D13EE31F43FA371AE4706000144490200AF15987389612A6C78F8F546A4B6806808A457006A0700E38C450492087EDF41AA06C8C9FD384FEEC673B2378ECB1FFE3373337C576805E236A00379B1CECB93BB260FB91478F9F21DC3AD5B8FC0B9FEFFA7570CDF579732FC3C30139EEBEF7F65D831E12E43DE763AE57A640010404C14EAFFB3F41AB054FBC2B000E491AFC0520084E140B6001E31AC12906AE0F7D30B905545E09E003709F18FDEF0C333CDCF8067A93854EC2F30D77F535FC5F087CF96E0583C29A500B21E50E3EED6ADC7E00400E2FF3ABB9EE1EBA24C863F8F2F41364901EBFA1D2F191AFAEFD237D723038000A23401800786E65E6698F2F73F6447F1AB0D48A5805828B414F8CFC02681540D5C86761BD974C9C8FD045AF788D54E58CD01D5EFBFE45B197ECAC3723D23CE3579E494023035A048BF7DFB31A485FFE925C39715450CDFF74F03B6F02147ECDD07D6F5FD7718F2B6BDA47FAE47060001C4440533FE2CBF0E2D05401D53F45240AE08DC0E60E2640262664802B803ED0EB259129FFB1998F09CDA85A5958FBE440C3421C56BC3F04B7D2530F7DB12188B67C0B27C8B502980C8F5B76F3F6178F5EA3DD8CC9F67D6327C9A9FCAF0FBD145C84932C05CBFED054343EF6D86FEA70394EB91014000917B7D3C3A608FD46090AF306338031A09E0D6706050AE443A4FF0A419304D3F66F87CF913C3D73B90850BE21DEF19983880DDB9D72144180F3DFD1C360506BBB6FD1F321BE9AAF67F4834F8A2286E86BF52150C7F79ADA1A5D07F248C9BFFEFDF7FF87C3C8C8D5D0F440C94EB5FBC7807E6FF79FF8CE1D3A636865F0F2FC04BBEBB5F18762C7FC2B0E8E9778C993BD8481E5D2F6302B913208098A864D69FE537185E3CFFCA001E0CF872FD0018C3817C313804386511D3C33F2E6D807407599408A55148E4639CC58FE73838A4AAE13F30D7FF05E6FA7F7CB6440EE8E02B0518B0D6FD3F7E4056E980723D48FCFBA9550CEFE724C1231F94EBB702737D0F28D77F1FF85C8F0C0002885A09003C3CBCE00AB02D00DD43F0623D525B40229C8181DF9281858F19D82380AC6FFB711B5A0D800F9DC0B7C18409CB26CDFF0CF8177682B6C2F030FC976D65F82FD70A9EA92434168F7D4D3EA1D13EC83AFCBB779F825BF8FF3EBE6078BF2887E1F3AE490CFF7E40EAFA7BC0167ECF2D86BC2DCF07B6AEC705000288898A66FD59711358027E039602A0535FAEA195028A2590E3AD25D8C0BEFD7611DA1064B724C28978CEF1C5B6F50B98EB1954573230405BF8A8AB71089702E80D3F6C75FDCF9FBF18EEDD7B064E0020FEB793AB18DECC8C63F8F500727DDDF73F0C6FB63C6368E8BA39F8723D320008206A26007029B0F02AA447001278303311212B600D2C05AC18382421A75B826610C189007C0299128ED61F138E85190CD8176DB002CD926E66609069019ACB833547133BA083DE03404E40A04807453E2811FCFDF09CE1ED824C868FDBFB19FE7DFF02AFEBBB6F32E46D1EA4B91E190004101395CDFBB312580A3CFAC43009DCDA7FFD80E1CD21A43902B516064E09167835F0EDE24644298075EA9709AD94C7B24207C6160C06B635663130F0D8608D34724B01E4C4018AF0070F9E33BC7903C9F55F8E2D6778352D9AE1E7FDB360D780EAFACDC05CDF7983A1FFC920CEF5C8002080A8D50B40E91180F2FBB1088673AC8C0C526C220A0CDA1DE71998B9A0670F5FC96278BB652EC3E787BFC0C7BCCAF7BD074D133230BCC9C4E23A76D47C026FD9FF47B4F499C51818444B191838F5F1B67609B5F8B1F1FFFD43F40040B91ED2B50366E1A7D719DE6DEE61F87EEF0C3C3D5EFFC8B076C5238635C0887F36185AF8C4F6020002888906E68252F9B7071F192680E7B381A5C08B6D48AB8634DA19386584C1F10A5A4BF015B49680451998D9C5B18CFCE1B8CD0B96EBF90381C5FD0CBC918F6FBA969852E0DBB79F0CF7EF3F6378FBF61398FF61CF0C86E79322187E00231FE484CFBF191E2D79C050D2739361E650C9F5C8002080689100C0A38351DB19167CF9C570061455CFB74D60F8FB15BA6A88859F81CB22175E0D7CB900AD0638AC50CB7FF80A5A2C4BB440B95EBC9B81412813DCDA2758CCE159B481AB2DF00F58BA80723D681DFE9F3F7F197E3DBFC9F06C6218C3FBDDD311B9FE13C3DAFC730CD9FB5F319C8446FCA0ADEB7101800062A291B9A022EFDBB5770C3DE08520C0C8BFBFB01021AB52C9C0A5A8044900E7A1BD014E374CA7619BD4E101E67AC969C004A3476A6D477429005AA5F3E8D14B86F7EFBF80C5DEED9AC6F0A43F98E1E7B31B60F9CF7F181E2D06E6FAAEEBE013549E4323FF2552AEFF3598733D320008205A2500F04C61CE7E863D1F7F32EC05CF141E58C0F0F12AA25BC86D5B0BAF06C0898015560DFC874CFBA2CFF2318B02EBFA0E60632F9DA85C4F4E2900CAF56FDE7C6478FAF43530D7FF03E6FA1B0C8F7B8318DEED9C0A77C635605D9F7B86217BDF4B70AE7F064D006FD072FD90010001C44443B3C1EB05B63D60A88575D11FAF460C0E719BC6831B81E085A5B0C5245C6ED85BFFDC01C0C89F0A6C5EEA51E8247CB9FE17B0B87FCDF0E1C357706278BB7332C3C3EE006083EF06E4345B605DBFE83E4349E735945C0F5B9BF70D5AD7FF671862002080689900C0DBC9265E60B8FEF42BC35470445F39C0F0F200A25BC863100009DC731BC02501380120D7F7A0BA5E1898EBF9D3209B4E288D7E2CB9FEFFFF7FE006DEB3676F197EFF06E6FA67D719EE77FA31BCD936199E0641B93EE73443F6DE1718B91ED4CAFF39D4723D32000820261A9B0FDE4EB6EC26C36460D882AE4F62B837AF10DC26000141977C78747F3EB70172E40C2BF40874DE6860436F0115723DEE5200347CFBF4E91B864F9FBE8213C59BED1319EE76F8307C7F721D721AFC2F86470BEF3294B45F1D5EB91E19000410AD1300780DC8FABB0CCF810DC26610EF3730F29F6E81740BD9650DC07B0B41A5C0FB23D06A802F1E58DC031B79BC31B419F8806EBD7EF7EE13C3F3E7EFC0C7A9FC78728DE16E9B17C3ABAD13E0CD8EAB1F18D6669D62C8DE330C733D320008205A0C046103A0336AF9B7F9326CE36261300105B0C5CCFB0C1C620A0CAF363632BC5ADF001ED7D1E8BDCF001A38A22500E5FAD7AF3F32FCFAF5075CFC3FDFD407C6B046DE4760AE5FFB8861D2EEE70C171910072B7C44CAF1C322E26103410001C44427BBC0DDC2532F810D4268B1707D12649E40D03A1E5E86BE3BBC90660E008DE6BD7FFF99E1E5CBF7D05C7F95E166B33BC38BCD7DF0A20A94EB334E326403237F58E77A64001040F44A00E0C1A186530CA79E7D61980A0AEDF7C006E1F37D0B185881399E4BDD015C02BC459E37A022009DAA015AA8F1F9F37730FFF9A65E86EB8DAE0CDF1E5D85344281B97EDE6D869296CBC3B7AEC705000288898E768172CFB765B719A6FC0496BCA080BF3507D22014B489879CDCFAFA01C367E429642AE4FA0F1FBE30BC7AF5019CEBBF3DBAC270ADC185E1D9C61E78917FE53DC3DAD4E3232BD723038000A26702003708B73C607876EB3D431348E0D7970F0C77973532F01B07303041278BDE1E5A48B55C0F8AF8AF5F7F801B7EA048BFD6E00C4E04FFA163F87381B9BEF9D2C8CBF5C8002080E8D508C468106EF0803608811C93B6FD0C9F8F2D044F1D838E9FD39B741F317B4846AE071D9B061AD801F90D14E1F7E6E4327C7B78053E9108CAF58D17C0CBD76087287D60409DB91BF6110F6B04020410D300D80B6E106E7FCC90FFE73FC3175084DC9855C8206417CF009B37787F7A0399B9FE17780207D4D2FFFBED23C3E3E5350C57EB1D21B99E0192EBE7DC62280146FE88CEF5C8002080062201801B8433AF325C7FF98D015CDE7FBA7781E1E5B983E02E2028113C479E3E26322583EA7AD0302E88FDF9C631866BC0887FB96B26643712B0FABFFC8E616DE26186EC9D4F47665D8F0B0004D0405401B0E138F0C291F5EE0C07D89919D44111A51956C0F06AC7047051ADD7799E815BC180A8BAFE13E8DAB97FFFC0B9FEC9FA4E86973B67200E4CFCC1707DE60D8649A7DF80AF737E0FC5C3B25F4F4E150010404C0365370364CAF4CBD1170CC5B0D1B7A7670EC027019F6D9D48D0F1A088076110FBCBCDA30C576BEDE1910FCAF5A75E332C4A39C250028CFC4B0CA8F3F5233AD7230380001AA8120006403B3EF866DB31344972316481229F9B135834B030807B0526D3EE33B0700B60CDF55FBF7E0737F8FE7CFDC8F0747D07C38B9DD31147A78172FD758689C004709B01F598D4EF0C4368AE9E1E25004000310DB01BC00DC2B5F71926FFFCCBF01CBC4DFA17B4BF086C0CBE3DB501AD85FF8FE1F3E76FE0C80777E56E1C01E67A5B70E4C3EAFA93AF1816251F62280146FE65A4BAFE2D52AE1F8D7C24001040035D02800068D727779D11838F8908C352502E16E0814CDA80DA00C6BDE7E1B91ED2B5FB07CEF577676530BC3FB31525D7CF00E67A600218CDF52494000001341812006C6C806F8E1D439F083B432C3BB062E0841EF1AFDBB097815DD912BC2E0F04DE9DD9C2700F18F9BF81890096EB0F3D6758D87799610B52230FD4AFFFC23044D6E50D64020008209641E2167055B0F22E437BAA3A830B30B62439A12BC29FEC99CFA0A060CEF0EBCD2386874BCAC1090096EB9F7D6538DB7A9EA1FFC167F891E8EF47733D69002080064B0900AF0A6A0C187C8C8419967271005B882C9088960CAC6278B17D2A4AAEDFF69861DA8C6B2847A283E8AF48ADFBD15C4F440900104083290130C27A0533AD19FAC438196279916EAB87E5FAFB9F190E755D609809CDF5241F8E3C0A50130040000DA60400024CD00122C129160CDD0A020C516CD013E13EFE6478B8E521C3DC45B7C023791F9006746047A48EE67A32120040000DB60400AB0A40170E81060084402502B491F80F9ACB3F43EB7958AEFF339AEBC94F0000013418130008B0404B025042805D75FA1F5AC4FF402AEE47733D8509002080066B028055072CD01201E6C0BF50FC6F34D7532701000410E3FFFFA319682403800003004042FE192CC3EC400000000049454E44AE426082','folder_yellow.png','folder yellow','image/png',0);
INSERT INTO "kexi__blobs" VALUES(6,X'89504E470D0A1A0A0000000D4948445200000040000000400806000000AA6971DE0000000467414D410000D6D8D44F58320000001974455874536F6674776172650041646F626520496D616765526561647971C9653C0000198D4944415478DAED5B678C1CE7797EA76E2FB77BFDC863BBA34452128F9DB444CA8A154782E5A6207612C5254E641836D224FFB09318F9E3048813FF082040708240361077C5708B1D4B242536B1533CF2588EE4B5BDB6B777B77BDB7767A7E4F9DE9DD5DE99B10C1B8243C71CEAE5ECCC4EF99EB73C6F594A721C877E93371972570177157057017715F01BBBA9FFF0E2303D7560351FD42C9B4452302D876C7C68EC2DBB2EB64DD8DB1082B8DF5910EC6DBE86F0D916D7F17973C53584F37C2F6F12A46ADAFC4ED9D4C9512C722408BE51F0A54F5768F926416C48BE64625D58A3842B6D12CFF7ABB2D4AF6BF2DE96A03ED013F3EEF3EA72F7F5A9C2D7B265F3594596AC375500DDF91B1B45A4EB1A145AADD92DB22C6DF569CAAEB690BEBDB3C5B3A7BBC5DBD31AD6F59057257C47BA2A537BD8F317DF3F9BFC366E3941D2AF990280B5E1758A24510700EDF0EBEAAEF6887707ACBB331EF2B4B54774C9EF51D863F21593021E99E6F306DD9C29D2FA4E3FF5B6F908DFBFBD54354FC824DDB90A709601361DDB2791BCDEA7CB7BE221FD81F6886737F6F775443DA1684023AF26132EA3B221420982CF13A9122516CAB46D7D9826172B54A85A846B99DCD6B5FB1E140AD1D43B4801ECCEE28FCD1C10C6F1FD9AE6EC8E05F5EDED91E09EB688DEBBBAD5E789857476E5C63DCC49405F03EA4AADAE004596091EC1C713F3159A4D57696377804CF04A16FC82E7EC4ACC97E350C0E2FF990278F136C7AF6C5B769B2CCBDB11BF3BE3617D7B4B40DF1DF26B9D706739E453616105A0602D89EF61208D4ADDDDF1F7E29289C52AA572558A057492707C73B6C81E22B6D94C955AE005506A2BC0EF81AE7E2449BF1A0534015BB607BB5578F19E904FDB110D6A3B6321ED019053146C4D3E8F0B165BCDB4015A660B174B25F27A7482A25684C9F2035992004EA3E45295227E95E6730A654B16F57505E88BDF1FA3CE884EBFBFBF8B3674F829ECD30E640A061420BDF50A6802E69419C0EE1E80DA1BF0A803119FBA2F1AD4D7C395FD619FCA69ED7FE322C374186C2AB540B33333E4F3EAB476DD3A9215101CC7CB0AEC6C6D19826C8010A92B429061088AF0207F6E5A15A457AE2C52E8BC4A9F7ABC97DAA39EFD0BB92A5E21D96F850218B05B1BB4E270ABA248BB833E7D7BC0A3EC8E04B42E585A0B78B1188EDFDB37DB8158166596B2945E5CA472A54AE148144AB4C8340D8AB57651095E10D175A295F81B075C770CCF16A004874A8645EB3B7C94CA1A746DBA481BBBFC386FD1FE4D31AA1A0E21550E007C2F118DFF520A70016B906E2C7E8FAA483B025E6527D2D256B8763CE467EBBE19D332E86AB54A1680D76A35CAE5720C52D3345AD7B79EAE0E5DA1B6B60ECA66B3948562BA7B569144122B9A2467050FE09095BB6D6D9816F235BA0196BFB72748AD210E09B67EB16CD29EBE2855A088167820AE7FC8B09C7159FA790A68A6253F5EDE87FD5E001EF06AEA5EAFAE6C0C7A9580DFAB32D9A8CACFACA2D93DA7D3153073991E581322A432326B268D8C8E900ECBAE5BB71E60976871314DE1809F5A62AD542E9729128D92CFE7A3999969EAECEC22AFD7CBE09B9ED0DC07BD2AE2DE64251D01E863D7329429D6E84F1F5DCDA123C2AE889488E248A4C58792D9EA7F803B6E5780FB501D77EDC5C3F640F3DB3C282B3D9ADA8314A30BEB023C13D6CFDA4A78D1D442854653451A9B2BD13872F312169387253EF1D85A3AB0394E7E002D158B94CFE7A8BBBB9B02A1088D8D4FD04C728132E945EA5DBB817CA5002D2D65A8B5AD9D3428CABE9D037893DC4F61BF4615C326F00DA7C72B93798A81FD5116C3DB6C41AE9C2EDB229E03B3998A0EF2306E5300706D85FCBB4753B645344D462AE207FC2CC08E5B8F8FCF97045896043EA31A1396E7FBF02CD21499F338BE170AE0F3B1589CEBF9B1B109F2F903140C85696E2EC9E003F81C0517783CABB87740CA6C589E45AA7B261F21A4980035A59E3215ECEF87A76D591D74B34AF3DE72CD4678E8FDB866130E076F530058FA79E4CB1D4168F1F6AD5EA16561CD11615988B0F2240017CA35F1700E051F00C36BC0C61A69B2CC0B8232F8DC58AAC41EA2421991963899E081DE75ADEC0D5E9F1FAC2E0B2B313F38D8E37885B91DF7C30A1E60D00A2DE6CA647216E10B60E026E7E023F348A9021E086A2A3C793F2AC8C19FB6AB8A936B007205E9CD814C469245B6F2284027D3150661980022B35AC0B01605548B74095697742CDE43BAC2FC50D77CD5610F582AD4687AB1C2B57948B0BDC3D6457CFBB83B843048976C792FDD069C5133209C6065E6F2506C3E83B2187C108EE2B40D698447F39E9A5010F62898F64F56CACFC98AB45201E373C533D952ED7D8827713D5BFC4B2F8D735321C9CCB8EC6A7E0F2CAC398839836AD532BDADDBA6BD6B14928D259A4FE7A818BA87AEA42CB29510C09B752BC13446CD6245AE6B477AB22D976459DCCF2E7871E4D6141C7E8D2C2040E1619669216596C9EFF5D0C4CC1CD70D45A4D0AAE3A560A4854B6B8941AF0C58D32DA1C1030FA2670889085EA1009493479399CAFBD052B26655805D870A6A0A56ABB797AC55B65415716E558BF4E466A2776C0E9325A9586484366C50A8904D9377E13A5D32B7D17CC5C79E807BF97978B1F09E26681B229EF986321825BBAE61C162664D200121A6913D6AFCEE4C264381809F5D3F932B512018A6484CA7744926DBB45CE04DBEB0EB7BFEBB0883200CBA91AAB7E3FB23CB752417CBB563E3A9A2652E0B837BBA838494C715163A3306A3C936172D039D16FDD6E60819A4912DA92CE9A51C158A65341F5EEA285C20AB66F052148909114D0A7769F5F864F042A4A67B4353954A856A009E981847AD6022154E511EF5C2126A83B9D41CF7F9ED1D9D246B7EEAE8ECA1CE9ED5D8778158A364D573A51B4A107253A7CD82E7725A94FCBA72005FAF0C01DC30343A571C0559F47BDD29CC86CE00A7184DE1C683AB34DB04A9218B6CEF456E96141436261452A06C264D2DF118F5DF7B2F99E52E5A9C7F89AE2CA4104ABD00EF30A9813FB848598F3030393D996C5559564461C44552B158A04221C7FC50AD999C31D2E934ADEEECA61C6A06BFDF87D5FA61759FCB74EC95BC46A7497CBC0193F8CC242CC83D08436AF06E10FEFE6CB948F2B21A46858B56C6E60AC77225B31F652C6B3A863D464B5C58E88AC439D58451FDBA43F1A00A66ADB26BB5B5775067770FB3B96D995CD3AF5FDB43DDB90C8D5B3D00AF719E45DBCB21B51EA16500702A39438AAA3178D405AC085485488D417C56B9BD6D8F47513B84B827080583942B1AB8DEC2339B3332D9AE839497F10AB071A3058B134A400E5FFC6143A220DB056FEF20F0FC1B0A100F4CE78DE353E9D2C77AE0C25000CB9A761F15264C7E98050096A1501900D3258756B77928D0D24E8EAC0966765F5E9FF7B575ADA687AA199A1D154A6285B2F6C133ECFE1E5DE36A5051113EA2F68F465103844852BD38E7111EC3F19B2B5541BAE0856A05C5D21C9E8DD150570FBF47A26675283C4081803CB9218A043C083D1B6BAEC1A3CAF09C005F6F92231410C55AF6611DDF6D2852C579B8A8796C2C59A8ECEC8B79355733BDAD7EBA851EDB27AA2A58C80121E6350FA5F2360078B0903A533B583067277800107005E7F779C8C642819D455764A16432A0C800DE100E0501C8A2AECE0D6C1D55115E65D0E252010B3711FB590E93684047CF50A40294110A474882011C3CC3E1D537ABC25840852708855669616E920AF90299E0A152B9449BEFDB8AF2DACFEB41F90C62D7F6674B061420B91C8007E060F4E66C7EA86C983B3D5A7DC0B02AEE25BF5B06DB36ECEC58140D7991CA4A9C9E700D8397640D8B2A9383EF65B788194A22FF3A8A707FCE02F0298481432847A9BF4B019828C77C2693E67A2027001B551E75CD2F2CA01AF4E2BD1A154D15E281E5DB280C05D4AA553775DA08191D1C81321A22F27C2BC2B198CF73598D43648910A5F1D96C641F88CE3CA01D98CE5414ACCB6205083032308EA58A4773A5DACE08D703128996163339EEA8540D60249CB77D183ED4E856AA42F787E0666C7A9B6C68DBB5098FD2934595ADCAEEE94E70C09994640504417A655A5C98C722837463F82AC078D84AB54A89622D712CD6242D10A3424D274F042C8FEF0BF084A585A4A81AD94B1445E6EAB158C8238394C983FB3DF0CC58BC8DE26DEDF8CE84227DFC5C376CB8754628DD8F2AB5BF6649D7EB1E60738B898ACD38866CF0CC6AB83E365E348A079A4997391D8A04A1C9E2BC4327C78A140C60FABADACF459103D7AB573126198641154B66CF5115EE299827C0472213709DAEE91E26B742218FD4D685CF1A056131902893217A7B54A3E00C0BC40720008EF71461AD320F4CC81135093C32DE4A417846B425CEF7AAAA0AD1709FC5CF096038C22DB86573FF922D5BA46B12A245DA8FD3D72589BBC1BA3B57AAD6C99BD3B9CCFE4D6D2D8D691466EE584C95C13840EFD1246E42B2007669DAA0AE589EE45A4170AC9B704B1CB39A1A22C5212640B8273F1F5C0D263729573639165B5A5A392ED114892CC00B0595513697A7C4E42287886D2345228EBD81105BBD241A2E2877EDFA7EE6129F4763D06C61B70486E5DD4992046E33A850B1380D9B36E3E4B923B2C37E78FBBF616D1C028D61E3DCB5A9DC3910E26F071595171E0FF264966FE2F242E3CF1099B2850ABD76798AB67554D885F1529EE8845ADAA8272ED1EC540DB584CE9ED45002C65F08218353940EF7AC1A655A4091C3EA91713F404E4C276909842989AED21FA63044915528212CC0719628180087D8568A622C66B83CC39D21AFA3C6B546E397297E7FA3AFE08C81EEF0411C78C15115B9C110302ABABCE25194C638E607B1E5315DE5C5FB0470F871002E188BF858EBCFFDD70815284C922740B61620C717A740AC13AD29C2C82C513A0B52CA16202566720305CE5CA6EC7285C923B1F9F914E5961650ECE4686636457A204EBA5794BC606CA4DA60B483FCE11814A2808B3C5C3F38C43D0367953C2C9C41C3952E1842B876295658010C4D6E2027F67528D483B961640DC06FE11050BCC1FA978865A4876337A6724E5F5750B2EB4AC10D2A938747971B9503BBECF54486C6667394810E7B7B3BB97CC5F338FE3674C5E8A987C5C8AA0A4FE1F4C78429943B91CCD3AEFE1628959F03220C901F56362C853AA35DB450B028A285D9A5011A404D37DD49B78DD91AA7964F7C57F60492F852547BE288D3E2D4C41452A1A2F83CA103F0F6F38A55AB527AEC22055A5711E9A174D42F7F64577F3C2C1EDA7829AA446E8C149CD3E109F34B25FACE91613271EF930FDF4B51BFCA31CCCD0CBAB38285148A185DD311A2BE5511EAEB0963641DA636BF040F00D34703E0018D53A017E0C1D7A4EA1A328E8398653767E1FA7E3930A709CCE741D8349B9E266849720951E3745D2A1468627C84AE0D5D64999F1DA7DE9E7692BCF152B6687C53BDF9EA57F8E6D1D7BE459B9EF874FE46D77B4E142BE60730F0E4161503D0463527DC9EBDE295F313A483E5FEF6A30F23FF76D39255E3F392E00E4804D7E26201822198A68D5465D09AEE282BB052AE90D2E24397463CB0700D05B2AA71E8AD44D4A8EF9BFB9610C2C3A7B0C2B92073BD2953B238E472D9454AA2994A23D5964A08D25004A9B193EED93240BDA82944A61B9AC8BC8D88A22AB99B51CAD2E0B7FF0EA4A21C9F7DF7BD1F88C05A500093A05F9745CCF1F477686C1E29A9429FFBD03EEA6E0B71DEF7F9FDDCFB2FEB46DCE186FD86F26C5DE529527B5B84E0DB625EC8E2FE10C4318BFC0C2C7C7FC3D2B78DC6D1A700BCCA55A1AE68F0BC1A5B39393D4EA39831265239D132736ADCD07F2FC5DB3B913AFDEC1535F65287FB114CC13A409EBB1A0A905830931ABB7CFCE4B5A9A7CD4DDDADF0EB3A6B7208915DAFE581F3A38F6F41DD5DAFDB03B0447D13809B7BDB252ADBAA7B02381BB1AE8A05F0B4165E46AADCECE2CAE019FC473CE66B58BDE9F3ACB4D6B04F4C78B9F0594ACFD1546204A5EF2C4ADF2C852311EA468BDC7FDF2E52BC518E7B94D36204C7844B16BFABF16F1B58897E8CC91AAB571B525A9C2DF6ED79D7930F0FAC6E951810577BDC8D212C78011878227E7576E7C622DDB11AB4EB3002545C6C25CB915C576585C0CD6DA4BF7AD9CC4EC397B3AD79B26B4158919C3A65C1FA9CD33D54A57C6A82AE5E3A4743174FD3F4E408DEAFD3FABE8DB4EB6D0FD37D037B28D0B68E477847919E11759CC156FCBAE496C3086B2ED05093480D0F90211EDE5BA5F2F9C1A1D3F9F2BE7B223EEEF41868BE582194057C23CA507629F75F8134C88717AAAA0C98C1D92E7289F82B00B7F99CAA365C1B209D7AA8F91022DD10440988D046785450E66661E16932B233E49497C81708C2A5BB68E3A6FBA9137302E44B5AC89974FCEAA879E2DC4BF3678E1F9EB831743E512995EC8E5DBF7BE0EF9F7DBA1BBF1772F5290903B8421031FD0A07BC9B1B0AB0210AC42B967DFDFC91F3A3F31FFAF08E355E0190ABAEC9B90C554B5EEAEB6D65ABE299FC304D55780FFC0C8E3137598B8B10CED9359B4FA9DCBA8A2EB25180C95C1663E6C093E26C6A06D61DA7B985341A2F85DAE22DB47E633FB576AC4208855040E9940371BEF8EAA5F2F153A712E74EBC3A3979E3C2B8B3349E154EE4E2B0E64ED9BEA9850FBEB7BF3B84F77286E277F9750D9C00C5A188BA3E3AF572430196AB040D124E8E5E1A1CBC3155D9B1A6CFDB20E3ADFD5DECB6A1905F581BD29CE7B9BCC5A4D53CE0CF8224B90EC0C6E0D9B5A17DA02603649ACDA601780215E12CBAC20CF94158B1D60EDAB0F15E0A45DBB8CAC426629995562C95EDCFFECDE78EFEE81B2F0C92952BBBEBF6B8E28754F1127DE7238FAFDBB2B68D170FBEE25F9C4BF0C0E1E905FACA0FAF954E1E3B7C70E8D56F3EA32E4B38861B0ADEEAECD5F973976F5DFFF0A37D03926BC1B55D51C6C5BFD9BB181B9BD4C8D1025F737ECF210261564677C8338072A94CF3A924CD4C275009CE8B3698197B55EF1AB4BD0F21A38470BDC6806D5B746FA63BF145E8E07C32992CFFF8ABFF72D67D85CF5D33377B909935F70CF8FEFCAFFFF1E38F3DFAC83A41DA8B28C68646A7E9E4B98B8BE78E1FBE71F5D299D7CDD92B27E12C878928A95273335C0963E99533278F0CA63FFED8405B00470C1CB1DEACC918A96B68EC5903EE79D6089F535419000C1E704C03F03C183BBB9411FD3EACDC4603DB77F35ED77D7C0FAA496E906CDC23D9DC91F25F8E78874BC69D1D1DDEF7FCDE1F6DF9C90FBE730DB3BD2A0CB4502A1646E189A344CA6CD7239FFAF2DACDBBD6BD7CEAB2FDDAE9B3D3678E1D1A9EB876EE756769EC02110D4366204BEC29BCDC955B0BA417E2B4DEFFF83BBFF58DAFFDD3239BA30C9EADFA26BFD830632BF534932F082BA760E5495A8495AB15F1C3670B4F757B7A7AF967304952D9C2887D06ED38CDE7E3881C0BC28A6C4E7A71394920E02A08657A2A318D2189C7B0A5C93FF98327DE317C65308B6F35EA1C785AD57CDBCCF99B09AACC5F21A29B902424C7A07FCEAFC345D70B628BB7CEDFBA70752CF7C8E66D616A6E8D18777980871D0CBA08024B2412FCCBEE5226CDFD781CC389AD5BB751675737185767865ACCC3F980C4A75A62DF24CAC68F9E1C46F8A440B8A660D2C4BB1C06AF8A2169C0277B83F7ADBE726BC679F1EB5FFE7A62EC56D9358B4CC9D77F6C12BDE45A9989F117F97782355753ED4E39553A75E1F2B0F1E4B65DAA5CE701ECDCC683DD558006D80CA560EDE45C52D4F63C29DEB66D3BB5B5C6C4F5ACF21B1359FBC47F9F5D3C78F0E0C8D98B4399F6BE3D9BBEF8D98FACDDB13E4615C3620BCBACD83A68F723F6F5BA5E87324D92B895BE7879CC3976EA7CEAF8D143B7AE9C7DE5557371F4072E0EC3B5F0D86D287F010538AED60C883E78FAE8F599A5A776AD8D29EC8E8651E57FDC908760F0C18A50359DDA3A3A919BB7A0E60E7075B784651CB998308E9EBA3079E8E59F8C5DBA706A243B71795E708BF0E4C4A9EF5FFBEA03FD7FBCF799F7B61835901D43758B5F497087C23FA8942D04EC7C914E0D5E318F1C3F3175E2C8A1D1B12B672F3BF9E9AB6E3C275CF72ED32FB9A974FB56826420B19991A19B8323296755282E8D4D2410CB55109686797D902DEDF17A798EDF609272D572BEF0FC572FBFF89DEFDDB879F9F484959B16CA34219A9BA622752F33AC81B511ABE1519A0CD154085101CABB3595A6E3E7864A875F792571FAC4D15B73C3E786C8CC8919DE08641AB20829700A74B7B75201366401B2B698189C3B3B7833FDC4B6D6783C86610780AB5A7320C1E454FFC1938727CFFDEBF3273EFFEC9F7DCF7DAE479C763F2B2E7BE4F6EEDB17FFAB4F7FE6D1F7BFEFEDAD8A0C8DA0B9C1F88F2E0ECFD0919317D2870EBE34F1FA991337721397AE4177375C979E7141976F03FDD62B80B72C7B8153E978EDF49991DAC70EC463B116CEE9B6CD8CDDE4C365A38A5C66A1EA82F52F0B29D35DFCF8838F7D70E72B3FFCDA27D14B280B88A033976E9A878F9D4C224C12575F3F356C2C8E0AC0B72013905948DA05EDD0B2ED57A1809A1B5BABAF5F7C6DE2D66C79F77D3D3EB25DE4CD01CCCAA2E7939FFCD46EFC8A5B9A9D9D5D5CD5D3132A148B89175E78E145CCFA2625C5335FEC3CB0F5335F7AE56A72EC4AF6CCF143A95B8327A7A83C9F705D7BB299A301FA0ED8FC9077AAB1BE7F7EE1E561CBC15643416042C4DEA8610F11FBAA10C342EA5EB98D8C4FFEA7DFEFF7BCA135D527E3AFCD387A37E4772003900E0E953B7093205B88E4673FF185EFCE0940707F564043A000ECED15A0F386E3A01DB59EF9FCF3A73BD76CFC38111D60F26B6E1E4818A2DF0920D537F9CEA9BBA43D75FAE489A982F9DEF6A0EACEF8E5953190424BFADAC51BC6E1578F4D1F3AF4726278F0D4B0959DBEE93275E6A75CBAEA0ADDD90A6892E1F8ADCBA7475FBDB4B8FD89ED71019E35831F53E9E899A1C24F0E1E4C9C38766464F2CA9961AAE5126E2CA796498EEEE04DA29FBFF590A2FFE17DEFFACBA79FFAE0FBD7CA9574E6F0E18363674F1EBB961EBD285255234DA55CD62EDC66E15F7305E8903D90ED44A239AFCD09AF80249701AEDC96AAFEDF28A0495C211764A909F8D77FBBFBFF0E13DDFD1F27EF2AE03779FB1F7564272FA5F86AD80000000049454E44AE426082','office-address-book.png','office-address-book','image/png',0);
INSERT INTO "kexi__blobs" VALUES(7,X'89504E470D0A1A0A0000000D4948445200000040000000400806000000AA6971DE000000017352474200AECE1CE900000006624B474400FF00FF00FFA0BDA793000000097048597300006EBA00006EBA01D6DEB1170000000774494D4507DB040909331633CB44E800000E554944415478DAED5B4DA8255711FEAA4EDF37BE971F8812132793C4F187E0307115B274639080F1077423D90AD9BB1AC185091270E556701524B88E0EC1A55918454744238C8384109279669289C6CCCF7BEF769F2A1775EA9C3A7DFBCE642148C8343CE6F6BDDDA74FD7A9FAEAABAFCE00B78E5BC7ADE3D601E091471EF99F8D75FAF4C31F9AF7A678F2E9071FFCD9F58383EF32338808440455BDE920AA8A3C4D482961776FEFC7AFBDF6DA990FA501BEFEAD6FEB57BFF14DBCF4EB17C1CC488921A250551011123314C0300CE53C819840507CF6D469ECDE7E279EFDC1F771717F9F3E2C0618E2C934097EF2EC8F70B45E43258339010044A558AABC1711540422826135601A27ECFDE6257CE5F1C7211FC063FE9FC7F1E3C7B1BFBFBF6C807FFCFD3C344F18D200610683CA0A03A20A11414A095A4DA15025A46309878787D87FF3228848EFBFFF7E25258005220C4036426616442062A82A140A024354A0A260A67A8F87A47DE63A0E51FB1D44C853061175F73233729EDEDBD9D9F9D2430F3DF4CA850B173643E0E4E7BFA03B9421F561C09018E3340144661002B20888084362AC478BFD2C8213F79DC09D77DE0122C26AB5321C01210DC94226251080340C1806B37D4A09CC0C26422ADF0DAB15982C04991920C2906C8C6118ECFAF2672FCA203F0790120360A88A2D5CCE383A3CC41D1FFF84FEFCB9E7DEFEC3EF7F77EFA207ACB362352844150313541453CE206240154A0A056C5200B2A25B958B17DFC01B6F16AB2A20B08733DB64A0E651390B880444095331AEAF4462B630220214C523AAA34044C0899173AE061615A8026948C839030A10999710DA82DD76DB1E7DE293C7EFD98A010A03BA294FEEE0000CE4140A2E6EC74C98B26104B119674809AA8A55224C53062502652D2F0F10995B4B2E61A4543D8553C2348E580D03C671C4B04A8058D8E5AC4843AA2E5E02B38C61DF6511ECECAC308D13B8788948862A415590861500C5344E383838D80E8220C638D9445A9C09380DC879AAB1344D19694810C98052B5BC96091301E37A2CABA1507563DAE1D7AA2A3825C8E1218819E338422423E501220A26BB4FD500D7425E2B1013A1AC36E1DA388289202298A609225A9C4841A09AD6FFFD9FABB8A10710999B890A186C80A8B9014CCE60767E60BE9173B6CF2A20264C63C6344D383A3AC2952B576A9C16B42AF337C71691E22536615180A98C0506A83C4701667B410F3B1FA30349D10EB8FD1A22C6B155C27868D73FF99D27F1FC2F9EEF41F0C4E7BEA8C7F4AAC51333503CA07B40404EFF8E8AE5CD0304E3B8C67ABDC6E9D3A771E6CC19FCF1DC39FC737FBF82964849AB449DF9A1D4C172447E476C4704222DA14570C8A8F79485D41AC8661840C144383A3CBA08A62F33F385CE00F77EE661DD95F7A105418918CC16635C57CCAD5AE22F678BB92C0029A669C23465BCFBEEBB78E18517F0F433CF606718CC70E5E51D44A3076C3B4480940839DBEBB44B0584042507C70C80C16C49D793EFD2C8ABD54A3FB677FBFE8B677F79A20B01C91947EBB5C5BE41B0D9AF069BA16A7557B16599A60CC07882C7A0AAE24FE7CE61351850A63273F7025FF5C40912917E76A4641EB0DA616816500D97547808404C484CD5C8C9733F10BCA35B347AEBEDCBF701C0F0C4134FE0ECD9B300802B57AF810EDE33842E71EEEE54FCAB3EA4C6A46A8D6B07B894121E7DF451FCF595574094AAF734D796024D004841C590D4206E8334A9D89BA85F139EE9A18002AC8D5C39E6B491FDF73C4D8D083DF6D8633F3D79F2E45343628BBA19C8A86AF7121A06AA315D7E67624C79C4E5CBFFC2E5CBEF6CBC8CE5E6F23211C80240CED9623F9F18FBCBB43B82A381610350140F79E7BDF7F1B7BFFC99060078FDF5D79F5AAFD7C839534F2F7BE8A931E5139AAD98D1CDDCB1B4804E0530ED05FC5CA4D164F6EB672F569FE5A9B38401421AEC8032846835B60483040F1800E0E8E8C82ABDB8EAF372B1C4146DB8333A3318C50DA651816A382707D892B24A383953D0E04D716C7FBDCEE3D44243496B5611F5E46C29B4D9A9204DC18F693C6A0618C7B1108B05B70A2E9AA7C9185F9910B981368C324B5F6551D5611D8DD25A7A42E51571C5DD6C75C597C666EA8856357531B2747851A83C092467F33A2B83A73AA919BE5416162BC1E86A52DC524BB5A8DAE2DBC7F171698E1DB9E718BECE39DBB85E5A37E6283D3E1020D354565A0B486BC189F65D077E593B563AC47C3CB7B04DCA3E9B3812E1BA3A652D636B525E002C40912BCBD3AE108DCF337A4C1D46F4311EA6AF00A5B4F192EE070AF3FF6E6C4617E243BDB10E826AC118053967CBC122809A5B8A48E309355EDB8BB77465DF734DA5DA798351E81462BBDCAF52AB42B7571623433ACB183D36497D6CA5DCDAB88648035AAE16ABE8ABD58DCB6236180816716F58E2E5EE3554CFB80B09FFBD0DC701103590A462F0B28A202A2BA88BBAA4C69A2600F6A6B1DA7CAB43F800547D444D0A0B0056C344A4726B8F53C79118AB8E09764ECDB833A0B5DABE8DEFD830959A3FE7F65CAF0A3740D1E72CD28589EB0608186578D579401B5072AEC81C413084BDE5F3229850909C620C6A70BFAC33600A61E7B54024535D162D39BD011F350F8DF8A53A0360ED2ACD180E3171713CABA484B061A918B73E3808218D61E601DA919308B23E59414F62A27788233E7AF7359E60E9D45654AA6C030528CC193153A1A17F5C08AE1C3FB8A50F604980428CC5552C96266775D12D7B179DE7EE86319B5E21E17A29145C67282F799E6A155905E0267F699FCB8BF1B4C30A3466DB446F95F63271B55C8F8BD7D7B82B56D78AF8A8C85BE720DABBA9485753E85C285153843CFE23658E4A6F284EAC5A2C62CA7CA5232DD73056570EE7F2F298090D9577537B617221335662B350B0AFA92B5B2B50D558F64AC3B0DB7FA3EA89AD8C8DB543E51FD1C815A8FC9E381F74E3741E505D9666AE1B551F10A054BFAB7F90C2B4D121FF669DA0811334E5066053BDB4C789980EBD9071E9AD3D677B9F013E7659795D1CBB1577C0468DAD1DDDF594B8590891AD5490C7BA54379F5C7013859A7AEB2B535CBA816D5B18A93C2186E10C8FBABA428A4A54F808D3061FE898E0DC321D801523350C407555FBD77A86539EC0E4BA5F0621558628A59A0C001158A42375CB8031FDF514B7145244957E0BA4129F9E707918CDB2422CC12348CD3BC2D4E55A6C64898ED5A9F50D9A7244A138411554AAB109AE09CD0A2CD709FB9A81E68289872EA1176BB4E15855854298CF33D210A513AD9DE036913E7DF5DC5B44A3445C63CF3B3B3A93A13ACAAC8A787B8C5D1FBBCEC56BFE8EAEA3B1D22D6A922A409A17D5E60D0F8892556775CC563EA0285189FD7A6DBF5A7359AD154D551B99A5415AD422882934A609A11A06959FE6CA72A707D2CC30F3C648446E40830CEE393130BEECC549EBBF791766B340712FEA3105B52C0E7C2160C0DC2710E48C2677159DBF50DD16D7D491B2E6C50DBB86E0D58C683DAF99836C0DF465A5ABC5450AAA8034D735BD87888D0E8DB5AD3B14F1BEDF42B8751563746D42E82851AD4FFA8CD6BCD54B68EB51B6F9340F8805457848437A9B446C8DB9B82D9D52439D12547B8CD0B6DA824E01AA4D369951DEA81952EE3404AF1A89A979476E1E61D92B7765BD8D994B63559A0198196918A0D354FB78F37ABD166AA507DFC9CE33D7DDD4075A66189800173AAA7BF106662C85538FEA0A1E38B4D9B847B4A57872E04BA9EE7E21003876EC98DE75D75D3DDF8E9AF782547D83AF6B1AFA609B65081FF4CAEE8E6D0F0FD45917E76C6171EDDA355CB972C5FA02AB61C0EEEE6EA5975DA371A1035B35F725E00B8A5C21D05D51342FA436B2C40D76A2C57B698BF1E672FEE6B80AE684A3A3208B477E5FCB4C63D25D03A42B77B17DE1A208BDADD515F7F66CDF3BB4FD5E2959080B9DEA5AF2D2421B6DC6EA5A393CAB077C7302C216A779CBE9466DA91BAD2616B4BAA57197C3AB651C5A1CB3E76584CDCE519CDBE07ADEB81E6B3E4DCCC6054A7A14006929A6446ABF3A4A5BF15FD411C277E5BE70FB425BBC7F3EE279FDDC1AE0376BB3BB114480C45E5916039C3A750A0F3CF0C086E87033285208688B2FC4CD5300056EBF2D5BD056B5D7454F2E2A119567A3123D5A484BDAA9D67DCB3DE1D2A5B7F1F2CBBF3503BCFAEAAB75F59D95359183C25E1B0E10D4273F9D919DCAC0D09A218E312EA428745356D7B2A8BA34EEA6E052DA9F2D7CB432B7A06596FA426DC19819972E5D6A1EE07BF56C9757AA6D641545625AECD0D82C170C506699CABEBECA14CB6EB1F0F6ADA3ECE5ADCBE7DA5729DDD8123351717B42D75788FFBAAE68A19DEA02A59496F580F538DA6E8E80F8CEBB5B8AD42091A3132528D0480FA9A9B4CCE7E4DEB7D844BA1CABD1E515D7BA172995C57243E49CC1DE2AD35E59EE769306C57A98436FDB22D7840ADB75C5B38A51BB7D01EE6AB51FE029A900680D8D99D21B812BF61728547FCE2B50363AA85A1BDEAFF38E754AB61B25179166AE13CC35866680D8F020971C0BC672AAED399EED06E9D86C29565A13B58C427D1AE2F88C486B990AB9A250C152A711A8F8E77EFB9B7B1EB36DD28CFB90624D135FBD2B86E24AC44DC61C002D151773CB037DDF708EB2EE7AACA187EFFD3DF46289EF13B03DBEFDB8DA001D48BE7F90FA0BB8FA1F56AB1DCF1120D5A205F6F38C5961282FAD85069BF81B5A4A519B23E6057D7E618BCA46CF2ED4E52210EA9BAA5E05465DA1DF55425D58F80688AE0D576A53D5823773552ADC62EF9BAF4522F474CEF9872080325570EAC945E8B9EB267DD3286FCD527ADC29E2282F5AC60EDB0AFA81FB9D5DADBFCF654B5E2FCEC60669C3922634C631B2888EE3F4BD0A7FBBBBBBF700F81A804F31711215B6CA97130012DBF4CFD474AF6D7F58F87C33F6BC4D006AFD0EB39C443D34504CFF57C39FCC3ED77322CA22F21633FFEAEAD5AB6FD1EEEE2E0E0E0E70EAD4A9E1FAF5EBC9778A8908854628A96A775E56860A0DA57E3F95D24C0CA59BFC771D9D71FD78AE14D51340396C1228BF6BF87F4E5A304DDD33524AEA5B7C8848F7F6F6A6F3E7CFE7DDDD5D1BF1EEBBEFC647EDF828BEF3ADE3D671EBB8756C1CFF05BF21B9CD4C9FE30A0000000049454E44AE426082','folder-black.png','folder-black','image/png',0);
INSERT INTO "kexi__blobs" VALUES(8,X'89504E470D0A1A0A0000000D4948445200000040000000400806000000AA6971DE000000017352474200AECE1CE900000006624B474400FF00FF00FFA0BDA79300000009704859730000375D0000375D011980465D0000000774494D4507DA0B0D172E29AA17589000000F314944415478DAED5B7B7014D59AFF9DEE9E676626F348484061632098758D052C058B1528D482C5B02A55945C37D49580BAFEB196DCBA4BA9EB0DD1D5DA95BB68B9053EB6F682AC52AC6679B8200F2D71BD370425B040B9481E4216622421092199CCAB7BBAFB9CFDC339539D9E9E64023155EB72AABABAA7FBCCF4F97EE77BFCBEEF9C016EB55BED56FBFFDCC844BD489224689A2602B01B0E21F5980248F2C36EB7EBC964F2E705000069CB962D7F535A5ABA2E180C16F9FD7E8FC7E311DD6E3762B198DED7D7178D44223DC78F1FFFE7DADADAB700A81332A80904204F92A467962E5D7A9BA669843106C61800C0E57289858585F99224F92E5FBEFC2C80F700847F6E003829A51E008410024232954F1004D2D3D39307C0F97304C05E5E5EEE120421BB3D128250289497F20F13D2847100D056585828B8DDEE51010885420EAEF6568D3186FCFC7CC76800545656A2B0B0500060BBD949BC1900BC6FBDF5D6C68B172FB6CC983163493C1EB78DF29EC96EB71B94D2AC87AEEB983469920860EA4820343636DA67CE9CF9E72D2D2D6DEFBCF3CE6F0078273A7C16EEDAB5EBD0D0D0104B2412B4ADAD8DAE5AB5AA0680D3E974F23E2280BCAAAAAAAA83070FEEECECEC8CC462311A8BC5D848872CCBB4BEBE3EFAE28B2FEE7BECB1C75601F0A77E0B2B57AE0400E78A152B9E3A7EFC384B2693B4B3B393D5D5D51D04503051C24FD9B76FDFE94422C1C2E1300B87C32C1A8DD2AEAE2EB661C386DF00C8073069F3E6CD1BDBDADABA755D67B22CB3582CC6A2D1E8B0C32C3CBF178FC799AAAAACBDBD9D1D3972A4F7D5575FFD3B004500F29F7FFEF9972F5CB8C062B1181D1A1A62914884F5F7F7B3D75F7FFDBF004C19C9C78C070F28DAB367CFA70F3EF8E02C4551867972411098AEEBA4B1B1F1ABB2B2B23F2E2D2D0D288AC21863C4D88F5F33C6322281F11EF715A228B2783C4EAE5EBD3A78F9F2E5B63973E6CCF77ABD8C524A8CFD0060EBD6AD67EAEAEA1E04D09BAB40E21884CFDFB56B57FDF2E5CBEF9565398B1327282D2D9DEAF57A5DAAAA0200313B3D1EFFADC2A0F1396F9452220802028180B3A4A4E4764992C0181B263C3FCF9E3D7BB22CCBB34E9E3C790080329E0038366CD8F0CADAB56B572B8A92319BC6B39597B79AE9915A36D028A5C33E1BC1A2948210828A8A8AE9DF7DF79DD0DEDE7E0C803E5E51207FE1C285BFD6348D716FCDCFE6C1F0819A076725849580E6BEC6CF56EF304791828202B664C9920D295F346E1A20C4E3F192850B1756381C8E0C0138B3336B84F19A3B275DD7118FC771FDFA75F4F6F6626060008AA280520A5114D3FDAC0032ABBDF91E0074777793975E7A69676F6FEF81547275F34E30A5DA53162D5A54BF63C78E4A1EEAB8D09224650060143C1C0E6360600076BB1D0E8723C3240821E919555515BAAEC3E3F1C0E7F341D7F5AC4098FD452291C033CF3CF3E5E1C3877F4908B9329AA98DD509463B3A3A1ADADBDB973DF4D04321AECA7C3082206400A0EB3A7A7A7AA0AA2A3C1E0F4451CC06EEB0EFD9ED764422115CBB760D5EAF775864C80602A5149B366D3AFFE1871FAE06F0FD4F11050060F0C2850BA7FC7E7FF59C39736CDCF118431857614A29C2E1306C361B44514C3F371FE688C085B3D96C70B95CE8EFEF87CD6683CD66B3340B0EFE810307222FBFFCF24A00E701B05C051A2B8FA600A2A5A5A576A313E202E8BA0E4110A0AA2AE2F1388CA4C4CA3F70E1ADCE1CC4BCBC3CC4E3F1B466647388C5C5C5760083A931E2A7D200E7134F3CB1A9A6A666B6D1368D5AA0284A5A78F30C9B494E3633303FE7A0DA6C36CB30A8EB3A8A8A8AA4BEBEBEE4B973E77E9F4BF8BBD164E8F6254B96AC5655D57226344D43381C4E87A56C332F08C2B083DFCBA625FC1C8D462DDF9B3203565555F557000A7FAA6C509A376F5ECDDCB973054DD3AC0680DEDE5E88A298E6094610781F411018575B455160ACFD59998C11044A2962B1588653E48C71D6AC59DEFBEFBFBF7A2C9A3D1600DC4B972E5D4908614672C307C21D9E71B09C2871E11863E8E9E9A1DBB76F3FF4C8238FAC9F39736655595959D58A152B7EF5FEFBEF1F191C1CA446476AE61700A0AA2A92C9644624A094C2EBF5B2458B165503C8BBE164C8E97442966521058E98EA230198BD7FFFFE86F2F2F2F4E0B80A8BA298F6D65C958D2ACE79426363E37FAF5EBDFA59006701C40CB62AA6063D67C78E1D6F2F5EBCF82E0EA29139F2CF8490344730B3C5B367CFE2D1471FAD04F04DEAF7293FACAACD5644C8FEC61B6FFC754545451563CCEB743AFD6EB7DBE3F7FB272B8A4244512466018D1A607CC6AF4551C4B163C74EAD59B3E617003A46F0D402803FDAB66DDBDE071E7860B699061B29AFCFE7CB0026D59FC9B2AC534ABB1863D1442211D6346DA8A9A9E9E073CF3DF72F667668150603A2286E0A0402364208E1C22412090882004A695A68FE724DD3D2F72C581EEBEAEA8AAD59B3E62900974789D114C0E5279F7C725D4343C3F1A953A7BA745D275651435114CBB048292592244994D269BAAEC3E57201002B2828A804B01B40CF683E20E07038EC8C316246DE2A31E1AA9A8D42534A497D7DFD765114DB7224284C1084D63D7BF6ECE01A6A953A7355B61A8F394701401289441E8080F9B7AC0028B6D96C695533DAD948D959B623168B61EBD6ADBB755D5772665B942A5BB66CF9F7643299B56EA0699A25005675465DD7218A22DC6E7720836358797BBBDD3E2CD5B518E0B067A9E24746764608C1D5AB57E3007E180B3D4DF5EDE8EEEE56B2016B95245901615C9AF3FBFDDE5CC2A0C8E3B6F165E6D86ED486442291610EFC733299546F70994B55555537E60C6601AD1222B386F2B1A7CC51CC0500850B64FC41B3C735A26FB4473371F1FBFDF96389CB86E60906836EF3FB47CA06CD1A6A04291E8F23994CC67301E0BADBED86A669C30018A95AC3898955C526180CA2BABAFA7E616CE55AA1A6A666492010C8AAE23CDD3602641EAFF1AC280AAE5FBF1ECE0900A34D1B9D8951AD8CD74EA7336BB9CAE170B0EAEAEAF59452AFDFEF1F55F294D09EEAEAEA67D98F2D83101967DA2CA8957F00808181010DC0502ED92003B044108429BC4C25CB32AFFCA4CBDCC64CCF6EB74355D58C82080F830505058573E7CEB57DF4D147FF395ABA2ACBB2F4C1071FFCFD3DF7DCF317945262359BC964124EA713A22866787C002C1A8D929E9E1E5CB97205172F5E44737333BEFAEAABA64B972E6D03208FC604855446350D800740FEA4499382454545955BB76E5DEB743AD3ECCE789665395D0FE0F7381B942489251209F2C30F3FBCF9F0C30FFF6D28144AF6F7F70F8B0A858585A4AFAFCF7EE8D0A17FB8F3CE3B7F4D0861BAAE1363019603108D4651545494E19C755D477777379E7EFAE96DBDBDBD5FABAADA9F9AF508804E42481F638C8EB5264852C76D9B366D3ABB78F1E220006206201E8FA7675F92A48C9C401445A6EB3AA1945EE8EAEAFAC765CB961D0030C035FFF0E1C30F9594943CEF743ACB08218C524A28A5E91AA171A6354D83CFE783B9420D801D3A74A8B7AEAE6E9E21F4B29BAD08F11FB9B67BF7EE2FEEBBEFBE554612C2CF797979E8ECEC4430184CDB20A7CEA93E441445489234A3A4A4E4779D9D9DBF638C4553AB3F1E4551C01863A9FEC498451A1D72241241717131784ACEFD11FF5E6363E3D1D4CA504E95A1B17866F9F4E9D33BBEFDF6DBAC71D9E3F14096E50CD668B2530200B1580CB22C7B1445F1C462312E2431C7714248BA989A4C26D359A031CCF1735B5B1B3EFFFCF3F7735D151A2B000CC0C9C3870F9FB1D96CCC6A974730181C962899BDB3398A58710A731CE785146E6AD9F621489284CF3EFBAC8931767A2CAC73AC25B1F0DEBD7B7FDBD6D646AC96BB28A508854296591A2F9A5A698555AE61E61C9AA661F2E4C9594BE4CDCDCDA8AFAFFFED58B7D68C15001DC08973E7CEF58CB4B8E972B9C0F3092338C6D83D1A10FC99AAAAD0340DC5C5C5E999CEC8E80401CDCDCD5792C9E4E9B11444C7541576BBDD5055D5B960C1825FAD5BB7AE8AAFF0980F1E09EC763B344D437F7F3F3C1E8F6552956DB98B1FAAAAC2E57265AC105991A0A2A222DFF7DF7F3FD4D1D1D1E476BB354EE6C60D005555ED1515158F6FDCB87173281462E4C766293CBF76381C080402E8EBEB43329984DD6ECFBA0A6CAE2EDB6C36F8FD7EA496C387990377C2463371B95C6CE6CC998BBFF9E69B4B57AF5E3D9FAB26E404407E7EBE545E5EBEACB6B6F6DFA64C99C28C858A6CC21B0B193E9F0F7EBF3F4D948C24C9D8571445381C0E78BD5EF09A8439EF303B4DC37D120804D8DD77DFFD484B4BCB094DD32EC9B24C732139B9B4C0A79F7EDAE3F3F924C61831D7FC8CD71C04E3753653318398CD4CAC2AC0DCA96A9A96E609A9E70C4064FEFCF97700B83E5E4E906CDFBEFD822008245BEDDE4AA5475273A330D92A4F56F7B8A03C941ACBE87C589F7CF2C9A5F176824A7373F399BCBCBCBF9C356B96646502233944AB25B46C0066CBF9CD2A6F95250A82C01A1A1AA2B5B5B5BF00D09E0B1F10C740827A9B9A9A5A4B4B4B57969595112B100441405353130020140A314E69AD4018A9A069C5045B5B5BD1DCDC8C69D3A66508CE853F75EA94F6C20B2FFC52D7F5DF33C6F4718D0229AF7AE9C4891397A64F9FFE706969697A0313E7EB6FBFFD76DB2BAFBCB276EFDEBD7F5055757A5151516128141A71E3859509A5C064822090F3E7CFE3DD77DF3D575B5B5BBB7FFFFEEDFDFDFDF3EFBDF7DE00CF330CC2EBB5B5B54F85C3E1FF608CE54C856F64BBBCCBE170ACA8ABABDBB17CF9723B630C914884BCF9E69B8D1F7FFCF19384908BA95D5C0582202CAEAAAA5A535959B970C1820579C5C5C58C97DBCD8ECFB07EC8BABBBBC9D75F7F1D397AF4E8B1A3478F7E00E00F00AEA53471466565E5BFBEF6DA6B7F969F9FCF1863E48B2FBE88D7D6D6AE4D24129F30C6129880E600707F4D4D4DC7CE9D3BD9DCB973EB014C2599F450C08FDB58CB7D3EDF3F353434B0969616D6DADACA5A5B5B2DAFBFFCF24B160A853603284F7D57B0F8CDDBEFB8E38E7DBB77EF66EBD7AF6F07B0581004C78D0872C37F982084488CB1DB00DC05E02421A47F943D3977BDF7DE7BE7172C5890B5D64F08415353131E7FFCF13F01D03CCAB8834EA7739E2CCB2DA9DC5FBB11396E78A735634C4BADF375586D7AB068314551B4147059418844224A6AE17434A7DC2FCBF2919B55650113D79276BB5DCEF667099ED40C0E0EC691C3F6B6F16A130980DAD7D7171145D19233F09C7F6868283691004CE43F4612A74E9DFA9FE9D3A74F0E0683C8CFCF4F6FA8482693181C1C445F5F1FCE9C39F39DB972FB53B609FBD7182144608C4D01F0A7849012A7D31902E04CD5FE154551AEA596CFCF0882D045B32D39FF5F05C0F44EC1506D36165EE91817516FB55BED56BBB9F6BFD861F1268BFE47BF0000000049454E44AE426082','application-x-executable.png','application-x-executable','image/png',0);
CREATE TABLE cars (id INTEGER PRIMARY KEY, model Text(200));
INSERT INTO "cars" VALUES(1,'Fiat');
INSERT INTO "cars" VALUES(2,'Syrena');
INSERT INTO "cars" VALUES(3,'Chrysler');
INSERT INTO "cars" VALUES(4,'Volvo');
INSERT INTO "cars" VALUES(5,'BMW');
CREATE TABLE ownership (id INTEGER PRIMARY KEY, owner Integer, car Integer, since Integer);
INSERT INTO "ownership" VALUES(1,1,1,2004);
INSERT INTO "ownership" VALUES(2,2,2,1982);
INSERT INTO "ownership" VALUES(3,3,3,2002);
INSERT INTO "ownership" VALUES(4,4,4,2005);
INSERT INTO "ownership" VALUES(5,10,4,2006);
INSERT INTO "ownership" VALUES(6,4,1,2003);
INSERT INTO "ownership" VALUES(7,12,3,1999);
COMMIT;
