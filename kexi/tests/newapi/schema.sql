
begin;

drop table kexi__objects;
drop table kexi__fields;
drop table kexi__querydata;
drop table kexi__db;

CREATE TABLE kexi__objects (o_id Integer, o_type Byte, o_name Text(200),
o_caption Text, o_help LongText);

CREATE TABLE kexi__fields (t_id Integer, f_type Byte, f_name Text(200), 
f_length Integer, f_precision Integer, 
f_constraints Integer, f_options Integer, f_order Integer, 
f_caption Text(200), f_help Text);

CREATE TABLE kexi__querydata (q_id Integer, q_sql LongText, q_valid Boolean );

CREATE TABLE kexi__parts (p_id Integer, p_name Text, p_mime Text, p_url Text);

CREATE TABLE kexi__db (db_property Text(32), db_value LongText );

insert into kexi__objects values (1, 1, 'persons', 'Persons', 'Persons group in our factory');
insert into kexi__fields values (1, 3, 'id',       0, 0, 16, 0, 0, null, null);
insert into kexi__fields values (1, 1, 'age',      0, 0, 16, 0, 1, null, null);
insert into kexi__fields values (1, 11, 'name',    30, 0, 16, 0, 2, null, null);
insert into kexi__fields values (1, 11, 'surname', 30, 0, 16, 0, 3, null, null);

insert into kexi__objects values (2, 1, 'cars', 'Cars', 'Cars owned by persons');
insert into kexi__fields values (2, 3, 'id',    0, 0, 16, 0, 0, null, null);
insert into kexi__fields values (2, 3, 'owner',    0, 0, 16, 0, 1, null, null);
insert into kexi__fields values (2, 11, 'model',   30, 0, 16, 0, 2, null, null);

insert into kexi__db values ('kexidb_major_ver', '1');
insert into kexi__db values ('kexidb_minor_ver', '2');

commit;

