#ifndef UTILS_H
#define UTILS_H

#include <regex>
#include <queue>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <filesystem>
#include <set>
#include <map>
#include <iostream>
#include <functional>
#include <sstream>
#include <numeric>

#include <Logger.h>

#include "Matrix.h"


/* ====================================================================================================
 * Typedefs
 */
template <typename Value>
using Table = std::vector<std::vector<Value>>;

template <typename Key, typename Value>
using NamedTable = std::unordered_map<Key, std::unordered_map<Key, Value>>;

/* ====================================================================================================
 * Structs
 */
struct Point {
	int x;
	int y;
};

template<>
struct std::hash<Point> {
	size_t operator()(const Point& point) const {
		return static_cast<size_t>(point.x) ^ (static_cast<size_t>(point.y) << 32);
	}
};

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
std::string pad_left(const std::string& s, int len, char filler = ' ');

/**
 * fills the right side of a string to the given length using the filler character
 * @param s string to be filled
 * @param len length to be filled to
 * @param filler char used to fill the string - default space
 * @return filled string
 */
std::string pad_right(const std::string& s, int len, char filler = ' ');
std::string pad_center(const std::string& s, int len, char filler = ' ');

std::string repeat(char c, int n);

/**
 * trims whitespace (recognized using
 * std::isspace()) from both ends of the string
 * @param s string to be trimmed
 * @return trimmed string
 */
std::string trim(std::string s);

std::string replace_all(const std::string& str, const std::string& pattern, const std::string& replace);
std::string replace_regex(const std::string& str, const std::regex& pattern, const char* replace);
std::string replace_regex_all(const std::string& str, const std::regex& pattern, const char* replace);

size_t find_nth(const std::string& str, const std::string& pattern, size_t n);
size_t find_nth(const std::string& str, const std::regex& pattern, size_t n);

std::optional<std::string> replace_nth(const std::string& str, const std::string& pattern, std::string replace, int n);

/* ====================================================================================================
 * Reading Data
 */
/**
 * Reads file
 * @param filename
 * @return content
 * @throws 0xDEAD If file could not be found or opened
 */
std::string read_file(const std::string& filename);

std::vector<std::string> split_lines(const std::string& s);

/* ====================================================================================================
 * Printing Data
 */
/**
 * Prints a 2d unordered_map
 * @param mat data to print
 * @param keyFormatter transformation function for key-values
 * @param valueFormatter transformation function for data-values
 */
template<typename Key, typename Value>
void printAdjacencyMatrix(const NamedTable<Key, Value>& mat,
	std::function<std::string(Key)> keyFormatter, std::function<std::string(Value)> valueFormatter) {

	std::set<Key> keys;
	int max_key_len = 0;
	for (const auto& [key, row] : mat) {
		keys.emplace(key);
		max_key_len = std::max(max_key_len, (int)(keyFormatter(key).size()));
		for (const auto& [key2, _] : row) {
			keys.emplace(key2);
			max_key_len = std::max(max_key_len, (int)(keyFormatter(key2).size()));
		}
	}

	int keyCounter = 0;
	std::unordered_map<Key, int> keyIdx;
	for (const auto& key : keys) {
		keyIdx.emplace(key, keyCounter);
		keyCounter++;
	}

	std::vector<std::vector<std::string>> values;
	for (int i = 0; i < keyCounter; i++) {
		values.emplace_back(keyCounter, pad_left("-", max_key_len) + "  ");
	}

	for (const auto& [key, row] : mat) {
		const auto x = keyIdx.find(key);
		for (const auto& [key2, value] : row) {
			const auto y = keyIdx.find(key2);
			values[x->second][y->second] = pad_left(valueFormatter(value), max_key_len) + "  ";
		}
	}

	std::cout << pad_right("", max_key_len);
	for (const auto& key : keys) {
		std::cout << "  " << pad_right(std::string(key), max_key_len);
	}
	std::cout << std::endl;

	for (const auto& [key, row] : mat) {
		std::cout << pad_right(std::string(key), max_key_len) << "  ";
		const auto idx = keyIdx.find(key);
		for (const auto& value : values[idx->second]) {
			std::cout << value;
		}
		std::cout << std::endl;
	}
}

/* ====================================================================================================
 * Splitting
 */

template<typename T>
T string_to_generic(std::string s) {
	Logger::critical("No implementation for generating this generic");
	return T();
}

template<>
inline std::string string_to_generic<std::string>(std::string s) {
	return s;
}

template<>
inline int string_to_generic<int>(std::string s) {
	return std::stoi(s);
}

template<>
inline long string_to_generic<long>(std::string s) {
	return std::stol(s);
}

template<>
inline double string_to_generic<double>(std::string s) {
	return std::stod(s);
}

template<>
inline char string_to_generic<char>(std::string s) {
	return s[0];
}
template<>
inline long long string_to_generic<long long>(std::string s) {
	return std::stoll(s);
}


/**
 *	Splits a given string at the given delimiter and trims the parts
 *	@param s string to be split
 *	@param delim delimiter (can be longer than 1 char)
 */
std::vector<std::string> split(const std::string& s, const std::string& delim);

/**
 *	Splits a given string at the given delimiter and trims the parts before converting them using the given function.
 *	@param s string to be split
 *	@param delim delimiter (can be longer than 1 char)
 *	@param fn converts parts after splitting using this function
 */
template<typename T>
std::vector<T> split(const std::string& s, const std::string& delim, std::function<T(std::string)> fn) {
	auto splitted = split(s, delim);
	auto result = std::vector<T>();
	result.reserve(splitted.size());
	for (auto str : splitted) {
		result.emplace_back(fn(str));
	}
	return result;
}

template<typename T>
std::vector<T> split(const std::string& s, const std::string& delim) {
	auto splitted = split(s, delim);
	auto result = std::vector<T>();
	result.reserve(splitted.size());
	for (auto str : splitted) {
		result.emplace_back(string_to_generic<T>(str));
	}
	return result;
}

std::vector<int> split_int(const std::string& s, const std::string& delim);

/**
 *	Splits a given string at the first occurrence of the given delimiter and returns both parts as pair
 *	@param s string to be split
 *	@param delim delimiter (can be longer than 1 char)
 *	@param fn converts parts after splitting using this function
 */
std::pair<std::string, std::string> split_once(const std::string& s, const std::string& delim);

template<typename T, typename U>
std::pair<T, U> split_once(const std::string& s, const std::string& delim) {
	auto idx = s.find(delim);
	if (idx == std::string::npos) {
		Logger::critical("Failed split_once");
	}

	return {
		string_to_generic<T>(s.substr(0, idx)),
		string_to_generic<U>(s.substr(idx + delim.size()))
	};
}

template<typename T>
std::pair<T, T> split_once(const std::string& s, const std::string& delim, std::function<T(std::string)> fn) {
	auto idx = s.find(delim);
	if (idx == std::string::npos) {
		Logger::critical("Failed split_once");
	}

	return {
		fn(s.substr(0, idx)),
		fn(s.substr(idx + delim.size()))
	};
}

template<typename T, typename... Args> requires (std::is_same_v<T, Args...> && sizeof...(Args) > 1)
T max(T arg, Args... args) {
	return max(arg, max(args...));
}

template<typename T, typename U> requires (std::is_same_v<T, U>)
T max(T t, U u) {
	return t > u ? t : u;
}

template<typename T, typename... Args> requires (std::is_same_v<T, Args...> && sizeof...(Args) > 1)
T min(T arg, Args... args) {
	return min(arg, min(args...));
}

template<typename T, typename U> requires (std::is_same_v<T, U>)
T min(T t, U u) {
	return t < u ? t : u;
}

template<typename... Args, std::size_t... Indices>
std::tuple<Args...> make_tuple_from_match(const std::smatch& match, std::index_sequence<Indices...>) {
	return std::make_tuple<Args...>(string_to_generic<Args>(match[Indices + 1].str())...);
}

/**
 * Extracts data from a given string using a regex and converts the matches to the correct type using the generics provided
 * @tparam Args Types of the matches. A template specialization of string_to_generic must be provided
 * @param pattern regex pattern to match
 * @param s	string to be matched
 * @return tuple of the converted matches
 */
template<typename... Args>
std::tuple<Args...> extract_data(std::string s, const std::regex& pattern) {
	std::smatch match;
	if (!std::regex_match(s, match, pattern)) {
		Logger::critical("Failed to match regex for '{}'", s);
	}

	return make_tuple_from_match<Args...>(match, std::index_sequence_for<Args...>{});
}

template<typename... Args>
std::vector<std::tuple<Args...>> extract_data_all(std::string s, const std::regex& pattern) {
	std::sregex_iterator iter(s.begin(), s.end(), pattern);
	std::sregex_iterator end;
	std::vector<std::tuple<Args...>> result{};
	while (iter != end) {
		std::smatch match = *iter;
		result.push_back(make_tuple_from_match<Args...>(match, std::index_sequence_for<Args...>{}));
		++iter;
	}

	return result;
}

template<typename... Args>
std::optional<std::tuple<Args...>> extract_data_opt(std::string s, const std::regex& pattern) {
	std::smatch match;
	if (!std::regex_match(s, match, pattern)) {
		return std::nullopt;
	}

	return make_tuple_from_match<Args...>(match, std::index_sequence_for<Args...>{});
}

std::vector<std::string> split_regex(const std::string& s, std::regex& pattern);

template<typename T>
std::vector<std::string> split_regex(const std::string& s, std::regex& pattern) {
	std::sregex_token_iterator iter(s.begin(), s.end(), pattern, -1);
	std::sregex_token_iterator end;
	auto result = std::vector<T>();
	for (; iter != end; ++iter) {
		result.emplace_back(string_to_generic<T>(*iter));
	}
	return result;
}

template<typename T>
std::vector<std::string> split_regex(const std::string& s, std::regex& pattern, std::function<T(std::string)> fn) {
	std::sregex_token_iterator iter(s.begin(), s.end(), pattern, -1);
	std::sregex_token_iterator end;
	auto result = std::vector<T>();
	for (; iter != end; ++iter) {
		result.emplace_back(fn(*iter));
	}
	return result;
}

std::pair<std::string, std::string> split_once_regex(const std::string& s, const std::regex& pattern);

template<typename T, typename U>
std::pair<T, U> split_once_regex(const std::string& s, const std::regex& pattern) {
	std::smatch match;
	if (!std::regex_search(s, match, pattern)) {
		Logger::critical("Failed to find regex in '{}'", s);
	}

	auto match_start = match.position(0);
	auto match_end = match_start + match.length(0);

	return {
		string_to_generic<T>(s.substr(0, match_start)),
		string_to_generic<U>(s.substr(match_end))
	};
}

template<typename T>
std::pair<T, T> split_once_regex(const std::string& s, const std::regex& pattern, std::function<T(std::string)> fn) {
	std::smatch match;
	if (!std::regex_search(s, match, pattern)) {
		Logger::critical("Failed to find regex in '{}'", s);
	}

	auto match_start = match.position(0);
	auto match_end = match_start + match.length(0);

	return {
		fn(s.substr(0, match_start)),
		fn(s.substr(match_end))
	};
}

std::vector<std::string> find_all_regex(const std::string& s, std::regex& pattern);

bool isDigit(char c);
bool isLowercase(char c);
bool isUppercase(char c);
bool isHex(char c);

std::string format_time(std::chrono::duration<std::chrono::nanoseconds::rep, std::chrono::nanoseconds::period> duration);

template<typename Result, typename... Args>
struct Test {
	std::string input;
	Result expected;
	bool file;
	std::tuple<Args...> args;
};

template<typename... Args>
struct Input {
	std::string input;
	bool file;
	std::tuple<Args...> args;
};

template <typename Result, typename... Args>
class Runner {
private:
	typedef std::function<Result(std::string, Args...)> SolverFn;
	typedef std::function<std::string(Result)> ResultTransformFn;

	SolverFn solve_fn;
	ResultTransformFn result_transform_fn = nullptr;

	std::vector<Test<Result, Args...>> tests;
	std::vector<Input<Args...>> inputs;
	std::vector<Result> results;

	unsigned tests_failed;
	unsigned tests_succeeded;

public:
	Runner(SolverFn solve_fn, const int year, const int day) : solve_fn(solve_fn), tests_failed(0), tests_succeeded(0) {
		Logger::init();
		Logger::info("==================================================");
		Logger::info("=========== Advent of Code {} Day {} ===========", year, pad_left(std::to_string(day), 2, '0'));
		Logger::info("==================================================");
	}

	void set_result_transformation(std::function<std::string(Result)> result_transform_fn) {
		this->result_transform_fn = result_transform_fn;
	}

	void add_test_string(const std::string& input, Result expected, Args... args) {
		tests.push_back(Test<Result, Args...>(input, expected, false, {args...}));
	}

	void add_test_file(const std::string& filename, Result expected, Args... args) {
		tests.push_back(Test<Result, Args...>(filename, expected, true, {args...}));
	}

	void add_input_string(const std::string& input, Args... args) {
		inputs.push_back(Input<Args...>(input, false, {args...}));
	}

	void add_input_file(const std::string& filename, Args... args) {
		inputs.push_back(Input<Args...>(filename, true, {args...}));
	}

	bool run_test(const Test<Result, Args...>& test) {
		auto input = test.input;
		if (test.file) {
			input = read_file(input);
		}
		Result result = std::apply(
			[=](auto&&... args) -> Result {
				return solve_fn(input, args...);
			},
			test.args
		);

		if (result == test.expected) {
			tests_succeeded++;
			return true;
		}

		Logger::error("Failed Test '{}': Expected {} but got {}", test.input, test.expected, result);
		tests_failed++;
		return false;
	}

	bool run_tests() {
		if (tests.empty()) return true;

		Logger::info("==================================================");
		Logger::info("Running {} Test(s)", tests.size());
		Logger::info("==================================================");

		tests_failed = 0;
		tests_succeeded = 0;

		for (auto& test : tests) {
			run_test(test);
		}

		Logger::info("==================================================");
		Logger::info("Test(s) Finished");
		Logger::info("--------------------------------------------------");
		if (tests_succeeded != 0) {
			Logger::info("Succeded: {}", tests_succeeded);
		}

		if (tests_failed != 0) {
			Logger::error("Failed: {}", tests_failed);
		}
		Logger::info("==================================================");

		return tests_failed == 0;
	}

	Result run_input(const Input<Args...>& input) {
		auto input_str = input.input;
		if (input.file) {
			input_str = read_file(input_str);
		}
		auto start_time = std::chrono::high_resolution_clock::now();
		Result result = std::apply(
			[=](auto&&... args) -> Result {
				return solve_fn(input_str, args...);
			},
			input.args
		);
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration =
			std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

		if (result_transform_fn == nullptr) {
			Logger::info("Input Finished '{}': {} ({})", input.input, result, format_time(duration));
		} else {
			Logger::info("Input Finished '{}': {} ({})", input.input, result_transform_fn(result), format_time(duration));
		}

		return result;
	}

	std::vector<Result> run_inputs() {
		results.clear();
		for (auto& input : inputs) {
			results.emplace_back(run_input(input));
		}
		return results;
	}

	std::vector<Result> run() {
		if (!run_tests()) return {};
		run_inputs();

		Logger::info("");
		return results;
	}
};

std::vector<size_t> find_all_idx(const std::string& s, const std::string& pattern);

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& list) {
	os << "{ ";
	for (const T& n : list) {
		os << n << ", ";
	}
	os << "}";
	return os;
}

template<typename T>
T sum(std::vector<T> list) {
	auto sum = static_cast<T>(0);
	for (auto n : list) {
		sum += n;
	}
	return sum;
}

template <typename T>
std::vector<std::vector<T>> create_mat(unsigned size, T fill = static_cast<T>(0)) {
	return std::vector<std::vector<T>>(size, std::vector<T>(size, fill));
}

template <typename T>
T max(std::vector<T> list) {
	T max_val = list[0];
	for (const auto& n : list) {
		max_val = max(max_val, n);
	}
	return max_val;
}

template <typename T>
T min(std::vector<T> list) {
	T min_val = list[0];
	for (const auto& n : list) {
		min_val = min(min_val, n);
	}
	return min_val;
}

enum class Dir {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

Vec2i dir_vec(Dir dir);

std::vector<Vec2i> all_dirs();

std::vector<Vec2i> all_dirs_diag();

std::string str(char c);
std::string str(int i);
std::string str(double i);
std::string str(long i);
std::string str(uint64_t i);

std::vector<size_t> find_all_idx(const std::string& s, char pattern);

size_t find_nth(const std::string& s, char pattern, size_t n);

std::string replace_all(const std::string& s, char pattern, const std::string& replace);

template<typename T>
std::vector<T> max_n(std::vector<T> list, size_t n) {
	std::sort(list.begin(), list.end(), std::greater<T>());
	if (list.size() < n) {
		return list;
	}
	return std::vector<T>(list.begin(), list.begin() + n);
}

template<typename T>
std::vector<T> min_n(std::vector<T> list, size_t n) {
	std::sort(list.begin(), list.end(), std::less<T>());
	if (list.size() < n) {
		return list;
	}
	return std::vector<T>(list.begin(), list.begin() + n);
}

template<typename T>
std::set<T> set_intersection(std::set<T> a, std::set<T> b) {
	std::set<T> res{};
	std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.begin()));
	return res;
}

template<typename T>
std::set<T> set_sym_diff(std::set<T> a, std::set<T> b) {
	std::set<T> res{};
	std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.begin()));
	return res;
}

template<typename T>
std::set<T> set_diff(std::set<T> a, std::set<T> b) {
	std::set<T> res{};
	std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.begin()));
	return res;
}

template<typename T, typename U>
std::set<T> map_key_set(const std::map<T, U>& map) {
	std::set<T> res{};
	for (auto it = map.begin(); it != map.end(); ++it) {
		res.insert(it->first);
	}
	return res;
}

template<typename T, typename U>
std::vector<T> map_key_list(const std::map<T, U>& map) {
	std::vector<T> res{};
	for (auto it = map.begin(); it != map.end(); ++it) {
		res.push_back(it->first);
	}
	return res;
}

template<typename T, typename U>
std::set<T> map_key_set(const std::unordered_map<T, U>& map) {
	std::set<T> res{};
	for (auto it = map.begin(); it != map.end(); ++it) {
		res.insert(it->first);
	}
	return res;
}

template<typename T, typename U>
std::vector<T> map_key_list(const std::unordered_map<T, U>& map) {
	std::vector<T> res{};
	for (auto it = map.begin(); it != map.end(); ++it) {
		res.push_back(it->first);
	}
	return res;
}

template<typename T, typename U>
std::unordered_map<T, U> invert_map(const std::unordered_map<U, T>& map) {
	std::unordered_map<T, U> res{};
	for (const auto& [key, value] : map) {
		res[value] = key;
	}
	return res;
}

template<typename T, typename U>
std::unordered_map<T, std::vector<U>> invert_map_vec(const std::unordered_map<U, std::vector<T>>& map) {
	std::unordered_map<T, std::vector<U>> res{};
	for (const auto& [key, values] : map) {
		for (const auto& value : values) {
			auto it = res.find(value);
			if (it == res.end()) {
				res.emplace(value, std::vector<U>{key});
			} else {
				it->second.push_back(key);
			}
		}
	}
	return res;
}

template<typename T, typename U>
std::map<T, U> invert_map(const std::map<U, T>& map) {
	std::map<T, U> res{};
	for (const auto& [key, value] : map) {
		res[value] = key;
	}
	return res;
}

template<typename T, typename U>
std::map<T, std::vector<U>> invert_map_vec(const std::map<U, std::vector<T>>& map) {
	std::map<T, std::vector<U>> res{};
	for (const auto& [key, values] : map) {
		for (const auto& value : values) {
			auto it = res.find(value);
			if (it == res.end()) {
				res.emplace(value, std::vector<U>{key});
			} else {
				it->second.push_back(key);
			}
		}
	}
	return res;
}

std::vector<int> diffs(const std::vector<int>& vec);
std::vector<int> diffs(const std::vector<int>& vec1, const std::vector<int>& vec2);

std::vector<std::string> rotate90c(const std::vector<std::string>& matrix);

template<typename T>
std::vector<std::vector<T>> rotate90c(const std::vector<std::vector<T>>& matrix) {
	int n = matrix.size();
	auto res = std::vector<std::vector<T>>{};
	for (int i = 0; i < n; ++i) {
		res.emplace_back(std::vector<T>{});
	}

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			res[i].emplace_back(matrix[n-j-1][i]);
		}
	}

	return res;
}

template<typename T>
std::vector<std::vector<T>> rotate90cc(const std::vector<std::vector<T>>& matrix) {
	int n = matrix.size();
	auto res = std::vector<std::vector<T>>{};
	for (int i = 0; i < n; ++i) {
		res.emplace_back(std::vector<T>{});
	}

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			res[i].emplace_back(matrix[j][n-i-1]);
		}
	}

	return res;
}

bool inbounds(int x, int y, int w, int h);

bool inbounds(int x, int y, int w, int h, int bw, int bh);

Vec2i arrow_dir(char c);

template<typename T, typename U>
struct std::hash<std::pair<T, U>> {
	size_t operator()(const std::pair<T, U>& pair) const noexcept {
		return std::hash<T>()(pair.first) ^ std::hash<U>()(pair.second);
	}
};

template<typename... Ts>
struct std::hash<std::tuple<Ts...>> {
	size_t operator()(const std::tuple<Ts...>& tuple) const noexcept {
		return std::apply([](const auto& ... xs){ return (std::hash<Ts>()(xs) ^ ...); }, tuple);
	}
};

int num_len(long long n);

template<typename T>
T mod_math(T a, T b) {
	return (a % b + b) % b;
}

template<typename T>
constexpr std::optional<int> leading_zeros(T n) {
	if (n == 0) return std::nullopt;
	int i = sizeof(T) * 8 -1;
	while (((1 << i) & n) == 0) --i;
	return i;
}

#endif //UTILS_H
