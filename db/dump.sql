--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: triplist; Type: DATABASE; Schema: -; Owner: postgres
--

CREATE DATABASE triplist WITH TEMPLATE = template0 ENCODING = 'UTF8' LC_COLLATE = 'en_US.UTF-8' LC_CTYPE = 'en_US.UTF-8';


ALTER DATABASE triplist OWNER TO postgres;

\connect triplist

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: triplist; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON DATABASE triplist IS 'База данных выездных листов';


--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: staff; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE staff (
    id integer NOT NULL,
    imia character varying NOT NULL,
    familia character varying NOT NULL,
    otchestvo character varying NOT NULL,
    birth date NOT NULL,
    pol "char" DEFAULT 'F'::"char" NOT NULL,
    CONSTRAINT staff_pol_check CHECK ((pol = ANY (ARRAY['M'::"char", 'F'::"char"])))
);


ALTER TABLE public.staff OWNER TO postgres;

--
-- Name: TABLE staff; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE staff IS 'Сотрудники';


--
-- Name: COLUMN staff.id; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.id IS 'Внутренний идентификатор БД';


--
-- Name: COLUMN staff.imia; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.imia IS 'Имя';


--
-- Name: COLUMN staff.familia; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.familia IS 'Фамилия';


--
-- Name: COLUMN staff.otchestvo; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.otchestvo IS 'Отчество';


--
-- Name: COLUMN staff.birth; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.birth IS 'Дата рождения';


--
-- Name: COLUMN staff.pol; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN staff.pol IS 'Пол';


--
-- Name: staff_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE staff_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.staff_id_seq OWNER TO postgres;

--
-- Name: staff_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE staff_id_seq OWNED BY staff.id;


--
-- Name: trip; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE trip (
    id integer NOT NULL,
    staff_id integer NOT NULL,
    strana character varying NOT NULL,
    nachalo date NOT NULL,
    konets date NOT NULL,
    tsel character varying NOT NULL
);


ALTER TABLE public.trip OWNER TO postgres;

--
-- Name: TABLE trip; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON TABLE trip IS 'Поездки персонала';


--
-- Name: COLUMN trip.id; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.id IS 'Внутренний идентификатор БД';


--
-- Name: COLUMN trip.staff_id; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.staff_id IS 'Код сотрудника (staff.id)';


--
-- Name: COLUMN trip.strana; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.strana IS 'Страна назначения';


--
-- Name: COLUMN trip.nachalo; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.nachalo IS 'Дата начала';


--
-- Name: COLUMN trip.konets; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.konets IS 'Дата окончания';


--
-- Name: COLUMN trip.tsel; Type: COMMENT; Schema: public; Owner: postgres
--

COMMENT ON COLUMN trip.tsel IS 'Цель поездки';


--
-- Name: trip_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE trip_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.trip_id_seq OWNER TO postgres;

--
-- Name: trip_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE trip_id_seq OWNED BY trip.id;


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY staff ALTER COLUMN id SET DEFAULT nextval('staff_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY trip ALTER COLUMN id SET DEFAULT nextval('trip_id_seq'::regclass);


--
-- Data for Name: staff; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO staff (id, imia, familia, otchestvo, birth, pol) VALUES (2, 'Иван', 'Иванов', 'Иванович', '1970-01-01', 'F');
INSERT INTO staff (id, imia, familia, otchestvo, birth, pol) VALUES (6, 'Николай', 'Сидоров', 'Алексеевич', '1981-06-03', 'M');
INSERT INTO staff (id, imia, familia, otchestvo, birth, pol) VALUES (5, 'Леонид', 'Семенов', 'Семенович', '2005-01-01', 'F');


--
-- Name: staff_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('staff_id_seq', 6, true);


--
-- Data for Name: trip; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO trip (id, staff_id, strana, nachalo, konets, tsel) VALUES (4, 2, 'сша', '1997-01-01', '1999-01-01', 'туризм');
INSERT INTO trip (id, staff_id, strana, nachalo, konets, tsel) VALUES (8, 6, 'Украина', '2012-04-01', '2012-04-06', 'Туризм');
INSERT INTO trip (id, staff_id, strana, nachalo, konets, tsel) VALUES (6, 5, 'Китай', '2000-01-01', '2000-01-01', 'Бизнес');
INSERT INTO trip (id, staff_id, strana, nachalo, konets, tsel) VALUES (7, 5, 'Беларусь', '1997-01-01', '1999-01-01', 'Поездка домой');


--
-- Name: trip_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('trip_id_seq', 8, true);


--
-- Name: staff_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY staff
    ADD CONSTRAINT staff_pkey PRIMARY KEY (id);


--
-- Name: trip_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres; Tablespace: 
--

ALTER TABLE ONLY trip
    ADD CONSTRAINT trip_pkey PRIMARY KEY (id);


--
-- Name: trips_fkey1; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY trip
    ADD CONSTRAINT trips_fkey1 FOREIGN KEY (staff_id) REFERENCES staff(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

