#include <utils.h>

int solve(const std::string &input) {
	int result = 0;

    std::regex pattern("(\\d+)");

	auto blocks = split(input, "\n\n");
	for (const auto& block : blocks) {
		auto lines = split(block, "\n");
		auto lines2 = split_int(block, "\n");
        const auto [a] = extract_data<int>(block, std::regex("(\\d+)"));

		for (const auto& line : lines) {

		}
	}

	auto lines = split(input, "\n");
	auto lines2 = split_int(input, "\n");

    for (const auto& line : lines) {
    	const auto [a] = extract_data<int>(line, std::regex("(\\d+)"));
    }

	return result;
}

int main(int argc, char** argv) {
    auto runner = Runner<int>(solve, REPLACE_YEAR, REPLACE_DAY_NUM);
    runner.add_test_file("t1.txt", 0);

    runner.add_input_file("i1.txt");

    runner.run();
}