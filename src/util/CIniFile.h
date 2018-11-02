#ifndef INIFILE_H
#define INIFILE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
using namespace std;


class CIniFile
{
public:
	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

	struct Record
	{
		string Comments;
		char Commented;
		string Section;
		string Key;
		string Value;
	};

	CIniFile();
	virtual ~CIniFile();

public:
	string GetStrValue(string KeyName, string SectionName, string defaultstr);
	int    GetIntValue(string KeyName, string SectionName, int defaultint);
	unsigned int GetUIntValue(string KeyName, string SectionName, unsigned int defaultuint);
	float  GetFloatValue(string KeyName, string SectionName, float defaultfloat);
	double GetDoubleValue(string KeyName,string SectionName, double defaultdouble);
	short  GetShortValue(string KeyName,string SectionName,short defaultshort);
	unsigned short GetUShortValue(string KeyName,string SectionName,unsigned short defaultushort);
	char   GetCharValue(string KeyName,string SectionName,char defaultchar);
	unsigned char GetUCharValue(string KeyName,string SectionName,unsigned char defaultchar);

	bool Load(string FileName);
	string Content();
	vector<CIniFile::Record> GetRecord(string KeyName, string SectionName);
	vector<CIniFile::Record> GetSection(string SectionName);
	vector<string> GetSectionNames();
	vector<CIniFile::Record> GetSections();
	bool RecordExists(string KeyName, string SectionName);
	bool SectionExists(string SectionName);
private:
	vector<CIniFile::Record> m_iniFileRecordVec;

private:
	struct RecordSectionIs : std::unary_function<CIniFile::Record, bool>
	{
		std::string section_;

		RecordSectionIs(const std::string& section): section_(section){}

		bool operator()( const CIniFile::Record& rec ) const
		{
			return rec.Section == section_;
		}
	};

	struct RecordSectionKeyIs : std::unary_function<CIniFile::Record, bool>
	{
		std::string section_;
		std::string key_;

		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		bool operator()( const CIniFile::Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};

	struct AscendingSectionSort
	{
		bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator()(const CIniFile::Record& Start, const CIniFile::Record& End)
		{
			return Start.Key > End.Key;
		}
	};
};
#endif
