#!/bin/bash
rm ./vr.db

rm ./shots/*

sqlite3 vr.db "CREATE TABLE images(name TEXT PRIMARY KEY NOT NULL, concept TEXT NOT NULL);"

sqlite3 vr.db "CREATE INDEX concept_index ON images(concept);"