#include "Utils.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>


using namespace std;


//=======================================================================
//                       The Entry Struct
//=======================================================================
std::ostream &operator<<(std::ostream &os, const Entry &e){
    // format printing for struct
    os<<setfill(' ')<<setw(20)<<e.cmte_id
	<<setfill(' ')<<setw(20)<<e.name
	<<setfill(' ')<<setw(20)<<e.zip_code
	<<setfill(' ')<<setw(20)<<e.date
	<<setfill(' ')<<setw(20)<<e.transaction_amt
	<<setfill(' ')<<setw(20)<<e.other_id
	<<endl;
    return os;
}



//=======================================================================
//                        Class TxtHandler 
//=======================================================================
TxtHandler::TxtHandler(){
    // place holder
}


TxtHandler::TxtHandler(const char *path, ios_base::openmode mode){

    TxtHandler();

    if(mode != fstream::in && mode != fstream::out){
	cout<<"TxtHandler: only read and write mode are supported."
	    <<endl;
	cout<<"            use mode fstream::in or fstream::out."
	    <<endl;
	exit(-1);
    }

    if(mode == fstream::in)
	LoadFile(path);
    else
	CreateFile(path);
}


TxtHandler::~TxtHandler(){
    // place holder
}


void TxtHandler::LoadFile(const char *path){
    this->path = path;

    try{
	_file.open(path, fstream::in);

	if(!_file.is_open()) 
	    cout<<"ERROR: cannot open file: "<<path<<", check your path."<<endl;

	// get file size information
	_file.seekg(0, _file.end);
	_file_length = _file.tellg();
	_file.seekg(0, _file.beg);
	_current_pos = _file.tellg();
    }
    catch(...){
	cout<<"File open exception, check input file."<<endl;
    }
}


void TxtHandler::CreateFile(const char *path){
    this->path = path;

    try{
	_file.open(path, fstream::out);

	if(!_file.is_open())
	    cout<<"ERROR: cannot create file: "<<path<<", check your path."<<endl;
    }
    catch(...){
	cout<<"File: \""<<path<<"\" creation Exception, check your path."<<endl;
    }
}


bool TxtHandler::GetLine(string &_tmp){

    if(_current_pos >= _file_length){
	cout<<"Success! Done with processing file \""<<path<<"\". (*^_^*)"<<endl;
	return false;
    }

    ShowProgress();

    _file.seekg(_current_pos, _file.beg);
    getline(_file, _tmp);
    _current_pos = _file.tellg();

    return true;
}


void TxtHandler::WriteLine(string &s){
    _file<<s<<endl;
}


void TxtHandler::ShowProgress(){

    lines++;
    if(lines >0 && lines %10000 == 0){
	cout<<"------------- ";
	cout<<lines<<" lines processed. ";
	cout<<"-------------\r";
	cout.flush();
    }
}



//=======================================================================
//                        Class RecordParser
//=======================================================================
RecordParser* RecordParser::_instance = nullptr;



RecordParser::~RecordParser(){
    // place holder
}



bool RecordParser::ParseLine(const string &in, Entry &rec){

    return IsValidEntry(in, rec);
}



bool RecordParser::IsValidEntry(const string &in, Entry &rec){

    _ParseFEC(in, rec);

    // filter entries according to criteria as described by the challenge on github
    if( rec.zip_code.size() < 5) return false;
    else rec.zip_code = rec.zip_code.substr(0, 5);

    if( rec.other_id.size() > 0) return false;
    if( !IsValidDate(rec.date) ) return false;
    if( !IsValidName(rec.name) ) return false;
    if( rec.transaction_amt.size() <= 0) return false;
    if( (char)rec.transaction_amt[0] == '-') return false; // negative donation is invalid
    if( rec.cmte_id.size() <= 0) return false;

    return true;
}


bool RecordParser::IsValidDate(const string &in){

    if(in.size() <= 0) return false;
    if(in.size() != 8)  return false;

    // add other criteria here

    return true;
}


bool RecordParser::IsValidName(const string &in){

    if(in.size() <=0) return false;

    // add other criteria here

    return true;
}


void RecordParser::_ParseFEC(const string &in, Entry &rec){

    vector<string> tmp = Delimit(in);

    rec.cmte_id = Strip(tmp[0]);
    rec.name = Strip(tmp[7]);
    rec.zip_code = Strip(tmp[10]);
    rec.date = Strip(tmp[13]);
    rec.transaction_amt = Strip(tmp[14]);
    rec.other_id = Strip(tmp[15]);
}


vector<string> RecordParser::Delimit(const string &in){
    // parse line according to data dictionary as described by the FEC

    vector<string> _tmp;
    string ele;

    for(char i: in){
	if(i == '|'){
	    _tmp.push_back(ele);
	    ele.clear();
	    continue;
	}
	ele += i;
    }

    _tmp.push_back(ele); // push last field

    return _tmp;
}


string RecordParser::Strip(const std::string &in){
    // remove leading and trailing space

    const auto _begin = in.find_first_not_of(" \t");
    if(_begin == string::npos)
	return "";

    const auto _end = in.find_last_not_of(" \t");
    const auto _range = _end - _begin + 1;
    return in.substr(_begin, _range);
}



//=======================================================================
//                        Class RepeatDonor
//=======================================================================
RepeatDonor * RepeatDonor::_instance = nullptr;


RepeatDonor::~RepeatDonor(){

    dic.clear();
}


string RepeatDonor::_hash(const Entry &e){

    return e.zip_code+e.name;
}

bool RepeatDonor::IsRepeatDonor(const Entry &e){

    int year = stoi(e.Year());

    if(dic.find(_hash(e)) == dic.end()){
	dic[_hash(e)] = year;
	return false;
    }

    // This criteria based on github FAQ section, part 2
    // if Year of e is out of order, and not a prior year, skip it
    if( dic[_hash(e)] > year){
	dic[_hash(e)] = year;
	return false;
    }

    return true;
}


//=======================================================================
//                           Class Records
//=======================================================================
Records* Records::_instance = nullptr;


string Records::_hash(const Entry &e){
    return e.cmte_id + e.Year() + e.zip_code;
}


unordered_map<string, std::vector<int>> & Records::GetRecords(){
    return dic;
}


void Records::Push(const Entry &e){

    string index = _hash(e);
    int amt = static_cast<int>(round(stof(e.transaction_amt)));

    if(dic.find(index) == dic.end()){

	vector<int> tmp(1, amt);
	dic[index] = tmp;
	dic_tot_amt[index] = amt;
    } else {

	InsertAmt(dic[index], amt);
	dic_tot_amt[index] += amt;
    }
}

void Records::InsertAmt(vector<int> &v, int &amt){

    int s = v.size();
    if(s <= 0) {
	cout<<"Someting funny happened..."<<endl;
	exit(-1);
    }
    // insert element to heap
    v.push_back(amt);
    BubbleUp(v, s);
}


void Records::BubbleUp(vector<int> &v, int &index){
   
    // min heap, bubble up 
    int top = (index-1)/2;
    int cur_index = index;
    while(top >=0){

        // move leftward, for easier percentile calculation
	// not standard operation for heap
	if(cur_index -1 >= 0){
	    if( v[cur_index -1] > v[cur_index]){
		Swap(v[cur_index-1], v[cur_index]);
		cur_index--;
		top = (cur_index-1)/2;
		continue;
	    }
	}

        // move upward, bubble up
	if(v[top] > v[cur_index]){
	    Swap(v[top], v[cur_index]);
	    cur_index = top;
	    top = (cur_index-1)/2;
	} else {
	    break;
	}
    }
}


void Records::Swap(int &a, int &b){

    int tmp = a;
    a = b;
    b = tmp;
}


void Records::TestHeap(){

    // @test unit
    // test Min Heap insert
    int a[15] = {56, 3, 56, 2, 3, 512, 78, 90, 33, 11, 24, 56, 79, 2, 90};
    vector<int> v; v.push_back(100);
    int i=0;

    for(;i<15;i++){
	InsertAmt(v, a[i]);
    }
    for(auto &i: v){
	cout<<i<<", ";
    }
    cout<<endl;
}


int Records::GetTotalDollars(string key){

    if(dic_tot_amt.find(key) == dic_tot_amt.end()){
	cout<<"GetTotalDollars: Something funny happened..."<<endl;
	exit(-1);
    }
    return dic_tot_amt[key];
}

int Records::GetNumContribs(std::string key){
    if(dic.find(key)==dic.end()){
	cout<<"GetNumContribs: Something funny happened..."<<endl;
	exit(-1);
    }
    return dic[key].size();
}

int Records::GetPercentile(string key, int &percent){

    int N = GetNumContribs(key);

    // compute percentile as described by wikipedia
    int n = static_cast<int>(ceil( static_cast<float>(percent) / 100 *N));
    n -= 1;

    return dic[key][n];
}



//===========================================================
//                      Class Analyzer
//===========================================================
Analyzer::Analyzer(string in, string out, string pp){

    // set default file paths
    input_path = in;
    output_path = out;
    percentile_path = pp;

    SetInputFile(input_path.c_str());
    SetOutputFile(output_path.c_str());
    GetPercentile(percentile_path.c_str());

    _record_parser = RecordParser::Instance();
    _repeat_donor = RepeatDonor::Instance();
    _records = Records::Instance();
}


Analyzer::~Analyzer(){
}


void Analyzer::SetInputFile(const char *path){

    input = new TxtHandler(path);
}


void Analyzer::SetOutputFile(const char *path){

    output = new TxtHandler(path, fstream::out);
}


void Analyzer::GetPercentile(const char *path){

    // only one number in this file,
    // simple implement for this one

    TxtHandler f(path, fstream::in);

    string tmp;
    f.GetLine(tmp);

    tmp = RecordParser::Instance()->Strip(tmp);

    percentile = stof(tmp);
}


void Analyzer::Analyze(){

    // overall entrance
    string tmp;
    string record_string;
    while(input->GetLine(tmp)){
	Entry entry_tmp;

	if(_record_parser->ParseLine(tmp, entry_tmp)){

	    if(_repeat_donor->IsRepeatDonor(entry_tmp)){

		_records->Push(entry_tmp);

		record_string = FormOutputEntry(entry_tmp);
		output->WriteLine(record_string);
	    }
	}
    }
}


string Analyzer::FormOutputEntry(const Entry &e){

    string tmp = "";
    string key = _records->_hash(e);

    tmp += e.cmte_id;
    tmp += "|";
    tmp += e.zip_code;
    tmp += "|";
    tmp += e.Year();
    tmp += "|";
    tmp += to_string(_records->GetPercentile(key, percentile));
    tmp += "|";
    tmp += to_string(_records->GetTotalDollars(key));
    tmp += "|";
    tmp += to_string(_records->GetNumContribs(key));

    return tmp;
}
