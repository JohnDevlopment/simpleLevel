#include "pdinifile.h"
#include "log.hpp"

PDIniFile::PDIniFile() : m_sections(), m_cursection(nullptr) {
	m_sections.insert(std::make_pair("", KeyValMap()));
	SetToSection("");
}

void PDIniFile::ReadFile(const std::string& file) {
	// clear old data
	m_cursection = nullptr;
	m_sections.clear();
	
	// open file
	std::ifstream ifs(file);
	Log_Assert(ifs.is_open(), "Failed to open INI file");
	
	// collect data
	StringMapPair curSection;
	std::string line;
	size_t szStart, szEnd;
	
	while (! ifs.eof()) {
	  if (! std::getline(ifs, line))
	  	break;
	  
	  szStart = 0;
	  szEnd = line.length();
	  
	  RemoveLeadingSpaces(line, szStart, szEnd);
	  
	  // skip empty lines
	  if (szStart >= szEnd || line[szStart] == ';') {
	  	continue;
	  }
	  // line defines a section
	  else if (line[szStart] == '[') {
	  	ParseSection(curSection, line, szStart, szEnd);
	  }
	  // line defines a key-value pair
	  else {
	  	ParseKeyValuePair(curSection, line, szStart, szEnd);
	  }
	}
	
	// add last section
	m_sections.insert(curSection);
	ifs.close();
	SetToSection("");
}

void PDIniFile::SetToSection(const std::string& sec) {
	SectionMapItr found = m_sections.find(sec);
	Log_Assert(found != m_sections.end(), "Section could not be found");
	m_cursection = &(found->second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void PDIniFile::ParseSection(StringMapPair& curSection, std::string& line, size_t& beg, size_t& end)
{
	m_sections.insert(curSection); // add previous section
	GetSectionName(line, beg, end);
	curSection.first = line;
	curSection.second.clear();
}

void PDIniFile::ParseKeyValuePair(StringMapPair& curSection, std::string& line, size_t& beg, size_t& end)
{
	StringPair pair;
	SeparateValueLine(pair, line, beg, end);
	curSection.second.insert(pair);
}

void PDIniFile::SeparateValueLine(StringPair& pair, const std::string& line, size_t& beg, size_t& end)
{
	size_t szKeyEnd, szValBeg, szCmtIdx;
	
	szKeyEnd = szValBeg = line.find_first_of('=', beg);
	Log_Assert(szKeyEnd != std::string::npos, "No equal sign found in key value");
	
	// remove spaces between key and =
	RemoveTrailingSpaces(line, beg, --szKeyEnd);
	// remove spaces in front of value
	RemoveLeadingSpaces(line, ++szValBeg, end);
	
	// locate comment
	szCmtIdx = line.find_first_of(';', szValBeg);
	if (szCmtIdx != std::string::npos) {
	  end = szCmtIdx;
	}
	
	// remove spaces from value to ';' or end
	RemoveTrailingSpaces(line, szValBeg, --end);
	
	// separate string into key-value pair
	pair.first = line.substr(beg, szKeyEnd - beg + 1);
	pair.second = line.substr(szValBeg, end - szValBeg + 1);
}

void PDIniFile::GetSectionName(std::string& line, size_t& beg, size_t& end)
{
	// go into the function with beg pointing to the first non-whitespace character
	++beg;
	
	// remove leading spaces from string
	RemoveLeadingSpaces(line, beg, end);
	end = line.find_first_of(']');
	Log_Assert(beg != end && static_cast<size_t>(end) != std::string::npos, "Invalid section name");
	
	// remove trailing spaces from string
	RemoveTrailingSpaces(line, beg, --end);
	
	// extract substring from line
	line = line.substr(beg, end - beg + 1);
}

void PDIniFile::RemoveLeadingSpaces(const std::string& line, size_t& beg, size_t end)
{
	// zero-length string
	if (beg >= end)
	  return;
	
	// increase index number until a non-whitespace character is found
	while (line[beg] == ' ' || line[beg] == '\t') {
	  if (beg == std::string::npos)
	  	break;
	  
	  ++beg;
	}
}

void PDIniFile::RemoveTrailingSpaces(const std::string& line, size_t beg, size_t& end)
{
	if (beg >= end)
	  return;
	
	while (line[end] == ' ' || line[end] == '\t')
	  --end;
}
