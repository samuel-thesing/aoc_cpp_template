#include "utils.h"

/* ====================================================================================================
 * Whitespace Correction
 */

/**
 * fills the left side of a string to the given length using the filler character
 * @param s string to be filled
 * @param len length to be filled to
 * @param filler char used to fill the string - default space
 * @return filled string
 */
std::string pad_left(const std::string& s, int len, char filler) {
	return std::string(std::max(len - s.length(), 0ull), filler) + s;
}

/**
 * fills the right side of a string to the given length using the filler character
 * @param s string to be filled
 * @param len length to be filled to
 * @param filler char used to fill the string - default space
 * @return filled string
 */
std::string pad_right(const std::string& s, int len, char filler) {
	return s + std::string(std::max(len - s.length(), 0ull), filler);
}

std::string pad_center(const std::string& s, int len, char filler) {
	auto len_half = s.length() / 2;
	return pad_right(pad_left(s, len_half, filler), len - len_half, filler);
}

std::string repeat(char c, int n) {
	return std::string(n, c);
}

/**
 * trims whitespace (recognized using
 * std::isspace()) from both ends of the string
 * @param s string to be trimmed
 * @return trimmed string
 */
std::string trim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c) {
		return !std::isspace(c);
	}));

	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c) {
		return !std::isspace(c);
	}).base(), s.end());

	return s;
}

std::string replace_all(const std::string& str, const std::string& pattern, const std::string& replace) {
	size_t n = 0;
	std::string str_cpy = str;

	while ((n = str_cpy.find(pattern, n)) != std::string::npos){
		str_cpy.replace(n, pattern.size(), replace);
		n += replace.size();
	}

	return str_cpy;
}

std::string replace_regex(const std::string& str, const std::regex& pattern, const char* replace) {
	return std::regex_replace(str, pattern, replace, std::regex_constants::match_flag_type::format_first_only);
}

std::string replace_regex_all(const std::string& str, const std::regex& pattern, const char* replace) {
	return std::regex_replace(str, pattern, replace);
}

size_t find_nth(const std::string& str, const std::string& pattern, size_t n) {
	int count = 0;
	size_t last_idx = str.find(pattern, n);
	while (last_idx != std::string::npos) {
		if (count == n) {
			return last_idx;
		}
		last_idx = str.find(pattern, last_idx+1);
		++count;
	}
	return std::string::npos;
}

size_t find_nth(const std::string& str, const std::regex& pattern, size_t n) {
	std::sregex_iterator iter(str.begin(), str.end(), pattern);
	std::sregex_iterator end;
	for (int i = 0; i < n && iter != end; ++iter, ++i) {}
	if (iter == end) {
		return std::string::npos;
	}
	return iter->position(0);
}

std::optional<std::string> replace_nth(const std::string& str, const std::string& pattern, std::string replace, int n) {
	auto idx = find_nth(str, pattern, n);
	if (idx == std::string::npos) {
		return {};
	}

	return str.substr(0, idx) + replace + str.substr(idx + pattern.size());
}

/* ====================================================================================================
 * Reading Data
 */
/**
 * Reads file
 * @param filename
 * @return content
 * @throws 0xDEAD If file could not be found or opened
 */
std::string read_file(const std::string& filename) {
	auto dir_name = std::filesystem::current_path().filename().string();
	dir_name = dir_name.substr(dir_name.size() - 2);
	auto cwd = std::filesystem::current_path() / "../../../" / "src" / dir_name;
	cwd = std::filesystem::canonical(cwd);
	auto file = std::ifstream(cwd / filename, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
		Logger::critical("Failed to open file '{}'. CWD: {}", filename, cwd.string());
    }

	std::stringstream contents;
	contents << file.rdbuf();

	return replace_all(contents.str(), "\r\n", "\n");
}

std::vector<std::string> split_lines(const std::string& s) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string line;
	while (std::getline(ss, line)) {
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		result.emplace_back(line);
	}
	return result;
}

/* ====================================================================================================
 * Splitting
 */


/**
 *	Splits a given string at the given delimiter and trims the parts
 *	@param s string to be split
 *	@param delim delimiter (can be longer than 1 char)
 */
std::vector<std::string> split(const std::string& s, const std::string& delim) {
	if (delim.empty()) {
		Logger::critical("`split` received an empty delimiter");
		return {};
	}
	if (trim(s).empty()) {
		return {};
	}

	size_t lastDelim = 0;
	auto curDelim = s.find(delim, lastDelim);
	std::vector<std::string> parts{};

	while (curDelim != std::string::npos) {
		std::string part = s.substr(lastDelim, curDelim - lastDelim);
		parts.emplace_back(trim(part));
		lastDelim = curDelim + delim.size();
		curDelim = s.find(delim, lastDelim);
	}

	auto lastPart = s.substr(lastDelim);
	parts.emplace_back(trim(lastPart));

	return parts;
}

std::vector<int> split_int(const std::string& s, const std::string& delim) {
	return split<int>(s, delim);
}

/**
 *	Splits a given string at the first occurrence of the given delimiter and returns both parts as pair
 *	@param s string to be split
 *	@param delim delimiter (can be longer than 1 char)
 *	@param fn converts parts after splitting using this function
 */
std::pair<std::string, std::string> split_once(const std::string& s, const std::string& delim) {
	auto idx = s.find(delim);
	if (idx == std::string::npos) {
		Logger::critical("Failed split_once");
	}

	return {s.substr(0, idx), s.substr(idx + delim.size())};
}

std::vector<std::string> split_regex(const std::string& s, std::regex& pattern) {
	std::sregex_token_iterator iter(s.begin(), s.end(), pattern, -1);
	std::sregex_token_iterator end;
	return {iter, end};
}

std::pair<std::string, std::string> split_once_regex(const std::string& s, const std::regex& pattern) {
	std::smatch match;
	if (!std::regex_search(s, match, pattern)) {
		Logger::critical("Failed to find regex in '{}'", s);
	}

	auto match_start = match.position(0);
	auto match_end = match_start + match.length(0);

	return {
		s.substr(0, match_start),
		s.substr(match_end)
	};
}

std::vector<std::string> find_all_regex(const std::string& s, std::regex& pattern) {
	std::sregex_iterator iter(s.begin(), s.end(), pattern);
	std::sregex_iterator end;
	std::vector<std::string> result{};
	while (iter != end) {
		std::smatch match = *iter;
		result.push_back(match[1]);
		++iter;
	}
	return result;
}

bool isDigit(char c) {
	return '0' <= c && c <= '9';
}

bool isLowercase(char c) {
	return 'a' <= c && c <= 'z';
}

bool isUppercase(char c) {
	return 'A' <= c && c <= 'Z';
}

bool isHex(char c) {
	return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
}

std::string format_time(std::chrono::duration<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::period> duration) {
	std::string result = "";
	const int64_t lengths[] = {
		// nano
		1000, //micro
		1000, //milli
		1000, //sec
		60, // min
		60, // hours
		24, // days
		365 // years
	};

	const char* names[] = {" ns", " μs ", " ms ", " s ", " min ", " h ", " d "};

	auto rest = duration.count();
	int i = 0;
	while (rest != 0 && i < 7) {
		result.insert(0, std::to_string(rest % lengths[i]) + names[i]);
		rest /= lengths[i];
		i++;
	}
	if (rest != 0) {
		result = std::to_string(rest) + " a " + result;
	}
	return result;
}

std::vector<size_t> find_all_idx(const std::string& s, const std::string& pattern) {
	std::vector<size_t> idxs{};
	size_t last_idx = s.find(pattern);
	while (last_idx != std::string::npos) {
		idxs.push_back(last_idx);
		last_idx = s.find(pattern, last_idx+1);
	}
	return idxs;
}

Vec2i dir_vec(Dir dir) {
	switch (dir) {
    	case Dir::LEFT: return Vec2i{-1, 0};
    	case Dir::RIGHT: return Vec2i{1, 0};
    	case Dir::UP: return Vec2i{0, -1};
    	case Dir::DOWN: return Vec2i{0, 1};
	}
	return Vec2i{0, 0};
}

std::vector<Vec2i> all_dirs() {
	return {
		Vec2i{1,0},
		Vec2i{0,1},
		Vec2i{-1,0},
		Vec2i{0,-1}
	};
}

std::vector<Vec2i> all_dirs_diag() {
	return {
		Vec2i{1,0},
		Vec2i{1,1},
		Vec2i{0,1},
		Vec2i{-1,1},
		Vec2i{-1,0},
		Vec2i{-1,-1},
		Vec2i{0,-1},
		Vec2i{1,-1}
	};
}

std::string str(char c) {
	return {c};
}

std::string str(int i) {
	return std::to_string(i);
}

std::string str(double i) {
	return std::to_string(i);
}

std::string str(long i) {
	return std::to_string(i);
}

std::string str(uint64_t i) {
	return std::to_string(i);
}

std::vector<size_t> find_all_idx(const std::string& s, char pattern) {
	return find_all_idx(s, str(pattern));
}

size_t find_nth(const std::string& s, char pattern, size_t n) {
	return find_nth(s, str(pattern), n);
}

std::string replace_all(const std::string& s, char pattern, const std::string& replace) {
	return replace_all(s, str(pattern), replace);
}

std::vector<int> diffs(const std::vector<int>& vec) {
	std::vector<int> res{};
	for (int i = 0; i < vec.size()-1; ++i) {
		res.push_back(vec[i+1] - vec[i]);
	}
	return res;
}

std::vector<int> diffs(const std::vector<int>& vec1, const std::vector<int>& vec2) {
	std::vector<int> res{};
	for (int i = 0; i < min(vec1.size(), vec2.size()); ++i) {
		res.push_back(vec2[i] - vec1[i]);
	}
	return res;
}

std::vector<std::string> rotate90c(const std::vector<std::string>& matrix) {
	int n = matrix.size();
	auto res = std::vector<std::string>{};

	for (int i = 0; i < n; ++i) {
		res.emplace_back("");
		for (int j = 0; j < n; ++j) {
			res[i] += matrix[n-j-1][i];
		}
	}

	return res;
}

bool inbounds(int x, int y, int w, int h) {
	return x >= 0 && y >= 0 && x < w && y < h;
}

bool inbounds(int x, int y, int w, int h, int bw, int bh) {
	return x >= 0 && y >= 0 && x < w-bw+1 && y < h-bh+1;
}

Vec2i arrow_dir(char c) {
	switch (c) {
		case '^': return Vec2i{0, -1};
		case 'v': return Vec2i{0, 1};
		case '>': return Vec2i{1, 0};
		case '<': return Vec2i{-1, 0};
		default: Logger::critical("Unrecognized arrow direction '{}'", c);
	}
}

int num_len(long long n) {
	return static_cast<int>(std::log10(n)) + 1;
}
