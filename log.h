  // Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <iostream>
#include <string>
#include <deque>
#include <utility> // pair
#include <iterator>

using std::string;

/* utility functions */
string getTimeStamp(uint64_t t);
uint64_t transformTimeStamp(std::string t);
bool isValidTimeStamp(std::string t);
string getLower(const string &s);

class Log {
	public:
		Log(const string _cat, const string _msg, const uint64_t _ts, const uint64_t _id) :
			cat(_cat), msg(_msg), ts(_ts), id(_id) {}
		string cat;
		string msg;
		uint64_t ts;
		uint64_t id;

		struct CompareByTimeStamp {
			bool operator()(const uint64_t &t, const Log &l) const { return t < l.ts; }
			bool operator()(const Log &l, const uint64_t &t) const { return l.ts < t; }
		};
		struct CompareLogEntries {
			bool operator()(const Log &left, const Log &right) const {
				if (left.ts < right.ts) {
					return true;
				} else if (left.ts > right.ts) {
					return false;
				} else {
					if (getLower(left.cat) < getLower(right.cat)) {
						return true;
					} else if (getLower(left.cat) > getLower(right.cat)) {
						return false;
					} else {
						return left.id < right.id;
					}
				}
			}
		};

	private:

}; // class Log

class LogMan {
	public:
		void getOptions(int argc, char **argv);
		void parseLogEntries(const string &filename);
		void sortMasterLog();
		void promptCmd();

	private:
		std::deque<Log> master_log;
		std::pair<int64_t, int64_t> most_recent = std::make_pair(-1, -1);

		void handleCmd(const char cmd/*, const string &input*/);
		void tCmdHandle(const string &t1, const string &t2) {
			int64_t num_entries = 0;
			if (isValidTimeStamp(t1) && isValidTimeStamp(t2)) {
				auto lower = std::lower_bound(master_log.begin(), master_log.end(),
						transformTimeStamp(t1), Log::CompareByTimeStamp());
				auto upper = std::upper_bound(master_log.begin(), master_log.end(),
						transformTimeStamp(t2), Log::CompareByTimeStamp());
				most_recent.first = lower - master_log.begin();
				most_recent.second = upper - master_log.begin();
				num_entries = most_recent.second - most_recent.first;
			}
			std::cout << "Timestamp search: " << num_entries << " entries found\n";
		}

		//std::pair<ForwardIt, ForwardIt> mCmdHandle(string t);
		//void dbgPrintRangeLog(std::pair<ForwardIt, ForwardIt> p);
		void cCmdHandle(string cat);
		void kCmdHandle(string keyword);

}; // class LogMan

