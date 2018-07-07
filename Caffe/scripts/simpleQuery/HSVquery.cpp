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

vector<imgstruct> results;

int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for(i = 0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    // add result to global datastructure
    imgstruct row = {argv[0], argv[1], stof(argv[2]), stof(argv[3]),stof(argv[4]), stof(argv[5])};
    results.push_back(row);

    printf("\n");
    return 0;
}

//https://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c (user Pavel)
static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

int main(int argc, char ** argv){
    if(argc < 6){
        cout << "Please specify: db, shot-dir and hsv.\n";
        cout << "./HSVquery ../vr.db ../shots/ 204 0.45 0.66\n";
        exit(0);
    }
    string dbPath = argv[1];
    string shotPath = argv[2];
    if(!endsWith(shotPath,"/")) shotPath += "/"; // ensure shotPath ends with an '/'
    float h = stof(argv[3]);
    float s = stof(argv[4]);
    float v = stof(argv[5]);


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

    /* Create SQL statement */
    float tolerance = 0.2;
    float lower_h = h * (1-tolerance);
    float upper_h = h * (1+tolerance);
    float lower_s = s * (1-tolerance);
    float upper_s = s * (1+tolerance);
    float lower_v = v * (1-tolerance);
    float upper_v = v * (1+tolerance);
    // only checking for hue, more possible/neccessary
    sql = "SELECT * from images where (h BETWEEN " + to_string(lower_h) + " AND " + to_string(upper_h) +")";
    sql += " AND (s BETWEEN " + to_string(lower_s) + " AND " + to_string(upper_s)+")";
    sql += " AND (v BETWEEN " + to_string(lower_v) + " AND " + to_string(upper_v)+")";

    cout << sql << "\n";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);

    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);


    // maybe sort results by color-difference to target-color?
    for(imgstruct &st : results){
        cout << st.filename << std::endl;
        Mat img = imread(shotPath + st.filename, -1);
        imshow(st.filename, img);
    }
    waitKey(0);

    return 0;
}