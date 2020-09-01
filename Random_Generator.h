#pragma once
// #ifndef TEO_RANDOM_GEN
// #define TEO_RANDOM GEN

#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <typeinfo>
#include <charconv>
#include <functional>
#include <string_view>
#include <system_error>

#ifdef max
#define __TEMP__ max
#undef max
#endif

class Exception
{
	std::string msg;

public:
	enum class Type : uint8_t
	{
		Bad_Typename, Bad_Type, Bad_Seed, Bad_Integer, Bad_Conv, Misc
	};
	Exception(Type t)
	{
		if (t == Type::Bad_Typename)
			msg = "Bad type used for random data generation!";
		else if (t == Type::Bad_Type)
			msg = "Bad type used to generate random data!";
		else if (t == Type::Bad_Seed)
			msg = "Bad data used for seed generation!";
		else if (t == Type::Bad_Integer)
			msg = "Usage of two types is not allowed for integer generation!";
		else if (t == Type::Bad_Conv)
			msg = "Can't convert initial type to integer type!";
		else
			msg = "Error occured with random generation!";
	}
	const char* Error() const
	{
		return msg.data();
	}
};

template <typename R>
class Random_Generator
{
	using clock = std::chrono::high_resolution_clock;
	clock::time_point m_beginning;
	std::mt19937 m_dev;
	size_t m_size;
	R m_min, m_max, m_div;
	std::string m_value;

	constexpr const bool CompareTypeIDs(const std::string& sti) const
	{
		const std::vector<std::string> tivec { std::string(typeid(char).name()), std::string(typeid(short).name()), std::string(typeid(unsigned short).name()), 
			std::string(typeid(int).name()), std::string(typeid(unsigned).name()), std::string(typeid(long long).name()) };
		for (const auto& cmpti : tivec)
			if (sti == cmpti)
				return true;
		return false;
	}
	long long TimeSeed()
	{
		clock::duration rsd = clock::now() - m_beginning;
		long long ret = rsd.count();
		return ret;
	}
	std::string Generate()
	{
		try
		{
			size_t sz;
			std::string str;
			if (!CompareTypeIDs(std::string(typeid(R).name())) || (m_min && typeid(R) == typeid(char)))
				throw Exception(Exception::Type::Bad_Type);
			else if (typeid(R) == typeid(char))
			{
				std::uniform_int_distribution<size_t> sdist(1, std::numeric_limits<unsigned short>::max());
				std::uniform_int_distribution<int> cdist(32, 126);
				sz = (m_size ? m_size : sdist(m_dev));
				auto rc = [&]() -> char { return static_cast<char>(cdist(m_dev)); };
				for (size_t i = 0; i < sz; i++)
					str.push_back(rc());
			}
			else
			{
				auto getdignum = [](const R& num) -> size_t {
					size_t dig;
					R buffrmax = num;
					for (dig = 0; buffrmax; ++dig, buffrmax /= 10);
					return dig;
				};
				if (!m_min && !m_max)
				{
					R rmax = std::numeric_limits<R>::max();
					std::uniform_int_distribution<size_t> sdist(1, getdignum(rmax));
					std::uniform_int_distribution<int> cdist(48, 57);
					sz = sdist(m_dev);
					auto rc = [&]() -> char { return static_cast<char>(cdist(m_dev)); };
					R strint;
					std::from_chars_result res;
					do
					{
						str.clear();
						for (size_t i = 0; i < sz; i++)
						{
							if (!i)
							{
								char c;
								for (c = rc(); c == '0'; c = rc());
								str.push_back(c);
							}
							else str.push_back(rc());
						}
						res = std::from_chars(str.data(), str.data() + str.size(), strint);
					} while (static_cast<bool>(res.ec));
				}
				else
				{
					if (m_div > m_max)
						throw Exception(Exception::Type::Misc);
					std::uniform_int_distribution<long long> rdist(m_min, m_max);
					long long result;
					do
					{
						result = rdist(m_dev);
					} while (result % (m_div == 0 ? 1 : (m_div == 1 ? 2 : m_div)) != (m_div == 1 ? 1 : 0));
					std::vector<char> cvec((result == 0 ? 1 : (result > 0 ? getdignum(result) : getdignum(result) + 1)));
					if (auto [p, ec] = std::to_chars(cvec.data(), cvec.data() + cvec.size(), result); ec == std::errc())
						str = std::string_view(cvec.data(), p - cvec.data());
				}
			}
			return str;
		}
		catch (const Exception& e)
		{
			std::cerr << e.Error() << '\n';
			std::cin.get();
			exit(0);
		}
	}

public:
	Random_Generator(size_t sz = 0)
		: m_beginning(clock::now())
		, m_dev(TimeSeed())
		, m_size(sz)
		, m_min(0)
		, m_max(0)
		, m_div(0)
		, m_value(Generate())
	{}
	Random_Generator(R min, R max, R div = 0)
		: m_beginning(clock::now())
		, m_dev(TimeSeed())
		, m_size(0)
		, m_min(min)
		, m_max(max)
		, m_div(div)
		, m_value(Generate())
	{}
	void NewMax(R max)
	{
		try
		{
			if (typeid(R) == typeid(char))
				throw Exception(Exception::Type::Misc);
		}
		catch (const Exception& e)
		{
			std::cerr << e.Error() << '\n';
			std::cin.get();
			exit(0);
		}
		m_max = max;
		NewGen();
	}
	void NewGen(size_t sz = 0)
	{
		if (typeid(R) == typeid(char))
			m_size = (sz ? sz : m_size);
		m_dev.seed(TimeSeed());
		m_value = Generate();
	}
	const std::string& str() const
	{
		return m_value;
	}
	void str(std::string& ret) const
	{
		ret = m_value;
	}
	R val() const
	{
		try
		{
			if (typeid(R) == typeid(char))
				throw Exception(Exception::Type::Bad_Conv);
		}
		catch (const Exception& e)
		{
			std::cerr << e.Error() << '\n';
			std::cin.get();
			exit(0);
		}
		R ret;
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), ret);
		return ret;
	}
	R max() const
	{
		try
		{
			if (typeid(R) == typeid(char))
				throw Exception(Exception::Type::Misc);
		}
		catch (const Exception & e)
		{
			std::cerr << e.Error() << '\n';
			std::cin.get();
			exit(0);
		}
		return m_max;
	}
	void val(R& ret) const
	{
		try
		{
			if (typeid(R) == typeid(char))
				throw Exception(Exception::Type::Bad_Conv);
		}
		catch (const Exception& e)
		{
			std::cerr << e.Error() << '\n';
			std::cin.get();
			exit(0);
		}
		std::from_chars(m_value.data(), m_value.data() + m_value.size(), ret);
	}
	friend std::ostream& operator<<(std::ostream& os, Random_Generator<R>& x)
	{
		os << x.str();
		return os;
	}
	operator R() const
	{
		return val();
	}
};

// #endif // !TEO_RANDOM_GEN