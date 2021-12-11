#define CATCH_CONFIG_MAIN
#include "catch2.hpp"
#include "../include/tortellini.hh"

#include <iostream>
#include <string>
#include <sstream>

namespace{
template<typename T>
void test_read(const std::string text, typename std::enable_if<!std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	std::stringstream iss;
	iss << "v = " << text << "\n";
	iss >> ini;
	const auto uut = ini[""]["v"] | T{};
	CHECK(uut == expected);
}

template<typename T>
void test_read(const std::string text, typename std::enable_if<std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	std::stringstream iss;
	iss << "v = " << text << "\n";
	iss >> ini;
	const auto uut = ini[""]["v"] | T{};
	CHECK(uut == Approx(expected));
}

template<typename T>
void test_default_wrong_type(const typename std::enable_if<std::is_integral<T>::value, T>::type expected){
	tortellini::ini ini;
	const auto text = "v = hello\n";
	std::istringstream iss{text};
	iss >> ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == expected);
}

template<typename T>
void test_default_wrong_type(const typename std::enable_if<std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	const auto text = "v = hello\n";
	std::istringstream iss{text};
	iss >> ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == Approx(expected));
}

template<typename T>
void test_oor(const typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value, T>::type expected){
	tortellini::ini ini;
	const auto max = std::to_string(std::numeric_limits<T>::max());
	const auto oor = max + "00";
	const auto text = "v = " + oor + "\n";
	std::istringstream iss{text};
	iss >> ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == expected);
}

template<typename T>
void test_oor(const typename std::enable_if<std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	const auto max = std::to_string(std::numeric_limits<T>::max());
	const auto oor = "1" + max;
	const auto text = "v = " + oor + "\n";
	std::istringstream iss{text};
	iss >> ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == Approx(expected));
}

template<typename T>
void test_oor(const typename std::enable_if<std::is_same<T, bool>::value, T>::type expected){
	tortellini::ini ini;
	const auto text = "v = 5\n";
	std::istringstream iss{text};
	iss >> ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(!uut);
}

template<typename T>
void test_empty(const typename std::enable_if<!std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == expected);
}

template<typename T>
void test_empty(const typename std::enable_if<std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	const auto uut = ini[""]["v"] | expected;
	CHECK(uut == Approx(expected));
}

template<typename T>
void test_create(const typename std::enable_if<!std::is_floating_point<T>::value, T>::type value, const std::string alternative_text){
	tortellini::ini ini;
	ini[""]["uut"] = value;
	std::ostringstream oss;
	oss << ini;

	std::stringstream ss;
	ss << "uut = " << alternative_text << "\n";
	CHECK(oss.str() == ss.str());
}

template<typename T>
void test_create(const typename std::enable_if<std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	ini[""]["uut"] = expected;
	std::ostringstream oss;
	oss << ini;

	std::stringstream ss;
	ss << "uut = " << std::setprecision(std::numeric_limits<T>::max_digits10 -1) << expected << "\n";
	CHECK(oss.str() == ss.str());
}
}
TEST_CASE("Reading an ini"){
	tortellini::ini ini;

	SECTION("Can read in"){
		std::istringstream iss{"hello"};
		iss >> ini;
	}

	SECTION("Can read a global section"){
		const auto text = std::string{"naked = rat\n"};
		std::istringstream ss{text};
		ss >> ini;
		CHECK(std::string{ini[""]["naked"] | "none"} == "rat");
	}

	SECTION("Can read another section"){
		const auto text = std::string{"[hello]\nnaked = rat\n"};
		std::istringstream ss{text};
		ss >> ini;
		CHECK(std::string{ini["hello"]["naked"] | "none"} == "rat");
	}

	SECTION("Can read periods in key names"){
		const auto text = std::string{"foo.0 = 10\nfoo.1 = 20\n"};
		std::istringstream ss{text};
		ss >> ini;
		CHECK((ini[""]["foo.0"] | 0) == 10);
		CHECK((ini[""]["foo.1"] | 0) == 20);
	}

	SECTION("int"){
		test_read<int>("5", 5);
	}

	SECTION("string"){
		test_read<std::string>("hello", "hello");
	}

	SECTION("const char *"){
		test_read<const char*>("hello", "hello");
	}

	SECTION("char"){
		test_read<char>("4", '4'-0x30);
		test_read<unsigned char>("3", '3'-0x30);
	}

	SECTION("bool"){
		test_read<bool>("1", true);
		test_read<bool>("yes", true);
		test_read<bool>("true", true);
		test_read<bool>("0", false);
		test_read<bool>("no", false);
		test_read<bool>("false", false);
	}

	SECTION("case insensitive header"){
		const auto text = std::string{"[HelLo]\nv = 4\n"};
		std::istringstream ss{text};
		ss >> ini;
		CHECK(int{ini["HELLO"]["v"] | 9999} == 4);
	}

	SECTION("case insensitive variable name"){
		const auto text = std::string{"[hello]\nv = 4\n"};
		std::istringstream ss{text};
		ss >> ini;
		CHECK(int{ini["hello"]["V"] | 9999} == 4);
	}

	SECTION("Can handle blank lines"){
		const auto text = std::string{"\n[hello]\n\n\nv = 4"};
		std::istringstream iss{text};
		iss >> ini;
		CHECK(int{ini["hello"]["v"] | -99} == 4);
	}

	SECTION("unsigned long"){
		test_read<unsigned long>("400", 400);
	}

	SECTION("long"){
		test_read<long>("400", 400);
		test_read<long>("-32", -32);
	}

	SECTION("float"){
		test_read<float>("433", 433);
		test_read<float>("433.3", 433.3);
		test_read<float>("-432.2", -432.2);
		test_read<float>("-32", -32);
	}

	SECTION("long double"){
		test_read<long double>("433", 433);
		test_read<long double>("433.3", 433.3);
		test_read<long double>("-433", -433);
		test_read<long double>("-433.2", -433.2);
	}
	SECTION("unsigned int"){
		test_read<unsigned int>("33", 33);
	}

}

TEST_CASE("File reading failures"){
	tortellini::ini ini;
	SECTION("Discards incomplete header"){
		const auto text = "[hello\nv = 4";
		std::istringstream ss{text};
		ss >> ini;
		CHECK(int{ini["hello"]["v"] | -99} == -99);
	}
}

TEST_CASE("Can use default when value is wrong type"){

	SECTION("unsigned long"){
		test_default_wrong_type<unsigned long>(5432);
	}

	SECTION("Int"){
		test_default_wrong_type<int>(543);
		test_default_wrong_type<int>(-5432);
	}

	SECTION("bool"){
		test_default_wrong_type<bool>(false); // All wrong types are false
	}

	SECTION("double"){
		test_default_wrong_type<double>(4.3);
		test_default_wrong_type<double>(-432.02);
	}

	SECTION("uint"){
		test_default_wrong_type<unsigned int>(432);
	}

	SECTION("unsigned long long"){
		test_default_wrong_type<unsigned long long>(5432);
	}

	SECTION("long long"){
		test_default_wrong_type<long long>(5432);
		test_default_wrong_type<long long>(-432);
	}

	SECTION("long"){
		test_default_wrong_type<long>(43);
		test_default_wrong_type<long>(-432);
	}

	SECTION("long double"){
		test_default_wrong_type<long double>(543.2);
		test_default_wrong_type<long double>(-80.9);
	}

	SECTION("float"){
		test_default_wrong_type<float>(432);
		test_default_wrong_type<float>(-4.3);
	}
}

TEST_CASE("Can use defaults when value out of range"){

	SECTION("Int"){
		test_oor<int>(44);
		test_oor<int>(-42);
	}

	SECTION("bool"){
		test_oor<bool>(true);
		test_oor<bool>(false);
	}

	SECTION("double"){
		test_oor<double>(66.601);
		test_oor<double>(-543);
	}

	SECTION("uint"){
		test_oor<unsigned int>(44);
	}

	SECTION("unsigned long"){
		test_oor<unsigned long>(666);
	}

	SECTION("unsigned long long"){
		test_oor<unsigned long long>(333333);
	}

	SECTION("long long"){
		test_oor<long long>(666);
		test_oor<long long>(-543234532);
	}

	SECTION("long"){
		test_oor<long>(3324);
		test_oor<long>(-432);
	}

	SECTION("long double"){
		test_oor<long double>(432.2);
		test_oor<long double>(-54345343);
	}

	SECTION("float"){
		test_oor<float>(5432.3);
		test_oor<float>(-4343.43);
	}
}


TEST_CASE("Can use defaults when nothing present"){

	SECTION("const char*"){
		test_empty<const char*>("Hhello world");
	}

	SECTION("String"){
		test_empty<std::string>("hello world");
	}

	SECTION("Int"){
		test_empty<int>(44);
		test_empty<int>(-432);
	}

	SECTION("bool"){
		test_empty<bool>(true);
		test_empty<bool>(false);
	}

	SECTION("double"){
		test_empty<double>(654.3);
		test_empty<double>(-432.2);
	}

	SECTION("uint"){
		test_empty<unsigned int>(543);
	}

	SECTION("unsigned long"){
		test_empty<unsigned long>(543);
	}

	SECTION("unsigned long long"){
		test_empty<unsigned long long>(543);
	}

	SECTION("long long"){
		test_empty<long long>(543);
		test_empty<long long>(-4321);
	}

	SECTION("long"){
		test_empty<long>(5432);
		test_empty<long>(-432);
	}

	SECTION("long double"){
		test_empty<long double>(543.5432);
		test_empty<long double>(-543.2);
	}

	SECTION("float"){
		test_empty<float>(543.3);
		test_empty<float>(-32.2);
	}
}

template<typename T>
void test_create(const typename std::enable_if<!std::is_floating_point<T>::value, T>::type expected){
	tortellini::ini ini;
	ini[""]["uut"] = expected;
	std::ostringstream oss;
	oss << ini;

	std::stringstream ss;
	ss << "uut = " << expected << "\n";
	CHECK(oss.str() == ss.str());
}



TEST_CASE("Creating an ini"){
	tortellini::ini ini;

	SECTION("Can create string"){
		test_create<std::string>("hello world");
	}

	SECTION("Char*"){
		test_create<const char*>("hello");
	}

	SECTION("Can create bool true"){
		test_create<bool>(true, "yes");
		test_create<bool>(false, "no");
	}

	SECTION("Can create int"){
		test_create<int>(10);
		test_create<int>(-33);
	}

	SECTION("Can create double"){
		test_create<double>(432.2);
		test_create<double>(-432.0);
	}

	SECTION("Can do float"){
		test_create<float>(5432.2);
		test_create<float>(-5432);
	}

	SECTION("Can do long double"){
		test_create<long double>(5432.0);
		test_create<long double>(-543.2);
	}

	SECTION("Can create unsigned int"){
		test_create<unsigned int>(543);
	}

	SECTION("Can create long"){
		test_create<long>(5432);
	}

	SECTION("Can create ulong"){
		test_create<unsigned long>(5432);
	}

	SECTION("Can create empty top section"){
		ini[""]["thing"] = "hello";
		std::ostringstream oss;
		oss << ini;
		CHECK(oss.str() == "thing = hello\n");
	}

	SECTION("Can create multiple sections"){
		ini[""]["global"] = "section";
		ini["sub"]["section"] = 99;
		std::ostringstream ss;
		ss << ini;
		CHECK(ss.str() == "global = section\n\n[sub]\nsection = 99\n");
	}

	SECTION("Can create multiple things in multiple sections"){
		ini[""]["global"] = "section";
		ini["sub"]["section"] = 99;
		std::ostringstream ss;
		ss << ini;
		CHECK(ss.str() == "global = section\n\n[sub]\nsection = 99\n");

		ini[""]["hello"] = "world";
		ini["sub"]["terfuge"] = 666.666;
		ss.str(std::string{});
		ss << ini;

		// because of the comparison operator I can be sure the items are in alphabetical order
		CHECK(ss.str() == "global = section\nhello = world\n\n[sub]\nsection = 99\nterfuge = 666.6660000000001\n");
	}

	SECTION("Skips UTF-8 BOM"){
		std::stringstream ss;
		ss << "\xEF\xBB\xBF""foo=bar\r\nworks=true\r\n";
		std::istringstream iss(ss.str());
		iss >> ini;
		CHECK((ini[""]["foo"] | "nope") == "bar");
		CHECK((ini[""]["works"] | false) == true);
	}
}

TEST_CASE("Iterating an INI") {
	tortellini::ini ini;
	std::istringstream("[foo]\nfoo1 = true\nfoo2 = 1234\n\n[bar]\nbar1 = hello\nbar2 = 12345") >> ini;

	SECTION("Can iterate sections") {
		for (const auto &section_pair : ini) {
			const auto &name = section_pair.name;
			const auto &section = section_pair.section;

			if (name == "foo") {
				CHECK((section["foo1"] | false) == true);
				CHECK((section["foo2"] | 0) == 1234);
			} else if (name == "bar") {
				CHECK((section["bar1"] | "") == "hello");
				CHECK((section["bar2"] | 0) == 12345);
			} else {
				FAIL("too many sections");
				break;
			}
		}
	}
}
