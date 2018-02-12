#include "Utils.h"
#include <iomanip>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

    /*
    // test
    //TxtHandler *file = new TxtHandler("/home/xinzhan/Downloads/indiv16/itcont.txt");
    //TxtHandler *file = new TxtHandler("test.txt", fstream::out);
    //TxtHandler *file = new TxtHandler("itcont.txt");

    string tmp;
    while(file->GetLine(tmp)){
	Entry entry_tmp;
	if(RecordParser::Instance()->ParseLine(tmp, entry_tmp)){
	    cout<<entry_tmp;
	}
    }

    Records::Instance()->TestHeap();
    */

    if(argc != 4) {
	cout<<"Please specify input, output and percentile file paths."
	    <<endl
	    <<"Usage: "
	    <<endl
	    <<"      ./main input percentile output"
	    <<endl;
	exit(-1);
    }

    /* 
    // test
    string input = "/home/xinzhan/Downloads/indiv16/itcont.txt";
    string output = "output.txt";
    string percent_file = "percentile.txt";
    */

    string input = string(argv[1]);
    string output = string(argv[3]);
    string percent_file = string(argv[2]);

    Analyzer *ana = new Analyzer(input, output, percent_file);

    ana -> Analyze();

    return 0;
}
