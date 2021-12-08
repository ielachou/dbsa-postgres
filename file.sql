
CREATE TABLE Table1
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "name" varchar(50),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id1_pkey PRIMARY KEY ("ID")
);


CREATE TABLE Table2
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "name" varchar(50),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id2_pkey PRIMARY KEY ("ID")
);



CREATE TABLE Table3
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "name" varchar(50),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id3_pkey PRIMARY KEY ("ID")
);




CREATE OR REPLACE FUNCTION random_between(low INT ,high INT) 
   RETURNS INT AS
$$
BEGIN
   RETURN floor(random()* (high-low + 1) + low)::int;
END;
$$ language 'plpgsql' STRICT;



do $$

declare
	  counter11 integer;
	  counter21 integer;
	  counter31 integer;
	  counter12 integer;
	  counter22 integer;
	  counter32 integer;
begin

   for counter in 1..20000 loop
   	
	  
	select * from random_between(0,300) INTO counter11;
	select * from random_between(990,1500) INTO counter21;
	select * from random_between(0,3000) INTO counter31;
	select * from random_between(300,992) INTO counter12;
	select * from random_between(1500,2000) INTO counter22;
	select * from random_between(counter31,3000) INTO counter32;
	  
   	
		
	INSERT INTO Table1  (name,r1,r2,r3)
	VALUES (
		
		(select (array['Wassim', 'Nidhal', 'Iliass','Ibrahim','Thierry','Mahmoud','Caca','Pipi','foo','Andrey'])[floor(random() * 10 + 1)]),
		int4range(counter11,counter12),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
	INSERT INTO Table2  (name,r1,r2,r3)
	VALUES (
		
		(select (array['Wassim', 'Nidhal', 'Iliass','Ibrahim','Thierry','Mahmoud','Caca','Pipi','foo','Andrey'])[floor(random() * 10 + 1)]),
		int4range(counter21,counter22),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
	INSERT INTO Table3  (name,r1,r2,r3)
	VALUES (
		
		(select (array['Wassim', 'Nidhal', 'Iliass','Ibrahim','Thierry','Mahmoud','Caca','Pipi','foo','Andrey'])[floor(random() * 10 + 1)]),
		int4range(counter31,counter32),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
	
   end loop;
end; $$;



