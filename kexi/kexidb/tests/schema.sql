
begin;

drop table kexi__tables;
drop table kexi__fields;

CREATE TABLE kexi__tables (t_id Integer(1), t_name Text(200));
CREATE TABLE kexi__fields (t_id Integer(1), f_type Byte(1), f_name Text(200), 
f_length Integer, f_precision Integer, 
f_constraints Integer, f_options Integer, f_order Integer, 
f_caption Text(200), f_help Text);

insert into kexi__tables values (1, 'persons');
insert into kexi__fields values (1, 3, 'id',       0, 0, 16, 0, 0, null, null);
insert into kexi__fields values (1, 1, 'age',      0, 0, 16, 0, 1, null, null);
insert into kexi__fields values (1, 11, 'name',    30, 0, 16, 0, 2, null, null);
insert into kexi__fields values (1, 11, 'surname', 30, 0, 16, 0, 3, null, null);

insert into kexi__tables values (2, 'cars');
insert into kexi__fields values (2, 3, 'owner',    0, 0, 16, 0, 0, null, null);
insert into kexi__fields values (2, 11, 'model',   30, 0, 16, 0, 1, null, null);

commit;
