#ifndef PDINIFILE_H_INCLUDED
#define PDINIFILE_H_INCLUDED

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class PDIniFile {
	// key-value map
	typedef std::map<std::string, std::string> KeyValMap;
	
	// map of sections linked by string names
	typedef std::map<std::string, KeyValMap> SectionMap;
	
	// pair of string and a map
	typedef std::pair<std::string, KeyValMap> StringMapPair;
	
	// string pair
	typedef std::pair<std::string, std::string> StringPair;
	
	// constant iterator to a map of string and string
	typedef KeyValMap::const_iterator KeyValMapCItr;
	
	// iterator to the map of sections
	typedef SectionMap::iterator SectionMapItr;
	
	SectionMap m_sections;
	KeyValMap* m_cursection;
	
	// parse sections of the file
	void ParseSection(StringMapPair& curSection, std::string& line, size_t& beg, size_t& end);
	void ParseKeyValuePair(StringMapPair& curSection, std::string& line, size_t& beg, size_t& end);
	
	// separate string into key-value pair
	void SeparateValueLine(StringPair& pair, const std::string& line, size_t& beg, size_t& end);
	
	// extract a substring from between "[]" and set the two endpoints of it
	void GetSectionName(std::string& line, size_t& beg, size_t& end);
	
	// set endpoints of a string where the whitespaces are cut out
	void RemoveLeadingSpaces(const std::string& line, size_t& beg, size_t end);
	void RemoveTrailingSpaces(const std::string& line, size_t beg, size_t& end);
	
	public:
	
	PDIniFile();
	~PDIniFile() {}
	
	// read an INI file and parse its data
	void ReadFile(const std::string& file);
	
	// set INI file to the selected section
	void SetToSection(const std::string& sec);
	
	template<typename T>
	void GetValue(const std::string& key, T& value);
};

template<typename T>
void PDIniFile::GetValue(const std::string& key, T& value) {
	KeyValMapCItr citr = m_cursection->find(key);
	if (citr == m_cursection->cend()) {
	  std::cerr << "Failed to locate key\n";
	  return;
	}
	
	std::stringstream ss(citr->second);
	ss >> value;
}

template<>
inline void PDIniFile::GetValue(const std::string& key, std::string& value)
{
	KeyValMapCItr citr = m_cursection->find(key);
	if (citr == m_cursection->cend()) {
	  std::cerr << "Failed to locate key\n";
	  return;
	}
	
	value = citr->second;
}

#endif /* PDINIFILE_H_INCLUDED */
