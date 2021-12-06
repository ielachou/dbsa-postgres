CREATE TABLE Table1
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id1_pkey PRIMARY KEY ("ID")
);


CREATE TABLE Table2
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id2_pkey PRIMARY KEY ("ID")
);



CREATE TABLE Table3
(
    "ID" integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 1 MINVALUE 1 MAXVALUE 2147483647 CACHE 1 ),
    "r1" int4range,
    "r2" tsrange,
    "r3" daterange,
    CONSTRAINT id3_pkey PRIMARY KEY ("ID")
);


do $$
begin
   for counter in 1..100000 loop
    INSERT INTO Table1  (r1,r2,r3)
	VALUES (

		int4range(floor(random() * (10-0+1) + 0)::int,floor(random() *(20-10+1) + 10)::int),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
    INSERT INTO Table2  (r1,r2,r3)
	VALUES (

		int4range(floor(random() * (10-0+1) + 0)::int,floor(random() *(20-10+1) + 10)::int),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
     INSERT INTO Table3  (r1,r2,r3)
	VALUES (

		int4range(floor(random() * (10-0+1) + 0)::int,floor(random() *(20-10+1) + 10)::int),	
		tsrange(timestamp '2000-01-01 00:00:00' + random() * (timestamp '2010-01-01 00:00:00' -timestamp '2000-01-01 00:00:00'), timestamp '2010-01-01 00:00:00' +random() * (timestamp '2021-01-01 00:00:00' -timestamp '2010-01-01 00:00:00')),
		daterange(date '2000-01-01' + floor(random()* (date '2010-01-01' -date '2000-01-01'))::int, date '2010-01-01' +floor(random() * (date '2021-01-01' -date '2010-01-01'))::int)
		);
   end loop;
end; $$;



