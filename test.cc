#include "./tortellini.hh"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

static const string ini_text = R"INI(
naked = no

[Sauce]
is-saucy = yes
tasty = yes
grams_of_fat = 100000
who-cares = nobody

[   Pasta   ]
whole-wheat = nah, fam!
al_dente=1
steam-levels = 42.9

invalid key = will be thrown out

[noodle face]
cute =        true
 smiling        = of course; you cannot deny it!

[HEXME]
color = 0xAABBCCDD
)INI";

int main() {
	tortellini::ini ini;

	istringstream iss(ini_text);
	iss >> ini;

	ini["Runtime"]["string"] = "isn't this a beautiful string?";
	ini["Runtime"]["some-int"] = 12345l;
	ini["Runtime"]["some-true"] = true;
	ini["Runtime"]["some-false"] = false;
	ini[""]["naked"] = true;
	ini["InTeGeRs"]["int"] = -12345;
	ini["InTeGeRs"]["int"] = (int) -12345;
	ini["InTeGeRs"]["long"] = -12345L;
	ini["InTeGeRs"]["longlong"] = -12345LL;
	ini["InTeGeRs"]["uint"] = 12345u;
	ini["InTeGeRs"]["uint"] = (unsigned int) 12345;
	ini["InTeGeRs"]["ulong"] = 12345L;
	ini["InTeGeRs"]["ulonglong"] = 12345LL;
	ini["InTeGeRs"]["float"] = 12.34f;
	ini["InTeGeRs"]["double"] = 12.34;
	ini["InTeGeRs"]["longdouble"] = 12.34L;

	string foo = "a string";
	ini["cpptypes"]["string"] = foo;
	foo = ini["cpptypes"]["string"] | "nope";

	#define D(_s, _k, _f) std::cerr << "@@@ [" << (_s) << "] " << (_k) << " = " << (ini[(_s)][(_k)] | (_f)) << std::endl
	D("pasta", "whole-wheat", "isn't set!");
	D("RuNTiME", "string", "isn't set!");
	D("RuNTiME", "string", true);
	D("nOODLE FACE", "cute", false);
	D("nOODLE FACE", "smiling", "not set!");
	D("pasta", "al_dente", false);
	D("HEXME", "color", 0L);
	D("Integers", "int", 10);
	D("Integers", "int", (int) 10);
	D("Integers", "long", 10L);
	D("Integers", "longlong", 10LL);
	D("Integers", "uint", 10);
	D("Integers", "uint", (unsigned int) 10);
	D("Integers", "ulong", 10UL);
	D("Integers", "ulonglong", 10ULL);
	D("Integers", "float", 10.0f);
	D("Integers", "double", 10.0);
	D("Integers", "longdouble", 10.0L);
	#undef D
	std::cout << std::endl << "----------------" << std::endl << std::endl;

	cout << ini;
	return 0;
}
