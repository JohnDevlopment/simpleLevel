#include "pdinifile.h"
#include "log.hpp"
#include "memory.hpp"
#include "string.h"
#include "ini_error.h"

PDIniFile::PDIniFile() : m_sections(), m_cursection(nullptr), m_file(nullptr) {
	m_sections.insert(std::make_pair("", KeyValMap()));
	SetToSection("");
}

int PDIniFile::ReadFile(const std::string& file) {
	char* sNewFile = realloc_mem(m_file, String_strlen(m_file), file.length());
	if (! sNewFile) {
	  throw std::bad_alloc();
	}
	String_strcpy(sNewFile, file.c_str());
	m_file = sNewFile;
	sNewFile = nullptr;
	
	// clear old data
	m_cursection = nullptr;
	m_sections.clear();
	
	// open file
	std::ifstream ifs(file);
	Log_Assert(ifs.is_open(), "Failed to open INI file");
	
	if ( ! ifs.is_open() ) {
	  Log_SetError("unable to open %s, it does not exist or could not be read", file.c_str());
	  return -1;
	}
	
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
	
	return 0;
}

int PDIniFile::SetToSection(const std::string& sec) {
	SectionMapItr found = m_sections.find(sec);
	
	if (found == m_sections.end()) {
	  Log_SetError("Could not find section '%s'", sec);
	  return -1;
	}
	
	m_cursection = &(found->second);
	return 0;
}

//////////////////////////////////////////private functions/////////////////////////////////////////

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
	
#ifndef NDEBUG
	if (szKeyEnd == std::string::npos) {
	  std::string sError = "no equal sign found in '" + line;
	  sError += '\'';
	  ini_error e(sError);
	  throw e;
	}
#endif
	
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
	
#ifndef DEBUG
	if (beg == end || end == std::string::npos) {
	  std::string sError = "invalid section name, " + line.substr(beg);
	  ini_error e( sError );
	  throw e;
	}
#endif
	
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
