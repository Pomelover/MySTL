#pragma once
#include<iostream>
#include<stdexcept>
#include <algorithm> 
namespace MySTL
{
	class string
	{
	public:
		using size_type = std::size_t;
	private:
		static constexpr size_type minCap = 200;
		size_type cap;
		size_type len;
		char* str;

		int compareString(const string& other) const
		{
			size_type n = (len < other.len) ? len : other.len;
			for (size_type i = 0; i < n; ++i)
			{
				if (str[i] < other.str[i]) return -1;
				if (str[i] > other.str[i]) return 1;
			}
			if (len < other.len) return -1;
			if (len > other.len) return 1;
			return 0;
		}


		static size_type computeCap(size_type sz)
		{
			return std::max((sz + minCap + 1) / minCap * minCap, minCap);
		}

		
		void resetToEmpty(string& s)
		{
			s.cap = minCap;
			s.len = 0;
			s.str = new char[s.cap];
			s.str[0] = '\0';
		}
	public:
		// 构造函数
		string() : cap(minCap), len(0), str(new char[minCap]) { *str = '\0'; }

		string(const string& s):cap(s.cap),len(s.len),str(new char[cap])
		{
			if (s.str == nullptr)
			{
				delete[] str;
				cap = minCap;
				len = 0;
				str = new char[cap];
				*str = '\0';
				return;
			}
			for (size_type i = 0; i < len; ++i) {
				*(str + i) = *(s.str + i);
			}
			*(str + len) = '\0';
		}


		string& operator=(const string& s)
		{
			if (this == &s) return *this;

			if (s.str == nullptr)
			{
				char* newBuf = new char[minCap];
				*newBuf = '\0';

				delete[] str;
				str = newBuf;
				cap = minCap;
				len = 0;
				return *this;
			}

			char* newBuf = new char[s.cap];
			for (size_type i = 0; i < s.len; ++i) {
				*(newBuf + i) = *(s.str + i);
			}
			*(newBuf + s.len) = '\0';

			delete[] str;
			str = newBuf;
			cap = s.cap;
			len = s.len;

			return *this;
		}


		string(string&& s) noexcept
			: cap(s.cap), len(s.len), str(s.str)
		{
			resetToEmpty(s);
		}

		string& operator=(string&& s) noexcept 
		{
			if (this == &s) return *this;
			delete[] str;
			cap = s.cap;
			len = s.len;
			str = s.str;
			resetToEmpty(s);
			return *this;
		}


		string(const char* cstr) :cap(0), len(0), str(nullptr)
		{
			if (cstr == nullptr)
			{
				cap = minCap;
				len = 0;
				str = new char[cap];
				*str = '\0';
				return;
			}
			const char* cur = cstr;
			while (*cur != '\0')
			{
				++cur;
				++len;
			}
			cap = computeCap(len);
			str = new char[cap];
			for (size_type i = 0; i < len; ++i) 
			{
				*(str + i) = *(cstr + i);
			}
			*(str + len) = '\0';
		}


		string(size_type sz, char c) : cap(computeCap(sz)), len(sz), str(new char[cap]) 
		{
			for (size_type i = 0; i < sz; ++i)
			{
				*(str + i) = c;
			}
			*(str + len) = '\0';
		}

		~string() {
			delete[] str;
		}

		// 容量管理 
		size_type size() const noexcept
		{
			return len;
		}

		size_type capacity() const noexcept
		{
			return cap;
		}

		bool empty() const noexcept
		{
			return len == 0;
		}

		void reserve(size_type n)
		{
			if (n <= cap) return;

			size_type newCap = computeCap(n);
			char* newStr = new char[newCap];

			for (size_type i = 0; i < len; ++i)
			{
				newStr[i] = str[i];
			}
			newStr[len] = '\0';

			delete[] str;
			str = newStr;
			cap = newCap;
		}


		void resize(size_type n, char c)
		{
			if (n == len) return;
			if (n < len)
			{
				len = n;
				*(str + len) = '\0';
			}
			else
			{
				reserve(n);
				while (len < n)
				{
					*(str + len) = c;
					++len;
				}
				*(str + len) = '\0';
			}
		}

		// 元素访问
		char& operator[](size_type i) 
		{
			return *(str + i);
		}
		const char& operator[](size_type i) const
		{
			return *(str + i);
		}

		char& at(size_type i)
		{
			if (i >= len) throw std::out_of_range("Index out of range!");
			return *(str + i);
		}
		const char& at(size_type i) const
		{
			if (i >= len) throw std::out_of_range("Index out of range!");
			return *(str + i);
		}

		char& front()
		{
			return at(0);
		}
		const char& front() const
		{
			return at(0);
		}

		char& back()
		{
			return at(len-1);
		}
		const char& back() const
		{
			return at(len - 1);
		}

		char* data()
		{
			return str;
		}
		const char* data() const
		{
			return str;
		}

		const char* c_str() const
		{
			return str;
		}

		// 运算符
		bool operator<(const string& rhs)const { return compareString(rhs) < 0; }
		bool operator<=(const string& rhs)const { return compareString(rhs) <= 0; }
		bool operator>(const string& rhs)const { return compareString(rhs) > 0; }
		bool operator>=(const string& rhs)const { return compareString(rhs) >= 0; }
		bool operator==(const string& rhs)const { return compareString(rhs) == 0; }
		bool operator!=(const string& rhs)const { return compareString(rhs) != 0; }
		string& operator+=(const string& rhs)
		{
			if (rhs.len == 0) return *this;

			size_type oldLen = len;
			size_type addLen = rhs.len;
			size_type newLen = oldLen + addLen;
			reserve(newLen);
			for (size_type i = 0; i < addLen; ++i) {
				*(str + oldLen + i) = *(rhs.str + i);
			}
			len = newLen;
			str[len] = '\0';
			return *this;
		}
		string operator+(const string& rhs) const
		{
			string newStr(*this);
			newStr += rhs;
			return newStr;
		}

		string& operator+=(char c)
		{
			reserve(len + 1);
			str[len] = c;
			++len;
			str[len] = '\0'; 
			return *this;
		}
		string operator+(char c) const
		{
			string newStr(*this);
			newStr += c;
			return newStr;
		}

		string& operator+=(const char* cstr)
		{
			if (cstr == nullptr) return *this;

			bool alias = false;
			for (size_type i = 0; i <= len; ++i)
			{
				if (str + i == cstr)
				{
					alias = true;
					break;
				}
			}

			if (alias)
			{
				string tmp(cstr); 
				return *this += tmp;
			}
			size_type addLen = 0;
			const char* cur = cstr;
			while (*cur != '\0')
			{
				++cur;
				++addLen;
			}

			if (addLen == 0) return *this;

			reserve(len + addLen);

			for (size_type i = 0; i < addLen; ++i)
			{
				str[len + i] = cstr[i];
			}

			len += addLen;
			str[len] = '\0';

			return *this;
		}

		string operator+(const char* cstr) const
		{
			string newStr(*this);
			newStr += cstr;
			return newStr;
		}

		friend string operator+(const char* lhs, const string& rhs)
		{
			if (lhs == nullptr)
			{
				return rhs;
			}
			string result(lhs);
			result += rhs;
			return result;
		}

		// 修改操作
		void push_back(char c)
		{
			*this += c;
		}

		void append(const string& rhs)
		{
			*this += rhs;
		}

		// 迭代器
		char* begin() { return str; }
		char* end() { return str + len; }
		const char* cbegin() const { return str; }
		const char* cend() const { return  str + len; }
	};
}
