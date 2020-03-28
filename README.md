<h1 align="center">
	<img height="250" src="logo.png" alt="Tortellini">
</h1>

The stupid - _and I mean really, really stupid_ - INI file reader and writer for C++11 and above. Calorie free (no dependencies)!

```c++
#include <tortellini.hh>

// (optional)
#include <fstream>

int main() {
	tortellini::ini ini;

	// (optional) Read INI in from a file.
	// Default construction and subsequent assignment is just fine, too.
	std::ifstream in("config.ini");
	in >> ini;

	// Retrieval
	//
	// All value retrievals must be done via the "default" operator
	// (the pipe operator).
	//
	// The right-hand-side type is what is returned. Anything other
	// than a string-like type causes a parse (see caveats section below).
	//
	// All keys are case-INsensitive. This includes section headers.
	std::wstring s         = ini["Section"]["key"] | "default string";
	int i                  = ini["Section"]["key"] | 42;    // default int
	long l                 = ini["Section"]["key"] | 42L;   // default long
	long long ll           = ini["Section"]["key"] | 42LL;  // default long long
	unsigned int u         = ini["Section"]["key"] | 42u;   // default unsigned int
	unsigned long ul       = ini["Section"]["key"] | 42UL;  // default unsigned long
	unsigned long long ull = ini["Section"]["key"] | 42ULL; // default unsigned long long
	float f                = ini["Section"]["key"] | 42.0f; // default float
	double d               = ini["Section"]["key"] | 42.0;  // default double
	long double ld         = ini["Section"]["key"] | 42.0L; // default long double
	bool b                 = ini["Section"]["key"] | true;  // default bool

	// Assignment
	//
	// (This example uses the same key, but of course
	// in reality this would just be overwriting the
	// same key over and over again - probably not
	// what you'd really want. I would hope I wouldn't
	// have to explain this, but you never know.)
	ini["New Section"]["new-key"] = "Noodles are tasty.";
	ini["New Section"]["new-key"] = 1234;
	ini["New Section"]["new-key"] = 1234u;
	ini["New Section"]["new-key"] = 1234L;
	ini["New Section"]["new-key"] = 1234UL;
	ini["New Section"]["new-key"] = 1234LL;
	ini["New Section"]["new-key"] = 1234ULL;
	ini["New Section"]["new-key"] = 1234.0f;
	ini["New Section"]["new-key"] = 1234.0;
	ini["New Section"]["new-key"] = 1234.0L;
	ini["New Section"]["new-key"] = true;

	// "Naked" section (top-most key/value pairs, before a section header)
	// denoted by empty string in section selector
	ini[""]["naked-key"] = "I'll be at the very top, without a section";

	// (optional) Write INI to file.
	std::ofstream out("config.ini");
	out << ini;
}
```

# Things you should know.

This library has very few bells and whistles. It doesn't do anything fancy.

Here are the guarantees/features:

- Case-insensitive keys and section headers. Reading from a key/section with different cases will work fine, and writing to an existing section/key will preserve the casing.
- Untouched output (from using `stream << ini`) is guaranteed to be parsable (by using `stream >> ini`).
- Barring I/O issues or exceptions coming from the standard library, Tortellini will not throw or abort (see below section about "invalid data").
- _Pasta in, pasta out._ Source strings (from a parse) are preserved to the output. If you use `yes` instead of `true`, Tortellini will preserve that (unless the application overwrites the value).
- `yes`, `1` and `true` are all parsable as `bool(true)`. All other values equate to `false`.
- All values are inherently string, and only parsed when retrieved.

Here are the caveats:

- **Do not store or share anything returned from the subscript operators (`[]`).** They return temporary objects that are meant for short-lived interactions. They are NOT lifetime safe and expect the underlying `tortellini::ini` instance to subsist beyond their own lifetimes.
- Invalid keys, values or section names skip the line entirely. This condition is henceforth referred to as "invalid data".
- Integer overflows when parsing are invalid data.
- Mismatched `]` for a `[` line is invalid data. Yes, keys _will_ bleed into the preceding section name. That's user error, not your application's. _Embrace it._
- Empty keys or empty values (excluding leading/trailing whitespace!) are "invalid data" in that they might be valid but not included in the resulting data and thus _won't_ be re-emitted.
- Empty sections (or sections with 100% invalid data as key/value pairs) are themselves invalid data and are not emitted.
- `[]` is a valid section name. It means the "naked" section. Re-emitting the INI will move those keys to the top regardless of where `[]` is positioned in the input file.
- No comments are supported; `;` is a valid (string) character.
- No caching or memoization; if you retrieve anything but a `std::wstring`, there _will_ be a parse. I never said Tortellini was hyper-over-optimized.

# License

You have two choices in license. Pick whichever one you want.
Tortellini is uncucumbered. Go crazy. Eat some pasta.

### Unlicense

```
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
```

### MIT License

```
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
```
