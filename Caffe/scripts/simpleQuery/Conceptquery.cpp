#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <sqlite3.h>

using namespace std;
using namespace cv;

struct imgstruct{
    string filename;
    string concept;
    float concept_confidence;
    float h;
    float s;
    float v;
};

std::set<string> imgconcepts;
std::unordered_map<string,string> wordToConceptMap;

vector<imgstruct> results;


int ConceptCallback(void *data, int argc, char **argv, char **azColName){
    /*int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }*/

    // add result to global datastructure
    imgconcepts.insert(argv[0]);

    //printf("\n");
    return 0;
}

int WordToConceptCallback(void *data, int argc, char **argv, char **azColName){
    string concept = argv[0];

    std::stringstream words(argv[1]);
    std::string segment;
    std::vector<std::string> seglist;

    cout << concept << "\t" << argv[1] << "\n";
    while(std::getline(words, segment, ','))
    {
        segment.erase(segment.begin(), std::find_if(segment.begin(), segment.end(), std::bind1st(std::not_equal_to<char>(), ' '))); // erase leading white-space
        seglist.push_back(segment);
        wordToConceptMap.insert(std::make_pair(segment,concept));
    }

    return 0;
}

int callback(void *data, int argc, char **argv, char **azColName){
    /*int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }*/

    // add result to global datastructure
    imgstruct row = {argv[0], argv[1], stof(argv[2]), stof(argv[3]),stof(argv[4]), stof(argv[5])};
    results.push_back(row);

    //printf("\n");
    return 0;
}

//https://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c (user Pavel)
static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}


int main(int argc, char ** argv){
    if(argc < 3){
        cout << "Please specify: db, shot-dir\n";
        cout << "./HSVquery ../vr.db ../shots/\n";
        exit(0);
    }
    string dbPath = argv[1];
    string shotPath = argv[2];
    if(!endsWith(shotPath,"/")) shotPath += "/"; // ensure shotPath ends with an '/'

    // start query (https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm)
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    string sql;
    const char* data = "Callback function called";

    /* Open database */
    rc = sqlite3_open(dbPath.c_str(), &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Select all concepts avail in database */
    // only checking for hue, more possible/neccessary
    sql = "SELECT DISTINCT concept from images";
    cout << sql << "\n";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), ConceptCallback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }


    // display available concepts
    String conceptList="";
    for(string concept : imgconcepts){
        if(conceptList.size()>0) conceptList += ",";
        conceptList +=  "'"+concept+"'";
    }


    sql = "SELECT * from synset_words WHERE concept in (" + conceptList +")";
    cout << sql << "\n";

    /* Execute SQL statement */
    cout << "\nAvailable concepts:\n";
    rc = sqlite3_exec(db, sql.c_str(), WordToConceptCallback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }

    // ask user for concept to search for
    cout << "please enter a word to search for: \n";
    string word;
    std::getline(std::cin, word);

    unordered_map<string, string>::iterator it = wordToConceptMap.find(word);

    if ( it == wordToConceptMap.end() )
        std::cout << "not found";
    else{
        sql = "SELECT * FROM images WHERE concept == '" + it->second +"';";
        cout << sql << "\n";

        rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);

        if( rc != SQLITE_OK ) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            fprintf(stdout, "Operation done successfully\n");
        }
    }

    sqlite3_close(db);

    // maybe sort results by concept_confidence?
    // maybe use synonym hierarchy to find "parent-concepts"? car-mirror and ear are quite specific
    for(imgstruct &st : results){
        cout << st.filename << std::endl;
        Mat img = imread(shotPath + st.filename, -1);
        imshow(st.filename, img);
    }
    waitKey(0);

    return 0;
}