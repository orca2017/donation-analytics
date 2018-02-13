
This program requires C++ 11, at least.

Program takes about 10 minutes to process 20,000,000 records/lines
(that is a ~4GB txt file, consumes max 562MB memory, on an intel core i7 6700 desktop).
Memory consumption mainly due to saved hash maps; so more records processed, more memory consumption.


-------------------------------------------------------------------------------------------------------

Program is organized in 4 classes: TxtHandler, RecordParser, RepeatDonor, Analyzer,
and one C++ struct named Entry.

struct Entry:

    This struct stores all 5 fields that we interested.
    cmte_id, name, zip_code, data, transaction_amt, and other id. All in string format.

class TxtHandler:

    This is a wrapper class for c++ fstream, handles input and output of text files.
    To simplify code, only two modes are supported: read mode and write mode. 
    Class returns each record in a string format.

class RecordParser:

    This is a Singleton class, this class parse each entry that comes from TxtHandler, and extracts
    the interested fields following the rules as described by FEC website.
    This class also do validation check for each entry, only donations coming from people 
    are valid, it also checks the format of date, name, transaction amount and cmte_id;

class RepeatDonor:

    This is also a Singleton class, it keeps a hash map. For each record, it will check its
    internal hash map, if it finds the corresponding donor, then the record comes from a 
    repeat donor, if not found, then it will update the record to the hash map, and return false.

    The hash map is implemented using c++11 unordered_map<string, int>, the key is a string, and 
    the value is an int. value = earlist year. key = name+zip_code.

    For the hash map, The reason that I keep "val = year" is, according to readme.txt in github, 
    whether we treat a record is repeat or not depends on whether the same donor makes donations in 
    previous years. So while checking, this class will compare the year 
    of current record with the year stored in the hash map, if it is earlier than the one in
    hash map, class will return false and update hash map to the earlier year. 

class Records:

    This is also a Singleton class, it keeps two hash maps, hash_map_1 is:
    unordered_map<string, vector<int>>
    where key = cmte_id + zip_code + year; value = a vector<int>, which stores each donation dollar amount

    hash_map_2 is: 
    unordered_map<string, int>
    where key is same with hash_map_1, but value = total dollar of donation

    This class also calculates percentile, using infomation in hash_map_1. In hash_map_1, the
    vector<int> was implemented using a Min Heap data structure for faster insert. Because for
    each entry, we need to calculate the current percentile, and we need a sorted vector. Inserting
    a heap is O(logN), while inserting a vector is O(N).

    hash_map_2 is purely for faster total dollar calculation, if we have this map, then we don't need
    to sum all vector elements each time. This sacrifices space complexity for a better time complexity.

class Analyzer:

    This is the overall entrance class, everything is called inside this class's member Analyze()

    it first calls TxtHandler to get an entry, then pass this entry to RecordParser, then to RepeatDonor,
    then to Records, and then output the corresponding information of the current record
    to "repeat_donor.txt" file.

    This class also read percentile.txt, and saves the percentile number to be calculated.

-------------------------------------------------------------------------------------------------------
A few extra words about this program:

  Besides implementing all the requirements as described on Code Challenge github, I also have two
  more checks:

    1, This code challenge said checking DATE format, I assume all dates must be in MMDDYYYY format,
       otherwise treat it as invalid format. Because challenge does not specify what formats are
       correct, and there are various of ways to express date, it is unreasonalbe to exhaust all 
       valid formats.
    2, Chanllenge also said checking NAME format, I only require name is not empty, reason is same 
       as the above one.



