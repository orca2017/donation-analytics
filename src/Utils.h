#ifndef _UTILS_H__
#define _UTILS_H__

#include <string>
#include <fstream>
#include <ios>
#include <vector>
#include <unordered_map>

//===========================================================
//    A Data Struct for storing interested info
//===========================================================
struct Entry{
    std::string cmte_id;
    std::string name;
    std::string zip_code;
    std::string date;
    std::string transaction_amt;
    std::string other_id;

    Entry(){
	cmte_id = ""; name = ""; zip_code = ""; date = "";
	transaction_amt = "";
	other_id = "";
    }

    Entry(std::string &s1, std::string &s2, std::string &s3,
	    std::string &s4, std::string &amt, std::string &s5)
	: cmte_id(s1), name(s2), zip_code(s3), date(s4),
	transaction_amt(amt), other_id(s5){
	}

    std::string Year() const {
	return date.substr(4, 4);
    }
};

std::ostream &operator<<(std::ostream &os, const Entry &e);


//===========================================================
//    A Wrapper Class for handling text files
//===========================================================
class TxtHandler{

public:
    TxtHandler();
    TxtHandler(const char *path, std::ios_base::openmode mode = std::fstream::in);
    ~TxtHandler();

    bool GetLine(std::string &);
    void LoadFile(const char *path);
    void WriteLine(std::string &s);
    void CreateFile(const char *path);

    // show progress
    void ShowProgress();

private:
    std::string path; // reserved
    std::fstream _file;

    std::streampos _file_length;
    std::streampos _current_pos;

    unsigned long long lines = 0;
};


//===============================================================
// A singleton Class for parsing and filtering each donor record
//===============================================================
class RecordParser{
public:
    ~RecordParser();
    static RecordParser * Instance(){
	if(_instance == nullptr)
	    _instance = new RecordParser();
	return _instance;
    }

    bool ParseLine(const std::string &in, Entry & rec);
    bool IsValidEntry(const std::string &in, Entry &rec);
    void _ParseFEC(const std::string &in, Entry &rec);
    std::vector<std::string> Delimit(const std::string &in);
    std::string Strip(const std::string &in);

    bool IsValidDate(const std::string &in);
    bool IsValidName(const std::string &in);

private:
    RecordParser(){};
    static RecordParser *_instance;
};



//===============================================================
// A singleton Class to identifiy if a donor is a repeat donor
//===============================================================
class RepeatDonor{
public:
    ~RepeatDonor();
    static RepeatDonor *Instance(){
	if(_instance == nullptr)
	    _instance = new RepeatDonor();
	return _instance;
    }

    std::string _hash(const Entry &e);
    bool IsRepeatDonor(const Entry &e);


private:
    RepeatDonor(){};
    static RepeatDonor *_instance;
    std::unordered_map<std::string, short> dic;
};


//===================================================================
//A Singleton Class to keep records of each entry from repeat donors
//===================================================================
class Records{
public:
    ~Records();
    static Records* Instance(){
	if(_instance == nullptr)
	    _instance = new Records();
	return _instance;
    }

    std::string _hash(const Entry &e);
    void Push(const Entry &e);
    std::unordered_map<std::string, std::vector<int>> &GetRecords();
    void InsertAmt(std::vector<int> &v, int &amt);

    // Use a Min Heap data structure to insert element
    void BubbleUp(std::vector<int> &v, int &index);
    void Swap(int &a, int &b);
    void TestHeap();

    int GetTotalDollars(std::string key);
    int GetNumContribs(std::string key);
    int GetPercentile(std::string key, int &percent);


private:
    Records(){};
    static Records *_instance;
    std::unordered_map<std::string, std::vector<int>> dic;
    std::unordered_map<std::string, int> dic_tot_amt;
};



//====================================================================
// An overall Class to organize each repeat donor entry to output file
//====================================================================
class Analyzer{
public:
    Analyzer(std::string in, std::string out, std::string percent_file);
    ~Analyzer();

    void Analyze();
    std::string FormOutputEntry(const Entry &e);

    void SetInputFile(const char *path);
    void SetOutputFile(const char *path);
    void GetPercentile(const char *path);

private:
    std::string input_path;
    std::string output_path;
    std::string percentile_path;
    TxtHandler *output;
    TxtHandler *input;
    int percentile;

    RecordParser *_record_parser;
    RepeatDonor *_repeat_donor;
    Records *_records;
};


#endif
