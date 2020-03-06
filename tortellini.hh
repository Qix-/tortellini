#ifndef TORTELLINI_HH__
#define TORTELLINI_HH__
#pragma once

/*
	                                ⣀⣀⣀⣀⣀⣀⣀⡀
	                         ⢀⠄         ⠉⠉⠉⠛⠛⠿⠿⢶⣶⣤⣄⡀
	                      ⢀⣠⠞⠁                   ⠈⠉⠛⠻⠶⣄⡀
	                    ⢀⣴⠟⠁       ⣀⣠⣤⠴⠖⠒⠈⠉    ⠑⠲⢤⣄⡀   ⠉⠑⠤⡀
	                   ⣰⡿⠃     ⣠⣴⡾⠟⠋⠁             ⠉⠻⢷⣦⡀
	                  ⣼⡟    ⢀⣴⡿⠛⠁                    ⠉⠻⣦     ⡀
	                 ⣼⡟    ⣠⡿⠋                 ⣀⣤⡀      ⠑⠄   ⢰⡀
	                ⢸⡟    ⣰⠟    ⢀⣤⣦⡄          ⠰⣿⣿⣿            ⣇
	                ⣿⠁   ⢠⡏     ⠸⣿⣿⡟   ⣤⣤⣀⣠⣾⣦  ⠉⠉⠁       ⢰    ⣿
	               ⢀⡏    ⢸            ⡀⠙⠿⣿⣿⠟⠁           ⢠⡏    ⣿⡄
	               ⢸⠃    ⠈           ⢸⣷                ⣴⡟     ⣿⠃
	               ⠘                 ⠘⠡⢤⣤⣀⣀          ⣠⣾⠟     ⢸⣿
	                                    ⠉⠙⠛⠛⠓     ⢀⣴⠾⠋⠁      ⣼⠇
	                       ⠑⠲⢶⣤⣤⣤⣤⣄⣀⣀⣀⣀  ⡼      ⠐⠊⠉         ⢠⡟
	                ⠸⣄         ⠈⠉⠉⠉⠉⠁   ⢸⡇                 ⢀⡞
	                 ⢻⣦                 ⢸⡇              ⢀⠄⢀⠎
	                  ⠹⢷⡀               ⠘⣿⡀         ⢀⣠⣴⠞⠁
	                    ⠑                ⠘⢧      ⢀⣠⡶⠟⠋⠁ ⠠⣷
	                       ⠒⠤⣤⣀⣀           ⠑    ⠊⠉     ⣀ ⠈
	                          ⠉⠛⠻⠿⠶⠶⠤⠤⠄  ⠠⠤⠤⣤⣤⣤⣤⣶⣶⡶⠶⠞⠛⠉

	                                TORTELLINI
	                  The really, really dumb INI file format.

	                           Made for Tide Online.
	                         Cooked up on 6 Mar, 2020.

	----------------------------------------------------------------------------

	Released under a dual-licence. Take your pick, no whammies.

	                                 UNLICENSE

	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	For more information, please refer to <http://unlicense.org/>

	                                MIT LICENSE

	Copyright (c) 2020 Josh Junon

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

	----------------------------------------------------------------------------
*/

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <type_traits>

namespace tortellini {

class ini final {
	friend class value;

	struct case_insensitive {
		static inline bool compare(const std::string &l, const std::string &r) noexcept {
			// == for OOB calls
			return ::strcasecmp(l.c_str(), r.c_str()) == 0;
		}

		inline bool operator()(const std::string &l, const std::string &r) const noexcept {
			// < for map
			return ::strcasecmp(l.c_str(), r.c_str()) < 0;
		}
	};

public:
	class section;

	class value final {
		friend section;

		std::string &_value;

		inline value(std::string &_value)
		: _value(_value)
		{}

		value(const value &) = delete;
		void operator=(const value &) = delete;

		inline value(value &&) = default;

		template <typename T, T (*Fn)(const char *, char **, int)>
		static inline T strparse(const std::string &s, T fallback) {
			if (s.empty()) return fallback;
			char *end = nullptr;
			T res = Fn(s.c_str(), &end, 0);
			return (*end || errno == ERANGE)
				? fallback
				: res;
		}

		template <typename T, T (*Fn)(const char *, char **)>
		static inline T strparse(const std::string &s, T fallback) {
			if (s.empty()) return fallback;
			char *end = nullptr;
			T res = Fn(s.c_str(), &end);
			return (*end || errno == ERANGE)
				? fallback
				: res;
		}

		template <typename T>
		typename std::enable_if<
			!std::is_convertible<T, std::string>::value,
			std::string
		>::type to_string(T r) const {
			if (std::is_same<T, bool>::value) {
				return r ? "yes" : "no";
			} else {
				return std::to_string(r);
			}
		}

		template <typename T>
		typename std::enable_if<
			std::is_convertible<T, std::string>::value,
			std::string
		>::type to_string(T r) const {
			return r;
		}

	public:
		template <typename T>
		inline value & operator =(const T &v) {
			_value = to_string(v);
			return *this;
		}

		inline bool operator |(bool fallback) const {
			return _value.empty()
				? fallback
				: (
					   case_insensitive::compare(_value, "1")
					|| case_insensitive::compare(_value, "true")
					|| case_insensitive::compare(_value, "yes")
				);
		}

		inline std::string operator |(std::string fallback) const {
			return _value.empty() ? fallback : _value;
		}

		inline std::string operator |(const char *fallback) const {
			return _value.empty() ? fallback : _value;
		}

		inline unsigned long operator |(unsigned long fallback) const {
			return strparse<unsigned long, std::strtoul>(_value, fallback);
		}

		inline unsigned long long operator |(unsigned long long fallback) const {
			return strparse<unsigned long long, std::strtoull>(_value, fallback);
		}

		inline long operator |(long fallback) const {
			return strparse<long, std::strtol>(_value, fallback);
		}

		inline long long operator |(long long fallback) const {
			return strparse<long long, std::strtoll>(_value, fallback);
		}

		inline float operator |(float fallback) const {
			return strparse<float, std::strtof>(_value, fallback);
		}

		inline double operator |(double fallback) const {
			return strparse<double, std::strtod>(_value, fallback);
		}
	};

	class section final {
		friend class ini;

		std::map<std::string, std::string, case_insensitive> &_mapref;

		inline section(std::map<std::string, std::string, case_insensitive> &_mapref)
		: _mapref(_mapref)
		{}

		section(const section &) = delete;
		void operator=(const section &) = delete;

		inline section(section &&) = default;

	public:
		inline value operator[](std::string key) {
			return value(_mapref[key]);
		}
	};

private:
	std::map<std::string, std::map<std::string, std::string, case_insensitive>, case_insensitive> _sections;

	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

public:
	inline ini() = default;
	inline ini(const ini &) = default;
	inline ini(ini &&) = default;

	inline section operator[](std::string name) noexcept {
		return section(_sections[name]);
	}

	template <typename TStream>
	friend inline TStream & operator>>(TStream &stream, tortellini::ini &ini) {
		std::string line;
		std::string section_name = "";

		while (std::getline(stream, line)) {
			trim(line);

			if (line.empty()) continue;

			if (line[0] == '[') {
				size_t idx = line.find_first_of("]");
				if (idx == std::string::npos) continue; // invalid, drop line
				section_name = line.substr(1, idx - 1);
				trim(section_name);
				continue;
			}

			std::string key;
			std::string value;

			size_t idx = line.find_first_of("=");
			if (idx == std::string::npos) continue; // invalid, drop line

			key = line.substr(0, idx);
			trim(key);
			if (
				   key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_")
				!= std::string::npos
			) {
				// invalid (keys can only be alpha-numeric-hyphen-undercore), drop line
				continue;
			}

			value = line.substr(idx + 1);
			trim(value);

			if (value.empty()) continue; // not really "invalid" but we choose not to keep it

			ini[section_name][key] = value;
		}

		return stream;
	}

	template <typename TStream>
	friend inline TStream & operator<<(TStream &stream, const tortellini::ini &ini) {
		bool has_sections = false;

		// force emit empty section if it exists
		{
			const auto &itr = ini._sections.find("");
			if (itr != ini._sections.cend()) {
				for (const auto &kv : itr->second) {
					if (kv.first.empty() || kv.second.empty()) continue;
					stream << kv.first << " = " << kv.second << std::endl;
					has_sections = true;
				}
			}
		}

		for (const auto &section : ini._sections) {
			if (section.first.empty()) continue; // already emitted


			bool has_emitted = false;

			for (const auto &kv : section.second) {
				if (kv.first.empty() || kv.second.empty()) continue;

				if (!has_emitted) {
					if (has_sections) stream << std::endl;
					stream << '[' << section.first << ']' << std::endl;
					has_emitted = true;
					has_sections = true;
				}

				stream << kv.first << " = " << kv.second << std::endl;
			}
		}

		return stream;
	}
};

}

#endif
