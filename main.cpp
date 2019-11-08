  // Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
#include "log.h"


int main(int argc, char **argv) {

	std::ios_base::sync_with_stdio(false);

	LogMan log_manager = LogMan();
	log_manager.getOptions(argc, argv);
	log_manager.parseLogEntries(argv[1]);
	log_manager.sortMasterLog();
	log_manager.promptCmd();

	return 0;
}
