  // Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include <iostream>
#include <string>
#include <strings.h> // strcasecmp
#include <vector> // val in category
#include <deque> // master_log
#include <unordered_map> // category
#include <unordered_set> // keywords
#include <utility> // pair
#include <iterator> // back_inserter

using std::string;

/* utility functions */
string getTimeStamp(int64_t t);
int64_t transformTimeStamp(std::string t);
bool isValidTimeStamp(std::string t);
string getLower(const string &s);
bool matchKeyword(char c_msg, char c_key);
bool isValidMatch(const string &s, long pos1, long pos2);

class Log {
	public:
		Log(const string _cat, const string _msg, const int64_t _ts) :
			cat(_cat), msg(_msg), ts(_ts) {}
		string cat;
		string msg;
		int64_t ts;

		/* used to get a container of indexes to log entries by ascending time stamp */
		class SortIndexByTimeStamp {
			const std::deque<Log> &logs;
			public:
				SortIndexByTimeStamp(const std::deque<Log> &master) : logs(master) {}

				bool operator()(uint64_t i, uint64_t j) const {
					if (logs[i].ts < logs[j].ts) {
						return true;
					} else if (logs[i].ts > logs[j].ts) {
						return false;
					} else {
						if (strcasecmp(&logs[i].cat[0], &logs[j].cat[0]) < 0) {
							return true;
						} else if (strcasecmp(&logs[i].cat[0], &logs[j].cat[0]) > 0) {
							return false;
						} else {
							return i < j;
						}
					}
				} // operator
		};

		/* used to compare log entries with time stamps directly */
		class CompareByTimeStamp {
			const std::deque<Log> &master;
			public:
				CompareByTimeStamp(const std::deque<Log> &_master) : master(_master) {}
				bool operator()(uint64_t i, int64_t t) const { return master[i].ts < t; }
				bool operator()(int64_t t, uint64_t i) const { return t < master[i].ts; }
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
		std::deque<Log> master_log; /* contains log entries in original order */
		std::vector<uint64_t> log_idx_ts; /* contains log id sorted by timestamp */
		std::unordered_map<string, std::vector<uint64_t>> keyword_map;
		std::unordered_map<string, std::vector<uint64_t>> category;
		std::deque<uint64_t> excerpt_list; /* contains log entries by log id */
		std::vector<uint64_t> last_keyword_search;
		std::pair<int64_t, int64_t> most_recent = std::make_pair(-1, -1);
		string last_cat_search = "";
		bool build_category = false;
		char last_search = 'n';

		bool hasLastSearch();
		void handleCmd(const char cmd);
		bool tCmdHandle();
		void mCmdHandle();
		void buildCategory();
		void cCmdHandle();
		void rCmdHandle();
		void pCmdHandle();
		void dCmdHandle();
		void bCmdHandle();
		void eCmdHandle();
		void lCmdHandle();
		void sCmdHandle();
		void gCmdHandle();
		void aCmdHandle();
		void kCmdHandle();
}; // class LogMan

