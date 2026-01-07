create table Events (
    eId integer not null primary key,
    eTitle VARCHAR(512),
    eDesc VARCHAR(1024),
    eDate VARCHAR(64),
    fatality integer,
    injury integer,
    notes varchar(1024)
);

create table Storm (
  stormName VARCHAR(32),
  sId integer not null generated always as identity,
  primary key (sId)
);

drop table STORM;

insert into STORM (stormName) SELECT distinct STORM_NAME from EVERYTHING;

create table During (
    sId integer not null,
    eId integer not null,
    foreign key (sId) references STORM,
    foreign key (eId) references EVENTS,
    primary key (sId,eId)
);

insert into During (sId, eId) SELECT sId, EVENT_ID from STORM, EVERYTHING where Storm.stormName = EVERYTHING.STORM_NAME;

select s.stormName, count(e.fatality) as death_count from Storm s, Events e where stormName is not null and s.eId = e.eId group by stormName order by death_count desc;

select s.stormName, sum(isNull(e.fatality, 0)) as death_count from  Events e, During d, Storm s  where e.eId = d.eId  and d.sId = s.sId group by s.stormName order by death_count desc;

select c.countryName, count(x."TRIGGER") as times_smited from Country c, Location l, Category x
    where c.code = l.code
    and l.eId = x.eId
    and x."TRIGGER" in ('volcano', 'earthquake', 'flooding', 'monsoon', 'tropical_cyclone', 'dam_embankment_collapse')
    group by c.countryName order by times_smited desc;

drop table Category;

create table CATEGORY (
    cId integer not null generated always as identity,
    category VARCHAR(32),
    "trigger" VARCHAR(32),
    size VARCHAR(32),
    setting VARCHAR(32),
    primary key (cId)
);

insert into Category (category, "trigger", size, setting) SELECT distinct LANDSLIDE_CATEGORY, LANDSLIDE_TRIGGER, LANDSLIDE_SIZE, LANDSLIDE_SETTING from EVERYTHING

create table IS_A (
    cId integer not null,
    eId integer not null,
    foreign key (eId) references Events,
    foreign key (cId) references Category,
    primary key (cId, eId)
);

insert into IS_A (cId, eId) select cId, EVENT_ID from CATEGORY c, EVERYTHING e
    where LANDSLIDE_CATEGORY = category and
        LANDSLIDE_TRIGGER = "trigger" and
        LANDSLIDE_SIZE = size and
        LANDSLIDE_SETTING = setting;


select c.countryName, count(x."trigger") as times_smited from Country c, Location l, IS_A i, Category x where c.code = l.code and l.eId = i.eId and i.CID = x.cId and x."trigger" in ('volcano', 'earthquake', 'flooding', 'monsoon', 'tropical_cyclone', 'dam_embankment_collapse') group by c.countryName order by times_smited desc;


select countryName, count(category.size) as Big_Ones from Country, Location, IS_A, Category where Country.code = Location.code and Location.eId = IS_A.eId and IS_A.cId = CATEGORY.cId and category.size in ('catastrophic', 'very_large', 'large') group by countryName order by Big_Ones desc;




create table Import(
    eIdImport integer not null,
    ISource VARCHAR(8),
    submitTime VARCHAR(32),
    createTime VARCHAR(32),
    editTime VARCHAR(32),
    foreign key (eIdImport) references EVENTS(eId),
    primary key (eIdImport)
);

/*
create table Source (
    url VARCHAR(1024) not null,
    sourceName VARCHAR(64),
    photoLink VARCHAR(1024),
    primary key (url)
); */


create table Division (
    divName VARCHAR(64),
    divPop integer,
    gClosest VARCHAR(64),
    gDist double,
    code varchar(8),
    eId integer not null,
    foreign key (code) references Country(code),
    foreign key (eId) references EVENTS(eId),
    primary key (eId)
);

drop table DIVISION;

create table Report (
    eId integer not null,
    url varchar(1024),
    sourceName VARCHAR(64),
    photoLink VARCHAR(1024),
    foreign key (eId) references Events(eId),
    primary key (eId)
);

alter table Report drop column url;
alter table Report add column url varchar(1024);

create table Country (
    code varchar(8) not null,
    countryName VARCHAR(64),
    primary key (code)
);

create table Category (
    category VARCHAR(32),
    "TRIGGER" VARCHAR(32),
    size VARCHAR(32),
    setting VARCHAR(32),
    eId integer not null,
    foreign key (eId) references Events(eId),
    primary key (eId)
);

create table Location (
    long double not null,
    lat double not null,
    lDesc varchar(1024),
    lAcc vARCHAR(32),
    eId integer not null,
    code varchar(8),
    foreign key (eid) references Events(eId),
    foreign key (code) references Country(code),
    primary key (long, lat, eId)
);

delete from DIVISION where divName is null and divPop is null and gClosest is null and gDist is null and code is null;
commit;


select ADMIN_DIVISION_POPULATION from EVERYTHING;

insert into Events (eId, eTitle, eDesc, eDate, fatality, injury, notes)
    select EVENT_ID, EVENT_TITLE, EVENT_DESCRIPTION, EVENT_DATE, FATALITY_COUNT, INJURY_COUNT, NOTES from EVERYTHING;

insert into LOCATION (long, lat, lDesc, lAcc, eId, code) SELECT LONGITUDE, LATITUDE, LOCATION_DESCRIPTION, LOCATION_ACCURACY, EVENT_ID, COUNTRY_CODE from EVERYTHING;
insert into Storm (stormName, eId) select convert(STORM_NAME, varchar(64)), EVENT_ID from EVERYTHING where STORM_NAME is not null;
insert into REPORT (eId, url, sourceName, photoLink) select distinct EVENT_ID, SOURCE_LINK, SOURCE_NAME, PHOTO_LINK from EVERYTHING where SOURCE_LINK is not null or SOURCE_NAME is not null or PHOTO_LINK is not null ;
insert into IMPORT (eIdImport, ISource, submitTime, createTime, editTime) select EVENT_ID, EVENT_IMPORT_SOURCE, SUBMITTED_DATE, CREATED_DATE, LAST_EDITED_DATE from EVERYTHING;
insert into DIVISION (divName, divPop, gClosest, gDist, code, eId) select ADMIN_DIVISION_NAME, ADMIN_DIVISION_POPULATION, GAZETEER_CLOSEST_POINT, GAZETEER_DISTANCE, COUNTRY_CODE, EVENT_ID from EVERYTHING;
insert into CATEGORY (category, "TRIGGER", size, setting, eId) SELECT distinct LANDSLIDE_CATEGORY, LANDSLIDE_TRIGGER, LANDSLIDE_SIZE, LANDSLIDE_SETTING, EVENT_ID from EVERYTHING
    where LANDSLIDE_CATEGORY is not null or LANDSLIDE_TRIGGER is not null or LANDSLIDE_SIZE is not null or LANDSLIDE_SETTING is not null;
insert into COUNTRY (code, countryName) select distinct convert(COUNTRY_CODE, varchar(8)), COUNTRY_NAME from EVERYTHING where COUNTRY_CODE is not null and COUNTRY_NAME is not null and COUNTRY_CODE != 'CZ';
insert into COUNTRY (code, countryName) VALUES ('CZ', 'Czech Republic');

select * from EVERYTHING;

select COUNTRY_NAME, count(distinct EVENT_ID) as c from EVERYTHING where COUNTRY_NAME is not null
group by COUNTRY_NAME order by c desc;

select COUNTRY_NAME from EVERYTHING except (
    select distinct COUNTRY_NAME
    from EVERYTHING
    where STORM_NAME is not null
);

select count(*) from EVERYTHING where COUNTRY_CODE is null;
select count(*) from EVERYTHING where COUNTRY_NAME is null;

select distinct COUNTRY_NAME, COUNTRY_CODE from EVERYTHING where COUNTRY_NAME is not null ;

select COUNTRY_CODE, COUNTRY_NAME from EVERYTHING;

insert into Country (CODE, COUNTRYNAME)  select distinct COUNTRY_CODE, COUNTRY_NAME from EVERYTHING where COUNTRY_CODE is not null and COUNTRY_NAME is not null;

/*Finds all the countries that have storms*/
select countryName from COUNTRY, LOCATION, STORM where Country.code = Location.code and Location.eId = STORM.eId;

select distinct SOURCE_LINK, SOURCE_NAME, PHOTO_LINK from EVERYTHING where SOURCE_LINK is null limit 2000;

select fatality from Events where fatality is not null order by fatality;

select eId as Christmas_Miracles, eDate from Events where regexp_matches(convert(eDate, varchar(64)), '^12/25/.*') and (Fatality is null or Fatality = 0) and (Injury is null or Injury = 0);

select LANDSLIDE_CATEGORY, LANDSLIDE_TRIGGER, LANDSLIDE_SIZE, LANDSLIDE_SETTING from EVERYTHING
where (LANDSLIDE_CATEGORY is null) or (LANDSLIDE_TRIGGER is null) or (LANDSLIDE_SIZE is null) or (LANDSLIDE_SETTING is null);

select distinct COUNTRY_CODE from EVERYTHING where COUNTRY_CODE is not null;
select distinct COUNTRY_CODE, COUNTRY_NAME from EVERYTHING where COUNTRY_CODE is not null and COUNTRY_NAME is not null;

select SOURCE_NAME, count(SOURCE_LINK) as reports from EVERYTHING group by SOURCE_NAME order by reports desc;



select distinct countryName from Country, Location where Country.code = Location.code
                                                    and Location.eId not in (select eId from STORM);

select distinct countryName from Country where code in(
select distinct code from Country except (
select distinct code from Location where eId in ( select eId from STORM)));


select c.countryName, count(l.lAcc) as acc_count from Location l, Country c where c.code = l.code and l.lAcc = 'exact' group by c.countryName order by acc_count desc;

select c.countryName, count(x."TRIGGER") as times_smited from Country c, Location l, Category x where c.code = l.code and l.eId = x.eId and x."TRIGGER" in ('volcano', 'earthquake', 'flooding', 'monsoon', 'tropical_cyclone', 'dam_embankment_collapse') group by c.countryName order by times_smited desc;

select distinct "TRIGGER" from CATEGORY;

select distinct size from Category;

select countryName, count(category.size) as Big_Ones from Country, Location, Category where Country.code = Location.code and Location.eId = Category.eId and category.size in ('catastrophic', 'very_large', 'large') group by countryName order by Big_Ones desc;


select fatality from Events where fatality is not null order by fatality desc;

select r.sourceName, sum(isNull(e.fatality, 0)) as sum_fatality, sum(isNull(e.injury, 0)) as sum_injury from REPORT r, Events e where r.eId = e.eId group by r.sourceName order by sum_fatality desc;

select "TRIGGER" from Category where "TRIGGER" in ('monsoon', 'tropical_cyclone');

select lDesc ,sqrt(power(lat, 2) + power(long, 2)) as Distance from Location order by Distance;



