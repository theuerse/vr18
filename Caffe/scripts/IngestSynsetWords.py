import sqlite3
import sys

if len(sys.argv) < 3:
    print("python " + sys.argv[0] + " ./vr.db ./synset_words")
    exit(-1)

db_path = sys.argv[1]
synset_words_filepath = sys.argv[2]

conn = sqlite3.connect('./vr.db')
c = conn.cursor()
cnt = 0

with open(synset_words_filepath,"r") as  conceptFile:
    for line in conceptFile.readlines():
        parts = line.rstrip().split()
        if len(parts) >= 2:
            c.execute('insert or replace into synset_words values (?,?)',[parts[0], " " .join(parts[1:])])
            cnt += 1

conn.commit()
conn.close()

print("added " + str(cnt) + " words from " + synset_words_filepath + " to " + db_path)
