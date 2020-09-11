#include <tdme/utilities/MutableString.h>

#include <string>

#include <tdme/math/Math.h>
#include <tdme/utilities/Time.h>

using std::string;
using std::to_string;

using tdme::utilities::MutableString;
using tdme::math::Math;
using tdme::utilities::Time;

MutableString::MutableString() 
{
}

MutableString::MutableString(const string& s)
{
	data = s;
}

MutableString::MutableString(int i)
{
	set(i);
}

MutableString::MutableString(float f, int32_t decimals) {
	set(f, decimals);
}

int32_t MutableString::length() const
{
	return data.size();
}

char MutableString::charAt(int32_t idx) const
{
	return data[idx];
}

MutableString& MutableString::reset()
{
	data = "";
	return *this;
}

MutableString& MutableString::set(char c)
{
	reset();
	append(c);
	return *this;
}

MutableString& MutableString::append(char c)
{
	data.push_back(c);
	return *this;
}

MutableString& MutableString::insert(int32_t idx, char c)
{
	data.insert(idx, 1, c);
	return *this;
}

MutableString& MutableString::set(const string& s)
{
	reset();
	append(s);
	return *this;
}

MutableString& MutableString::append(const string& s)
{
	data+= s;
	return *this;
}

MutableString& MutableString::insert(int32_t idx, const string& s)
{
	data.insert(idx, s);
	return *this;
}

MutableString& MutableString::set(const MutableString& s)
{
	reset();
	append(s);
	return *this;
}

MutableString& MutableString::append(const MutableString& s)
{
	data+= s.data;
	return *this;
}

MutableString& MutableString::insert(int32_t idx, const MutableString& s)
{
	insert(idx, s.data);
	return *this;
}

MutableString& MutableString::set(int32_t i)
{
	reset();
	append(i);
	return *this;
}

MutableString& MutableString::append(int32_t i)
{
	data+= to_string(i);
	return *this;
}

MutableString& MutableString::insert(int32_t idx, int32_t i)
{
	// see: http://stackoverflow.com/questions/7123490/how-compiler-is-converting-integer-to-string-and-vice-versa
	auto negative = false;
	if (i < 0) {
		negative = true;
		i = -i;
	}
	while (true == true) {
		auto remainder = i % 10;
		i = i / 10;
		insert(idx, static_cast< char >(('0' + remainder)));
		if (i == 0) {
			break;
		}
	}
	if (negative == true) {
		insert(idx, '-');
	}
	return *this;
}

MutableString& MutableString::set(float f, int32_t decimals)
{
	reset();
	append(f, decimals);
	return *this;
}

MutableString& MutableString::append(float f, int32_t decimals)
{
	insert(data.size(), f, decimals);
	return *this;
}

MutableString& MutableString::insert(int32_t idx, float f, int32_t decimals)
{
	// see: http://stackoverflow.com/questions/7123490/how-compiler-is-converting-integer-to-string-and-vice-versa
	auto integer = static_cast<int>(f);
	for (auto i = 0; i < decimals; i++) {
		auto integerDecimal = static_cast<int>(((f - integer) * Math::pow(10.0f, i + 1))) - (10 * static_cast<int>(((f - integer) * Math::pow(10.0f, i))));
		insert(idx + i, Math::abs(integerDecimal));
	}
	insert(idx, '.');
	insert(idx, Math::abs(integer));
	if (f < 0.0) insert(idx, '-');
	return *this;
}

MutableString& MutableString::delete_(int32_t idx, int32_t count)
{
	data.erase(idx, count);
	return *this;
}

int32_t MutableString::indexOf(const MutableString& s, int32_t idx) const
{
	return data.find(s.data, idx);
}

int32_t MutableString::indexOf(const MutableString& s) const
{
	return indexOf(s, 0);
}

bool MutableString::equals(const string& s2) const
{
	return data == s2;
}

bool MutableString::equals(const MutableString& s2) const
{
	return data == s2.data;
}

const string& MutableString::getString() const
{
	return data;
}

MutableString MutableString::clone() {
	return MutableString(data);
}
