#include "base.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct AssetInfo {
	AssetInfo(const string &source, const string &target = "", const string &filter = "")
		: source_path(source), target_path(target), filter(filter) {
		if(target_path.empty())
			target_path = source_path;
	}

	string source_path, target_path;
	string filter;
};

const AssetInfo assets[] = {
							{"human1", "human", "Human|Armature"},
							{"rock"},
							{"level1"},
							{"helpers/arrow"},
							{"helpers/axis_arrow"}};

double lastModificationTime(const string &file_name) {
	struct stat attribs;
	if(stat(file_name.c_str(), &attribs) != 0)
		THROW("stat failed for file %s: %s\n", file_name.c_str(), strerror(errno));

	return double(attribs.st_mtim.tv_sec) + double(attribs.st_mtim.tv_nsec) * 0.000000001;
}

int safe_main(int argc, char **argv) {
	double time = getTime();
	bool errors = false;

	for(const auto &asset : assets) {
		string source = "data/assets/" + asset.source_path + ".blend";
		string target = "data/models/" + asset.target_path + ".model";

		if(access(target) && lastModificationTime(source) < lastModificationTime(target)) {
			printf("Unmodified source: %s\n", target.c_str());
			continue;
		}

		string flags =
			asset.filter.empty() ? "" : "--blender-objects-filter \"" + asset.filter + '"';

		string command = "libfwk/tools/model_convert " + source + " " + target + " " + flags;
		pair<string, bool> result = execCommand(command);
		if(!result.second) {
			errors = true;
			printf("Errors during conversion!\n");
		}
		printf("%s", result.first.c_str());
	}

	printf("Conversion time: %.2f sec\n", getTime() - time);

	return errors ? 1 : 0;
}

int main(int argc, char **argv) {
	try {
		return safe_main(argc, argv);
	} catch(const Exception &ex) {
		printf("%s\n\nBacktrace:\n%s\n", ex.what(), cppFilterBacktrace(ex.backtrace()).c_str());
		return 1;
	}
}
