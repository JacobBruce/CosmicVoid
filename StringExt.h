#pragma once
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>
//#include <locale>
//#include <codecvt>

/*inline std::wstring UTF8ToWstr(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

inline std::string WstrToUTF8(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}*/

inline std::wstring StrToWstr(std::string str)
{
	return std::wstring(str.begin(), str.end());
}


inline void ReplaceSubstr(std::string& s, std::string search, std::string replace)
{
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = s.find(search, pos);
		if (pos == std::string::npos) { break; };
        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}

inline std::vector<std::string> ExplodeStr(const std::string& text, const std::string sep)
{
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + sep.length();
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}

template<typename T>
inline std::string IntToHex(const T& var)
{
	std::ostringstream s;
	s << std::uppercase << std::hex << var;
	return s.str();
}

template<typename T>
inline std::string VarToStr(const T& var)
{
	std::ostringstream s;
	s << var;
	return s.str();
}

template<typename T>
inline std::wstring VarToWstr(const T& var)
{
	return StrToWstr(VarToStr(var));
}

inline void SuperNumStr(std::string& str)
{
    ReplaceSubstr(str, "0", "\u2070");
    ReplaceSubstr(str, "1", "\u00B9");
    ReplaceSubstr(str, "2", "\u00B2");
    ReplaceSubstr(str, "3", "\u00B3");
    ReplaceSubstr(str, "4", "\u2074");
    ReplaceSubstr(str, "5", "\u2075");
    ReplaceSubstr(str, "6", "\u2076");
    ReplaceSubstr(str, "7", "\u2077");
    ReplaceSubstr(str, "8", "\u2078");
    ReplaceSubstr(str, "9", "\u2079");
}

static inline void ltrim(std::string &s, const char c) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](int ch) {
        return c != ch;
    }));
}

static inline void rtrim(std::string &s, const char c) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [&](int ch) {
        return c != ch;
    }).base(), s.end());
}

inline std::string FormatFltStr(std::string str)
{
    std::vector<std::string> strParts = ExplodeStr(str, "e+");
    if (strParts.size() > 1) {
        ltrim(strParts[1], '0');
        SuperNumStr(strParts[1]);
        return strParts[0]+"\u00D7"+"10"+strParts[1];
    }

    strParts = ExplodeStr(str, "e-");
    if (strParts.size() > 1) {
        ltrim(strParts[1], '0');
        SuperNumStr(strParts[1]);
        return strParts[0]+"\u00D7"+"10\u207B"+strParts[1];
    } else {
        if (str.length() > 8) {
            str.resize(8);
        }
        return str;
    }
}

inline std::string FormatSecStr(float time_val)
{
    std::string unitStr = LANG(STR_SECONDS);
    if (time_val > 120.0f) {
        time_val /= 60;
        unitStr = LANG(STR_MINUTES);
        if (time_val > 120.0f) {
            time_val /= 60;
            unitStr = LANG(STR_HOURS);
            if (time_val > 48.0f) {
                time_val /= 24;
                unitStr = LANG(STR_DAYS);
            }
        }
    }
    return FormatFltStr(VarToStr(time_val)) + " " + unitStr;
}

inline std::string StarTypeStr(unsigned char type)
{
    switch (type) {
        case 0: return LANG(STR_RED_D); break;
        case 1: return LANG(STR_ORANGE_D); break;
        case 2: return LANG(STR_YELLOW_D); break;
        case 3: return LANG(STR_WHITE_D); break;
        case 4: return LANG(STR_BROWN_D); break;
        case 5: return LANG(STR_RED_G); break;
        case 6: return LANG(STR_YELLOW_G); break;
        case 7: return LANG(STR_BLUE_G); break;
        case 8: return LANG(STR_RED_S); break;
        case 9: return LANG(STR_YELLOW_S); break;
        case 10: return LANG(STR_BLUE_S); break;
        case 11: return LANG(STR_RED_H); break;
        case 12: return LANG(STR_YELLOW_H); break;
        case 13: return LANG(STR_BLUE_H); break;
        case 14: return LANG(STR_NEUTRON_S); break;
        case 15: return LANG(STR_BLACK_H); break;
        default: return LANG(STR_UNKNOWN);
    }
}

inline std::string PlanetTypeStr(unsigned char type)
{
    switch (type) {
        case 0: return LANG(STR_DWARF_PLANET); break;
        case 1: return LANG(STR_GAS_DWARF); break;
        case 2: return LANG(STR_GAS_GIANT); break;
        case 3: return LANG(STR_ICE_PLANET); break;
        case 4: return LANG(STR_ICE_GIANT); break;
        case 5: return LANG(STR_WATER_PLANET); break;
        case 6: return LANG(STR_EARTH_PLANET); break;
        case 7: return LANG(STR_DESERT_PLANET); break;
        case 8: return LANG(STR_SILICATE_PLANET); break;
        case 9: return LANG(STR_LAVA_PLANET); break;
        case 10: return LANG(STR_CARBON_PLANET); break;
        case 11: return LANG(STR_IRON_PLANET); break;
        default: return LANG(STR_UNKNOWN);
    }
}

inline std::string MoonTypeStr(unsigned char type)
{
    switch (type) {
        case 0: return LANG(STR_VOLCANIC_MOON); break;
        case 1: return LANG(STR_ICE_MOON); break;
        case 2: return LANG(STR_WATER_MOON); break;
        case 3: return LANG(STR_ROCK_MOON); break;
        default: return LANG(STR_UNKNOWN);
    }
}

inline std::string MoonFuncStr(unsigned char func)
{
    switch (func) {
        case 0: return LANG(STR_NOT_USED); break;
        case 1: return LANG(STR_PIRATE_BASE); break;
        case 2: return LANG(STR_WASTE_DISPOSAL); break;
        case 3: return LANG(STR_COLONIZATION); break;
        case 4: return LANG(STR_INDUSTRIALIZATION); break;
        case 5: return LANG(STR_COMMERCIALIZATION); break;
        case 6: return LANG(STR_PLANET_DEFENSE); break;
        case 7: return LANG(STR_SCIENTIFIC_RESEARCH); break;
        case 8: return LANG(STR_TRADING_HUB); break;
        case 9: return LANG(STR_PLANET_ENTRY_PORT); break;
        default: return LANG(STR_UNKNOWN);
    }
}

inline std::string LifeTypeStr(unsigned char type)
{
    switch (type) {
        case 0: return LANG(STR_NO_LIFE); break;
        case 1: return LANG(STR_SINGLE_CELLED); break;
        case 2: return LANG(STR_SIMPLE_LIFE); break;
        case 3: return LANG(STR_COMPLEX_LIFE); break;
        case 4: return LANG(STR_PRIMITIVE_CIV); break;
        case 5: return LANG(STR_TYPE_0_CIV); break;
        case 6: return LANG(STR_TYPE_I_CIV); break;
        case 7: return LANG(STR_TYPE_II_CIV); break;
        case 8: return LANG(STR_TYPE_III_CIV); break;
        default: return LANG(STR_UNKNOWN);
    };
}

inline std::string FormatStr(const std::string& str, const char* val)
{
    std::string result(str);
    ReplaceSubstr(result, "?", val);
    return result;
    //TODO: controller support
}
