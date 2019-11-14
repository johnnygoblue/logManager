  // Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm> // replace, sort
#include <sstream> // istringstream
#include <numeric> // iota
#include <cctype> // isalnum

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

/* read log entries from input file */
void LogMan::parseLogEntries(const string &filename) {
	std::ifstream ifs(filename, std::ifstream::in);
	string t = "";
	string cat = "";
	string msg = "";
	uint64_t num_entries = 0;

	while (getline(ifs, t, '|')) {
		getline(ifs, cat, '|');
		getline(ifs, msg);
		Log entry = Log(cat, msg, transformTimeStamp(t));
		master_log.push_back(entry);
		++num_entries;
	}
	ifs.close();
	cout << num_entries << " entries read\n";
}

/* sort master log by timestamp */
void LogMan::sortMasterLog() {
	log_idx_ts.resize(master_log.size());
	std::iota(log_idx_ts.begin(), log_idx_ts.end(), 0);
	Log::SortIndexByTimeStamp sibts(master_log);
	sort(log_idx_ts.begin(), log_idx_ts.end(), sibts);
	//for (size_t i = 0; i < log_idx_ts.size(); ++i) {
	//	cout << log_idx_ts[i] << " ";
	//}
}

/* prompt command from user input */
void LogMan::promptCmd() {
	char cmd;

	do {
		cout << "% ";
		cin >> cmd;
		handleCmd(cmd);
	} while (cmd != 'q');
}

void LogMan::handleCmd(const char cmd) {
	switch (cmd) {
		/* searching commands */
		case 't': {
			tCmdHandle();
			break;
				  }
		case 'm': {
			mCmdHandle();
			break;
				  }
		case 'c':
				  {
			if (!build_category) {
				buildCategory();
			}
			cCmdHandle();
			break;
				  }
		case 'k': {
			if (!build_category) {
				buildCategory();
			}
			kCmdHandle();
			break;
				  }
		/* excerpt list commands */
		case 'a':
			aCmdHandle();
			break;
		case 'r':
			rCmdHandle();
			break;
		case 'd':
			dCmdHandle();
			break;
		case 'b':
			bCmdHandle();
			break;
		case 'e':
			eCmdHandle();
			break;
		case 's':
			sCmdHandle();
			break;
		case 'l':
			lCmdHandle();
			break;
		case 'g':
			gCmdHandle();
			break;
		case 'p':
			pCmdHandle();
			break;
		case 'q':
			break;
		case '#':
			{
			string junk;
			getline(cin, junk);
			break;
			}
		default:
			cerr << "Unrecognized command '" << cmd << "'!\n";
			break;
	} // switch
	if (cmd == 't' || cmd == 'm' || cmd == 'c' || cmd == 'k') {
		last_search = cmd;
	}
}

void LogMan::tCmdHandle() {
	string t1;
	string t2;
	int64_t num_entries = 0;
	getline(cin, t1, '|');
	t1.erase(0, 1);
	cin >> t2;
	if (isValidTimeStamp(t1) && isValidTimeStamp(t2)) {
		auto lower = std::lower_bound(log_idx_ts.begin(), log_idx_ts.end(),
				transformTimeStamp(t1), Log::CompareByTimeStamp(master_log));
		auto upper = std::upper_bound(log_idx_ts.begin(), log_idx_ts.end(),
				transformTimeStamp(t2), Log::CompareByTimeStamp(master_log));
		most_recent.first = lower - log_idx_ts.begin();
		most_recent.second = upper - log_idx_ts.begin();
		num_entries = most_recent.second - most_recent.first;
	}
	std::cout << "Timestamps search: " << num_entries << " entries found\n";
}

void LogMan::mCmdHandle() {
	string t;
	int64_t num_entries = 0;
	cin >> t;

	if (isValidTimeStamp(t)) {
		auto p = std::equal_range(log_idx_ts.begin(), log_idx_ts.end(),
				transformTimeStamp(t), Log::CompareByTimeStamp(master_log));
		most_recent.first = p.first - log_idx_ts.begin();
		most_recent.second = p.second - log_idx_ts.begin();
		num_entries = most_recent.second - most_recent.first;
	}
	std::cout << "Timestamp search: " << num_entries << " entries found\n";
}

/* build hash table with (cat, vec[idx]) from master log */
void LogMan::buildCategory() {
	for (uint64_t i = 0; i < log_idx_ts.size(); ++i) {
		string key = getLower(master_log[log_idx_ts[i]].cat);
		category[key].push_back(log_idx_ts[i]);
	} // for
	build_category = true;
}

void LogMan::cCmdHandle() {
	string cat;
	cin >> cat;
	cat = getLower(cat);
	last_cat_search = cat;
	std::cout << "Category search: " << category[cat].size() << " entries found\n";
}

/* return true if any of 't', 'm', 'c', 'k' commands occur before */
bool LogMan::hasLastSearch() {
	return last_search != 'n';
}

/* add most recent search results to the end of excerpt list */
void LogMan::rCmdHandle() {
	// append last search result to excerpt list
	if (hasLastSearch()) {
		int64_t num_entries = 0;
		switch(last_search) {
			case 't': case 'm':
				if (most_recent.first != most_recent.second) {
					auto lower_it = log_idx_ts.begin() + most_recent.first;
					auto upper_it = log_idx_ts.begin() + most_recent.second;
					num_entries = most_recent.second - most_recent.first;
					std::copy(lower_it, upper_it, std::back_inserter(excerpt_list));
					break;
				}
				break;
			case 'c':
				if (last_cat_search.length() != 0) {
					num_entries = (int64_t)category[last_cat_search].size();
					std::copy(category[last_cat_search].begin(), category[last_cat_search].end(),
						std::back_inserter(excerpt_list));
				}
				break;
			case 'k':
				if (last_keyword_search.size() != 0) {
					std::copy(last_keyword_search.begin(), last_keyword_search.end(),
							std::back_inserter(excerpt_list));
				}
				num_entries = (int64_t)last_keyword_search.size();
				break;
		} // switch
		cout << num_entries << " log entries appended\n";
	}
}

void LogMan::pCmdHandle() {
	for (size_t i = 0; i < excerpt_list.size(); ++i) {
		cout << i << "|" << excerpt_list[i] << "|" << master_log[excerpt_list[i]];
	}
}

void LogMan::dCmdHandle() {
	unsigned pos;
	cin >> pos;
	if (pos >= excerpt_list.size()) {
		cerr << "Invalid integer for [d] delete command\n";
		return;
	}
	excerpt_list.erase(excerpt_list.begin() + pos);
	cout << "Deleted excerpt list entry " << pos <<"\n";
}

void LogMan::bCmdHandle() {
	unsigned pos;
	cin >> pos;
	if (pos >= excerpt_list.size()) {
		cerr << "Invalid integer for [b] move to beginning command\n";
		return;
	}
	if (pos == 0) {
		cout << "Moved excerpt list entry " << pos <<"\n";
		return;
	}
	uint64_t target = excerpt_list[pos];
	excerpt_list.erase(excerpt_list.begin() + pos);
	excerpt_list.push_front(target);
	cout << "Moved excerpt list entry " << pos <<"\n";
}

void LogMan::eCmdHandle() {
	unsigned pos;
	cin >> pos;
	if (pos >= excerpt_list.size()) {
		cerr << "Invalid integer for [b] move to beginning command\n";
		return;
	}
	if (pos == excerpt_list.size() - 1) {
		cout << "Moved excerpt list entry " << pos <<"\n";
		return;
	}
	uint64_t target = excerpt_list[pos];
	excerpt_list.erase(excerpt_list.begin() + pos);
	excerpt_list.push_back(target);
	cout << "Moved excerpt list entry " << pos <<"\n";
}

void LogMan::lCmdHandle() {
	cout << "excerpt list cleared\n";
	if (excerpt_list.empty()) {
		cout << "(previously empty)\n";
	} else {
		uint64_t last = excerpt_list.size() - 1;
		cout << "previous contents:\n";
		cout << 0 << "|" << excerpt_list[0] << "|" << master_log[excerpt_list[0]];
		cout << "...\n";
		cout << last<< "|" << excerpt_list[last] << "|" << master_log[excerpt_list[last]];
		excerpt_list.clear();
	}
}

void LogMan::sCmdHandle() {
	cout << "excerpt list sorted\n";
	if (excerpt_list.empty()) {
		cout << "(previously empty)\n";
	} else {
		uint64_t last = excerpt_list.size() - 1;
		cout << "previous ordering:\n";
		cout << 0 << "|" << excerpt_list[0] << "|" << master_log[excerpt_list[0]];
		cout << "...\n";
		cout << last<< "|" << excerpt_list[last] << "|" << master_log[excerpt_list[last]];
		sort(excerpt_list.begin(), excerpt_list.end(), Log::SortIndexByTimeStamp(master_log));
		cout << "new ordering:\n";
		cout << 0 << "|" << excerpt_list[0] << "|" << master_log[excerpt_list[0]];
		cout << "...\n";
		cout << last<< "|" << excerpt_list[last] << "|" << master_log[excerpt_list[last]];
	}
}

void LogMan::gCmdHandle() {
	if (last_search == 'n') {
		return;
	} else {
		switch (last_search) {
			case 't': case 'm':
				{
					for (int64_t i = most_recent.first; i != most_recent.second; ++i) {
						cout << log_idx_ts[(uint64_t)i] << "|" << master_log[log_idx_ts[(uint64_t)i]];
					}
					break;
				}
			case 'c':
				{
					for (unsigned i = 0; i < category[last_cat_search].size(); ++i) {
						cout << category[last_cat_search][i] << "|" << master_log[category[last_cat_search][i]];
					}
					break;
				}
			case 'k':
				{
					for (unsigned i = 0; i < last_keyword_search.size(); ++i) {
						cout << last_keyword_search[i] << "|" << master_log[last_keyword_search[i]];
					}
					break;
				}
		} // switch
	} // else
}

void LogMan::aCmdHandle() {
	unsigned pos;
	cin >> pos;
	if (pos >= master_log.size()) {
		cerr << "Invalid integer for [a] append log entry command\n";
		return;
	}
	excerpt_list.push_back(pos);
	cout << "log entry " << pos << " appended\n";
}

void LogMan::kCmdHandle() {
	uint64_t num_entries = 0;
	string input;
	string keyword;
	getline(cin, input);
	std::unordered_set<string> keywords;
	transform(input.begin(), input.end(), input.begin(), tolower);
	/* find and store keywords in set */
	for (auto it = input.begin(); it != input.end(); ++it) {
		auto pos1 = find_if(it, input.end(), isalnum);
		if (pos1 != input.end()) {
			auto pos2 = pos1;
			while (pos2 != input.end()) {
				if (!isalnum(*pos2)) {
					break;
				}
				++pos2;
				++it;
			} // while
			keyword = string(pos1, pos2);
			if (keywords.find(keyword) == keywords.end()) {
				keywords.insert(keyword);
			}
		} // if
	} // for

	/* match keyword with corresponding log entries id */
	for (auto it = keywords.begin(); it != keywords.end(); ++it) {
		if (keyword_map.find(*it) != keyword_map.end()) {
			continue; // already stored in keyword_map
		} // if
		unsigned keysize = (unsigned)(*it).size();
		bool match = false;
		/* find log entries that contain this keyword */
		for (uint64_t i = 0; i < log_idx_ts.size(); ++i) {
			uint64_t id = log_idx_ts[i];
			bool found = false;
			//cout << "Find keyword " << (*it) << " in log entry " << id << " :";
			auto s_cat = search(master_log[id].cat.begin(), master_log[id].cat.end(),
					(*it).begin(), (*it).end(), matchKeyword);
			while (s_cat != master_log[id].cat.end()) {
				//cout << "(cat " << s_cat - master_log[id].cat.begin() << ") ";
				if (isValidMatch(master_log[id].cat, (s_cat - master_log[id].cat.begin()),
							(s_cat - master_log[id].cat.begin() + keysize))) {
					//cout << "found";
					found = true;
					break;
				}
				s_cat = search(s_cat + (long)keysize, master_log[id].cat.end(), (*it).begin(), (*it).end(), matchKeyword);
			}
			//cout << "\n";
			auto s_msg = search(master_log[id].msg.begin(), master_log[id].msg.end(),
					(*it).begin(), (*it).end(), matchKeyword);
			while (s_msg != master_log[id].msg.end()) {
				//cout << "(msg " << s_msg - master_log[id].msg.begin() << ") ";
				if (isValidMatch(master_log[id].msg, s_msg - master_log[id].msg.begin(),
							s_msg - master_log[id].msg.begin() + keysize)) {
					//cout << "found";
					found = true;
					break;
				}
				s_msg = search(s_msg + (long)keysize, master_log[id].msg.end(), (*it).begin(), (*it).end(), matchKeyword);
			}
			//cout << "\n";
			if (found) {
				match = found; // we got a match for this keyword!
				keyword_map[*it].push_back(id);
			}
		} // for
		if (!match) { // one of the keyword has no match in master log, no need to go further!
			cout << "Keyword search: 0 entries found\n";
			return;
		}
	} // for

	// do set_intersection
	std::vector<uint64_t> prev;
	std::vector<uint64_t> ret;
	auto key = keywords.begin();
	prev.assign(keyword_map[*key].begin(), keyword_map[*key].end());
	for (auto it = keywords.begin(); it != keywords.end(); ++it) {
		ret.clear();
		set_intersection(prev.begin(), prev.end(), keyword_map[*it].begin(),
				keyword_map[*it].end(), back_inserter(ret), Log::SortIndexByTimeStamp(master_log));
		prev = ret;
	}
	num_entries = ret.size();
	cout << "Keyword search: " << num_entries << " entries found\n";
	last_keyword_search.assign(ret.begin(), ret.end());
	return;
}

/************************************************************************
*************** Definitions of utility functions ************************
************************************************************************/

/* converts internal timestamp in uint64_t to original string format */
string getTimeStamp(int64_t t) {
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
int64_t transformTimeStamp(string t) {
	replace(t.begin(), t.end(), ':', '1');
	int64_t ts = (int64_t)stoull(t, NULL, 10);
	return ts;
}

/* check if timestamp has the right number of digits */
bool isValidTimeStamp(string t) {
	if (t.length() != ts_num_digits) {
		cerr << "Invalid timestamp '" << t << "'\n";
		return false;
	}
	return true;
}

/* return string content in lower case */
string getLower(const string &s) {
	string ret = s;
	for (auto &c : ret) {
		c = (char)tolower(c);
	}
	return ret;
}

/* compare char in a msg with char keyword that is lowercase */
bool matchKeyword(char c_msg, char c_keyword) {
	return (tolower(c_msg) == c_keyword);
}

/* determine if a matched keyword in string is a valid keyword */
bool isValidMatch(const string &s, long pos1, long pos2) {
	bool is_valid_beg = false;
	bool is_valid_end = false;
	if (pos1 == 0) {
		is_valid_beg = true;
	} else {
		is_valid_beg = !isalnum(s[(unsigned)--pos1]);
	}
	if ((unsigned)pos2 == s.length()) {
		is_valid_end = true;
	} else {
		is_valid_end = !isalnum(s[(unsigned)pos2]);
	}
	return is_valid_beg && is_valid_end;
}
