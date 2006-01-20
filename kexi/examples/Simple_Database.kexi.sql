BEGIN TRANSACTION;
CREATE TABLE cars (id INTEGER PRIMARY KEY, owner UNSIGNED Integer, model Text(200));
INSERT INTO "cars" VALUES(1, 2, 'Fiat');
INSERT INTO "cars" VALUES(2, 2, 'Syrena');
INSERT INTO "cars" VALUES(3, 3, 'Chrysler');
INSERT INTO "cars" VALUES(5, 4, 'Volvo');
INSERT INTO "cars" VALUES(6, 3, 'BMW');
CREATE TABLE kexi__db (db_property Text(32), db_value CLOB);
INSERT INTO "kexi__db" VALUES('kexidb_major_ver', '1');
INSERT INTO "kexi__db" VALUES('kexidb_minor_ver', '2');
INSERT INTO "kexi__db" VALUES('kexiproject_major_ver', '1');
INSERT INTO "kexi__db" VALUES(' kexiproject_major_ver', 'Główna wersja projektu');
INSERT INTO "kexi__db" VALUES('kexiproject_minor_ver', '0');
INSERT INTO "kexi__db" VALUES(' kexiproject_minor_ver', 'Poboczna wersja projektu');
CREATE TABLE kexi__fields (t_id UNSIGNED Integer, f_type UNSIGNED Byte, f_name Text(200), f_length Integer, f_precision Integer, f_constraints Integer, f_options Integer, f_default Text(200), f_order Integer, f_caption Text(200), f_help CLOB);
INSERT INTO "kexi__fields" VALUES(1, 3, 'id', 0, 0, 119, 1, NULL, 1, 'ID', NULL);
INSERT INTO "kexi__fields" VALUES(1, 3, 'age', 0, 0, 0, 1, NULL, 2, 'Age', NULL);
INSERT INTO "kexi__fields" VALUES(1, 11, 'name', 200, 0, 0, 0, NULL, 3, 'Name', NULL);
INSERT INTO "kexi__fields" VALUES(1, 11, 'surname', 200, 0, 0, 0, NULL, 4, 'Surname', NULL);
INSERT INTO "kexi__fields" VALUES(2, 3, 'id', 0, 0, 119, 1, NULL, 1, 'ID', NULL);
INSERT INTO "kexi__fields" VALUES(2, 3, 'owner', 0, 0, 0, 1, NULL, 2, 'Car owner', NULL);
INSERT INTO "kexi__fields" VALUES(2, 11, 'model', 200, 0, 0, 0, NULL, 3, 'Car model', NULL);
INSERT INTO "kexi__fields" VALUES(53, 11, 'a', 200, 0, 0, 200, '1', 0, NULL, NULL);
INSERT INTO "kexi__fields" VALUES(53, 11, 'b', 200, 0, 0, 200, '2', 1, NULL, NULL);
CREATE TABLE kexi__objectdata (o_id UNSIGNED Integer NOT NULL, o_data BLOB, o_sub_id Text(200));
INSERT INTO "kexi__objectdata" VALUES(4, '<!DOCTYPE UI>
<UI stdsetdef="1" version="3.1" >
<pixmapinproject/>
<class>QWidget</class>
<widget class="QWidget" >
<property name="name" >
<cstring>formularz1</cstring>
</property>
<property name="geometry" >
<rect>
<x>0</x>
<y>0</y>
<width>530</width>
<height>290</height>
</rect>
</property>
<property name="caption" >
<string>formularz1</string>
</property>
<property name="dataSource" >
<string>q_persons</string>
</property>
<property name="paletteBackgroundColor" >
<color>
<red>238</red>
<green>207</green>
<blue>208</blue>
</color>
</property>
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit1</cstring>
</property>
<property name="geometry" >
<rect>
<x>90</x>
<y>140</y>
<width>120</width>
<height>20</height>
</rect>
</property>
<property name="dataSource" >
<string>id</string>
</property>
</widget>
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit2</cstring>
</property>
<property name="geometry" >
<rect>
<x>90</x>
<y>170</y>
<width>120</width>
<height>20</height>
</rect>
</property>
<property name="dataSource" >
<string>name</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel1</cstring>
</property>
<property name="geometry" >
<rect>
<x>90</x>
<y>50</y>
<width>152</width>
<height>36</height>
</rect>
</property>
<property name="dataSource" >
<string>name</string>
</property>
<property name="font" >
<font>
<family>Verdana</family>
<pointsize>12</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>1</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="shadowEnabled" >
<bool>true</bool>
</property>
<property name="text" >
<string>name</string>
</property>
</widget>
<widget class="Line" >
<property name="name" >
<cstring>Line1</cstring>
</property>
<property name="geometry" >
<rect>
<x>90</x>
<y>90</y>
<width>400</width>
<height>11</height>
</rect>
</property>
<property name="frameShadow" >
<enum>Plain</enum>
</property>
<property name="lineWidth" >
<number>3</number>
</property>
<property name="orientation" >
<enum>Horizontal</enum>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel2</cstring>
</property>
<property name="geometry" >
<rect>
<x>250</x>
<y>50</y>
<width>192</width>
<height>36</height>
</rect>
</property>
<property name="dataSource" >
<string>surname</string>
</property>
<property name="font" >
<font>
<family>Verdana</family>
<pointsize>12</pointsize>
<weight>50</weight>
<bold>0</bold>
<italic>1</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="shadowEnabled" >
<bool>true</bool>
</property>
<property name="text" >
<string>surname</string>
</property>
</widget>
</widget>
<includehints>
<includehint>klineedit.h</includehint>
<includehint>qlabel.h</includehint>
</includehints>
<layoutDefaults spacing="6" margin="11" />
<tabstops>
<tabstop>LineEdit1</tabstop>
<tabstop>LineEdit2</tabstop>
<tabstop>TextLabel1</tabstop>
<tabstop>TextLabel2</tabstop>
</tabstops>
</UI>
', NULL);
INSERT INTO "kexi__objectdata" VALUES(57, 'SELECT *, cars.owner AS ab, 1.3 AS wyr1 FROM cars WHERE owner > 3', 'sql');
INSERT INTO "kexi__objectdata" VALUES(57, '<query_layout><table name="cars" x="11" y="5" width="109" height="102"/></query_layout>', 'query_layout');
INSERT INTO "kexi__objectdata" VALUES(65, '<!DOCTYPE UI>
<UI stdsetdef="1" version="3.1" >
<pixmapinproject/>
<class>QWidget</class>
<widget class="QWidget" >
<property name="name" >
<cstring>form1</cstring>
</property>
<property name="geometry" >
<rect>
<x>0</x>
<y>0</y>
<width>530</width>
<height>360</height>
</rect>
</property>
<property name="autoTabStops" >
<bool>true</bool>
</property>
<property name="blendOpacity" />
<property name="caption" >
<string>form1</string>
</property>
<property name="dataSource" >
<string>cars</string>
</property>
<property name="displayMode" >
<enum>NoGradient</enum>
</property>
<property name="focusPolicy" >
<enum>NoFocus</enum>
</property>
<property name="gradientColor1" >
<color>
<red>76</red>
<green>255</green>
<blue>121</blue>
</color>
</property>
<property name="gradientColor2" >
<color>
<red>94</red>
<green>157</green>
<blue>49</blue>
</color>
</property>
<property name="gradientType" >
<enum>PipeCrossGradient</enum>
</property>
<property name="icon" />
<property name="paletteBackgroundColor" >
<color>
<red>203</red>
<green>238</green>
<blue>185</blue>
</color>
</property>
<property name="paletteBackgroundPixmap" />
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit1</cstring>
</property>
<property name="geometry" >
<rect>
<x>120</x>
<y>90</y>
<width>120</width>
<height>18</height>
</rect>
</property>
<property name="dataSource" >
<string>id</string>
</property>
<property name="focusPolicy" >
<enum>StrongFocus</enum>
</property>
<property name="text" >
<string></string>
</property>
</widget>
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit2</cstring>
</property>
<property name="geometry" >
<rect>
<x>120</x>
<y>130</y>
<width>120</width>
<height>18</height>
</rect>
</property>
<property name="dataSource" >
<string>owner</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel1</cstring>
</property>
<property name="geometry" >
<rect>
<x>30</x>
<y>20</y>
<width>266</width>
<height>34</height>
</rect>
</property>
<property name="dataSource" >
<string>model</string>
</property>
<property name="font" >
<font>
<family>Verdana</family>
<pointsize>16</pointsize>
<weight>75</weight>
<bold>1</bold>
<italic>0</italic>
<underline>0</underline>
<strikeout>0</strikeout>
</font>
</property>
<property name="paletteBackgroundPixmap" />
<property name="shadowEnabled" >
<bool>true</bool>
</property>
<property name="text" >
<string>Model</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel2</cstring>
</property>
<property name="geometry" >
<rect>
<x>40</x>
<y>90</y>
<width>76</width>
<height>14</height>
</rect>
</property>
<property name="text" >
<string>ID</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel3</cstring>
</property>
<property name="geometry" >
<rect>
<x>40</x>
<y>130</y>
<width>76</width>
<height>14</height>
</rect>
</property>
<property name="text" >
<string>Owner</string>
</property>
</widget>
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit3</cstring>
</property>
<property name="geometry" >
<rect>
<x>120</x>
<y>170</y>
<width>120</width>
<height>18</height>
</rect>
</property>
<property name="dataSource" >
<string>cars.model</string>
</property>
</widget>
<widget class="KexiDBLineEdit" >
<property name="name" >
<cstring>LineEdit4</cstring>
</property>
<property name="geometry" >
<rect>
<x>120</x>
<y>230</y>
<width>120</width>
<height>18</height>
</rect>
</property>
<property name="dataSource" >
<string>model</string>
</property>
<property name="lineWidth" >
<number>1</number>
</property>
</widget>
<widget class="KexiPushButton" >
<property name="name" >
<cstring>PushButton1</cstring>
</property>
<property name="geometry" >
<rect>
<x>320</x>
<y>90</y>
<width>127</width>
<height>30</height>
</rect>
</property>
<property name="text" >
<string>One</string>
</property>
</widget>
<widget class="KexiPushButton" >
<property name="name" >
<cstring>PushButton2</cstring>
</property>
<property name="geometry" >
<rect>
<x>320</x>
<y>160</y>
<width>127</width>
<height>27</height>
</rect>
</property>
<property name="focusPolicy" >
<enum>NoFocus</enum>
</property>
<property name="text" >
<string>Two</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel4</cstring>
</property>
<property name="geometry" >
<rect>
<x>40</x>
<y>170</y>
<width>76</width>
<height>14</height>
</rect>
</property>
<property name="text" >
<string>Model</string>
</property>
</widget>
<widget class="KexiLabel" >
<property name="name" >
<cstring>TextLabel5</cstring>
</property>
<property name="geometry" >
<rect>
<x>40</x>
<y>230</y>
<width>66</width>
<height>14</height>
</rect>
</property>
<property name="text" >
<string>Model</string>
</property>
</widget>
</widget>
<includehints>
<includehint>klineedit.h</includehint>
<includehint>qlabel.h</includehint>
<includehint>kpushbutton.h</includehint>
</includehints>
<layoutDefaults spacing="6" margin="11" />
<tabstops>
<tabstop>TextLabel1</tabstop>
<tabstop>TextLabel2</tabstop>
<tabstop>LineEdit1</tabstop>
<tabstop>PushButton1</tabstop>
<tabstop>TextLabel3</tabstop>
<tabstop>LineEdit2</tabstop>
<tabstop>TextLabel4</tabstop>
<tabstop>LineEdit3</tabstop>
<tabstop>PushButton2</tabstop>
<tabstop>TextLabel5</tabstop>
<tabstop>LineEdit4</tabstop>
</tabstops>
</UI>
', NULL);
INSERT INTO "kexi__objectdata" VALUES(75, 'SELECT * FROM persons', 'sql');
INSERT INTO "kexi__objectdata" VALUES(75, '<query_layout><table name="persons" x="142" y="104" width="109" height="120"/></query_layout>', 'query_layout');
INSERT INTO "kexi__objectdata" VALUES(96, '<!DOCTYPE UI>
<UI stdsetdef="1" version="3.1" >
 <pixmapinproject/>
 <class>QWidget</class>
 <widget class="QWidget" >
  <property name="name" >
   <cstring>report2</cstring>
  </property>
  <property name="geometry" >
   <rect>
    <x>0</x>
    <y>0</y>
    <width>590</width>
    <height>590</height>
   </rect>
  </property>
  <property name="caption" >
   <string></string>
  </property>
  <widget class="Label" >
   <property name="name" >
    <cstring>Label1</cstring>
   </property>
   <property name="geometry" >
    <rect>
     <x>10</x>
     <y>20</y>
     <width>566</width>
     <height>134</height>
    </rect>
   </property>
   <property name="backgroundOrigin" >
    <enum>ParentOrigin</enum>
   </property>
   <property name="font" >
    <font>
     <family>Trebuchet MS</family>
     <pointsize>48</pointsize>
     <weight>75</weight>
     <bold>1</bold>
     <italic>1</italic>
     <underline>0</underline>
     <strikeout>0</strikeout>
    </font>
   </property>
   <property name="text" >
    <string>Sales Report</string>
   </property>
  </widget>
  <widget class="PicLabel" >
   <property name="name" >
    <cstring>PicLabel1</cstring>
   </property>
   <property name="geometry" >
    <rect>
     <x>420</x>
     <y>170</y>
     <width>1</width>
     <height>1</height>
    </rect>
   </property>
   <property name="backgroundMode" >
    <enum>PaletteBackground</enum>
   </property>
   <property name="backgroundOrigin" >
    <enum>ParentOrigin</enum>
   </property>
   <property name="cursor" >
    <cursor>0</cursor>
   </property>
   <property name="frameShape" >
    <enum>NoFrame</enum>
   </property>
   <property name="pixmap" >
    <pixmap>image0</pixmap>
   </property>
   <property name="scaledContents" >
    <bool>true</bool>
   </property>
  </widget>
  <widget class="KexiSubReport" >
   <property name="name" >
    <cstring>SubReport1</cstring>
   </property>
   <property name="geometry" >
    <rect>
     <x>20</x>
     <y>170</y>
     <width>330</width>
     <height>290</height>
    </rect>
   </property>
  </widget>
 </widget>
 <images>
  <image name="image0" >
   <data format="XBM.GZ" length="79" >789c534e494dcbcc4b554829cdcdad8c2fcf4c29c95030e0524611cd48cd4ccf28010a1797249664262b2467241641a592324b8aa363156c15aab914146aadb90067111b1f</data>
  </image>
 </images>
 <layoutDefaults spacing="6" margin="11" />
 <tabstops>
  <tabstop>SubReport1</tabstop>
 </tabstops>
</UI>
', NULL);
INSERT INTO "kexi__objectdata" VALUES(101, 'example', NULL);
CREATE TABLE kexi__objects (o_id INTEGER PRIMARY KEY, o_type UNSIGNED Byte, o_name Text(200), o_caption Text(200), o_desc CLOB);
INSERT INTO "kexi__objects" VALUES(1, 1, 'persons', 'Persons in our factory', NULL);
INSERT INTO "kexi__objects" VALUES(2, 1, 'cars', 'Cars owned by persons', NULL);
INSERT INTO "kexi__objects" VALUES(4, 3, 'persons', 'Formularz1', NULL);
INSERT INTO "kexi__objects" VALUES(57, 2, 'selected_cars', 'Zapytanie7', NULL);
INSERT INTO "kexi__objects" VALUES(65, 3, 'form', 'Form1', NULL);
INSERT INTO "kexi__objects" VALUES(75, 2, 'q_persons', 'Zapytanie6', NULL);
INSERT INTO "kexi__objects" VALUES(96, 4, 'report', 'Report2', NULL);
INSERT INTO "kexi__objects" VALUES(101, 5, 'script1', 'Skrypt1', NULL);
CREATE TABLE kexi__parts (p_id INTEGER PRIMARY KEY, p_name Text(200), p_mime Text(200), p_url Text(200));
INSERT INTO "kexi__parts" VALUES(1, 'Tables', 'kexi/table', 'http://koffice.org/kexi/');
INSERT INTO "kexi__parts" VALUES(2, 'Queries', 'kexi/query', 'http://koffice.org/kexi/');
INSERT INTO "kexi__parts" VALUES(3, 'Formularze', 'kexi/form', 'http://');
INSERT INTO "kexi__parts" VALUES(4, 'Reports', 'kexi/report', 'http://www.koffice.org/kexi/');
INSERT INTO "kexi__parts" VALUES(5, 'Scripts', 'kexi/script', 'http://www.koffice.org/kexi/');
CREATE TABLE kexi__querydata (q_id UNSIGNED Integer, q_sql CLOB, q_valid Boolean);
CREATE TABLE kexi__queryfields (q_id UNSIGNED Integer, f_order Integer, f_id Integer, f_tab_asterisk UNSIGNED Integer, f_alltab_asterisk Boolean);
CREATE TABLE kexi__querytables (q_id UNSIGNED Integer, t_id UNSIGNED Integer, t_order UNSIGNED Integer);
CREATE TABLE persons (id INTEGER PRIMARY KEY, age UNSIGNED Integer, name Text(200), surname Text(200));
INSERT INTO "persons" VALUES(1, 27, 'Jarosław', 'Staniek');
INSERT INTO "persons" VALUES(2, 60, 'Lech', 'Wałęsa');
INSERT INTO "persons" VALUES(3, 45, 'Bill', 'Gates ™');
INSERT INTO "persons" VALUES(4, 35, 'John', 'Smith');
INSERT INTO "persons" VALUES(5, 23, 'gżegżółka', NULL);
INSERT INTO "persons" VALUES(10, 12, 'Вашим заявкам', NULL);
INSERT INTO "persons" VALUES(12, 322, 'Παναγιώτης Σκαρβέλης ', '');
CREATE TABLE tabela3 (d Text(200), f Text(200), g Text(200), h Text(200), de Text(200));
CREATE TABLE tabela31 (a Text(200), b Text(200));
CREATE TABLE tabela311 (a Text(200), b Text(200), c Text(200), d Text(200));
CREATE TABLE tabela33 (a Text(200) DEFAULT '0', b Text(200));
CREATE TABLE tabela333 (a Text(200) DEFAULT '0', b Text(200) DEFAULT '1', x Text(200));
CREATE TABLE tabela4 (f Text(200) DEFAULT '0');
CREATE TABLE tabela6 (a Text(200), b Text(200), c Text(200), d Text(200));
CREATE TABLE uuu (id Text(1) PRIMARY KEY DEFAULT '0', j Boolean DEFAULT 1, dd Text(200));
CREATE TABLE kexi__blobs (o_id INTEGER PRIMARY KEY, o_data BLOB, o_name Text(200), o_caption Text(200), o_mime Text(200) NOT NULL, o_folder_id Integer UNSIGNED);
COMMIT;
