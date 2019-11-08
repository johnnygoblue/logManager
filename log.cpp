  // Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm> // replace, sort
#include <sstream> // istringstream

#include "log.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::getline;
using std::to_string;
using std::replace;
using std::sort;
using std::pair;

const int ts_num_digits = 14;

/* overload ostream */
std::ostream& operator<<(std::ostream& os, const Log& l) {
	string ts = getTimeStamp(l.ts);
	os << ts << "|" << l.cat << "|" << l.msg << "\n";
	return os;
}

/* get options */
void LogMan::getOptions(int argc, char **argv) {
	int option_index = 0, option = 0;
	opterr = false;

	struct option longOpts[] = {{ "help", no_argument, nullptr, 'h'},
								{ nullptr, 0, nullptr, '\0'}};

	while ((option = getopt_long(argc, argv, "vs:mh", longOpts, &option_index)) != -1) {
		switch (option) {
			case 'h':
				cout << "Log Manager\n";
				cout << "Please use input log file for parsing!\n";
				exit(0);
			default:
				cerr << "Unrecognized option '" << option << "'" << endl;
				exit(1);
		} // switch
	} // while
}

/* read log entries from input */
void LogMan::parseLogEntries(const string &filename) {
	std::ifstream ifs(filename, std::ifstream::in);
	string t = "";
	string cat = "";
	string msg = "";
	uint64_t num_entries = 0;

	while (getline(ifs, t, '|')) {
		getline(ifs, cat, '|');
		getline(ifs, msg);
		Log entry = Log(cat, msg, transformTimeStamp(t), num_entries);
		master_log.push_back(entry);
		++num_entries;
	}
	ifs.close();
	cout << num_entries << " entries read\n";
}

/* sort master log by timestamp */
void LogMan::sortMasterLog() {
	sort(master_log.begin(), master_log.end(), Log::CompareLogEntries());
	//for (size_t i = 0; i < master_log.size(); ++i) {
	//	cout << master_log[i];
	//}
}

/* prompt command from user input */
void LogMan::promptCmd() {
	//string input;
	char cmd;

	do {
		cout << "% ";
		cin >> cmd;
		//getline(cin, input);
		handleCmd(cmd);
	} while (cmd != 'q');
}

void LogMan::handleCmd(const char cmd/*, const string &input*/) {
	//std::istringstream iss(input);
	switch (cmd) {
		/* searching commands */
		case 't': {
			string t1;
			string t2;
			cin >> t1 >> t2;
			tCmdHandle(t1, t2);
			break;
				  }
		case 'm':
			break;
		case 'c':
			break;
		case 'k':
			break;
		/* excerpt list commands */
		case 'a':
			break;
		case 'r':
			break;
		case 'd':
			break;
		case 'b':
			break;
		case 'e':
			break;
		case 's':
			break;
		case 'l':
			break;
		case 'g':
			break;
		case 'p':
			break;
		case 'q':
			break;
		case '#':
			break;
		default:
			cerr << "Unrecognized command '" << cmd << "'!\n";
			break;
	} // switch
}

/************************************************************************
*************** Definitions of utility functions ************************
************************************************************************/

/* converts internal timestamp in uint64_t to original string format */
string getTimeStamp(uint64_t t) {
	string ts = to_string(t);
	while (ts.size() != ts_num_digits) {
		ts.insert(ts.begin(), '0');
	} // while
	assert(ts[2] == '1');
	assert(ts[5] == '1');
	assert(ts[8] == '1');
	assert(ts[11] == '1');
	ts[2] = ':';
	ts[5] = ':';
	ts[8] = ':';
	ts[11] = ':';
	return ts;
}

/* transform the string timestamp into uint64_t with ':' replaced by '1' */
uint64_t transformTimeStamp(string t) {
	replace(t.begin(), t.end(), ':', '1');
	uint64_t ts = stoull(t, NULL, 10);
	return ts;
}

/* check if timestamp has the right number of digits */
bool isValidTimeStamp(string t) {
	if (t.length() != ts_num_digits) {
		cerr << "Invalid timestamp detected!\n";
		return false;
	}
	return true;
}

/* return string content in lower case */
string getLower(const string &s) {
	string ret = s;
	for (auto &c : ret) {
		tolower(c);
	}
	return ret;
}
