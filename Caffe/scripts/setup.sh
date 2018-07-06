#!/bin/bash
rm ./vr.db

rm ./shots/*

sqlite3 vr.db "CREATE TABLE images(name TEXT PRIMARY KEY NOT NULL, concept TEXT NOT NULL, concept_confidence REAL NOT NULL, h REAL NOT NULL, s REAL NOT NULL, v REAL NOT NULL);"

sqlite3 vr.db "CREATE INDEX concept_index ON images(concept);"
sqlite3 vr.db "CREATE INDEX h_index ON images(h);"
sqlite3 vr.db "CREATE INDEX s_index ON images(s);"
sqlite3 vr.db "CREATE INDEX v_index ON images(v);"