#include "CIniFile.h"

CIniFile::CIniFile()													// Default constructor
{
}

CIniFile::~CIniFile()
{
}

// A function to trim whitespace from both sides of a given string
void Trim(std::string& str, const std::string & ChrsToTrim = " \t\n\r", int TrimDir = 0)
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == std::string::npos){str.erase(); return;}
    if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
    if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
}

bool CIniFile::Load(string FileName)
{
	string s;																// Holds the current line from the ini file
	string CurrentSection;													// Holds the current section name

	vector<CIniFile::Record>& content = m_iniFileRecordVec;
	ifstream inFile (FileName.c_str());										// Create an input filestream
	if (!inFile.is_open()) return false;									// If the input file doesn't open, then return
	content.clear();														// Clear the content vector

	string comments = "";													// A string to store comments in

	while(!std::getline(inFile, s).eof())									// Read until the end of the file
	{
		Trim(s);															// Trim whitespace from the ends
		if(!s.empty())														// Make sure its not a blank line
		{
			Record r;														// Define a new record

			if((s[0]=='#')||(s[0]==';'))									// Is this a commented line?
			{
				if ((s.find('[')==string::npos)&&							// If there is no [ or =
					(s.find('=')==string::npos))							// Then it's a comment
				{
					comments += s + '\n';									// Add the comment to the current comments string
				} else {
					r.Commented = s[0];										// Save the comment character
					s.erase(s.begin());										// Remove the comment for further processing
					Trim(s);
				}// Remove any more whitespace
			} else r.Commented = ' ';										// else mark it as not being a comment

			if(s.find('[')!=string::npos)									// Is this line a section?
			{		
				s.erase(s.begin());											// Erase the leading bracket
				s.erase(s.find(']'));										// Erase the trailing bracket
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = s;												// Set the Section value
				r.Key = "";													// Set the Key value
				r.Value = "";												// Set the Value value
				CurrentSection = s;
			}

			if(s.find('=')!=string::npos)									// Is this line a Key/Value?
			{
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = CurrentSection;									// Set the section to the current Section
				r.Key = s.substr(0,s.find('='));							// Set the Key value to everything before the = sign
				r.Value = s.substr(s.find('=')+1);							// Set the Value to everything after the = sign
			}
			if(comments == "")												// Don't add a record yet if its a comment line
				content.push_back(r);										// Add the record to content
		}
	}
	
	inFile.close();															// Close the file
	return true;
}

string CIniFile::Content()
{
	string s = ""; // Hold our return string
	const vector<CIniFile::Record>& content = m_iniFileRecordVec; // Holds the current record
	for (int i = 0; i < (int) content.size(); i++) // Loop through the content
	{
		if (content[i].Comments != "")
			s += content[i].Comments; // Add the comments
		if (content[i].Commented != ' ')
			s += content[i].Commented; // If this is commented, then add it
		if ((content[i].Key == "")) // Is this a section?
			s += '[' + content[i].Section + ']'; // Add the section
		else
			s += content[i].Key + '=' + content[i].Value; // Or the Key value to the return srting

		if (i != (int) content.size())
			s += '\n'; // If this is not the last line, add a CrLf
	}
	return s; // Return the contents
}

vector<string> CIniFile::GetSectionNames()
{
	vector<string> data;													// Holds the return data
	const vector<CIniFile::Record>& content = m_iniFileRecordVec; 					// Holds the current record
	for (int i = 0; i < (int) content.size(); i++) // Loop through the content
	{
		if (content[i].Key == "") // If there is no key value, then its a section
			data.push_back(content[i].Section); // Add the section to the return data
	}
	return data;															// Return the data
}

vector<CIniFile::Record> CIniFile::GetSection(string SectionName)
{
	vector<CIniFile::Record> data;													// Holds the return data
	const vector<CIniFile::Record>& content = m_iniFileRecordVec; // Holds the current records
	for (int i = 0; i < (int) content.size(); i++) // Loop through the content
	{
		if ((content[i].Section == SectionName) && // If this is the section name we want
				(content[i].Key != "")) // but not the section name itself
			data.push_back(content[i]); // Add the record to the return data
	}

	return data; // Return the data
}

bool CIniFile::RecordExists(string KeyName, string SectionName)
{
	vector<CIniFile::Record>& content = m_iniFileRecordVec; 					// Holds the current record													// Holds the current record
	vector<CIniFile::Record>::iterator iter = std::find_if(content.begin(),
			content.end(), CIniFile::RecordSectionKeyIs(SectionName, KeyName)); // Locate the Section/Key

	if (iter == content.end())
		return false; // The Section/Key was not found
	return true;															// The Section/Key was found
}

bool CIniFile::SectionExists(string SectionName )
{
	vector<CIniFile::Record>& content = m_iniFileRecordVec;						// Holds the current record													// Holds the current record
	vector<CIniFile::Record>::iterator iter = std::find_if(content.begin(),
			content.end(), CIniFile::RecordSectionIs(SectionName)); // Locate the Section

	if (iter == content.end())
		return false; // The Section was not found
	return true;															// The Section was found
}

vector<CIniFile::Record> CIniFile::GetRecord(string KeyName, string SectionName)
{
	vector<CIniFile::Record> data;// Holds the return data
	if( m_iniFileRecordVec.empty())
		return data;

	vector<CIniFile::Record>::iterator iter = std::find_if(m_iniFileRecordVec.begin(),
			m_iniFileRecordVec.end(), CIniFile::RecordSectionKeyIs(SectionName,
					KeyName)); // Locate the Record

	if (iter == m_iniFileRecordVec.end())
		return data; // The Record was not found

	data.push_back(*iter);												// The Record was found
	return data;															// Return the Record
}

string CIniFile::GetStrValue(string KeyName, string SectionName, string defaultstr)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return content[0].Value;											// And return the value

	return defaultstr;																// No value was found
}

int CIniFile::GetIntValue(string KeyName, string SectionName, int defaultint)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return atoi(content[0].Value.c_str());										// And return the value

	return defaultint;																// No value was found
}

unsigned int CIniFile::GetUIntValue(string KeyName, string SectionName, unsigned int defaultuint)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (unsigned int)atoi(content[0].Value.c_str());										// And return the value

	return defaultuint;
}


float CIniFile::GetFloatValue(string KeyName, string SectionName, float defaultfloat)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (float)atof(content[0].Value.c_str());										// And return the value

	return defaultfloat;																// No value was found
}

double CIniFile::GetDoubleValue(string KeyName,string SectionName, double defaultdouble)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return atof(content[0].Value.c_str());										// And return the value

	return defaultdouble;																// No value was found
}

short CIniFile::GetShortValue(string KeyName,string SectionName,short defaultshort)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (short)atoi(content[0].Value.c_str());										// And return the value

	return defaultshort;																// No value was found
}

unsigned short CIniFile::GetUShortValue(string KeyName,string SectionName,unsigned short defaultushort)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (unsigned short)atoi(content[0].Value.c_str());										// And return the value

	return defaultushort;
}

char  CIniFile::GetCharValue(string KeyName,string SectionName,char defaultchar)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (char)atoi(content[0].Value.c_str());										// And return the value

	return defaultchar;
}

unsigned char  CIniFile::GetUCharValue(string KeyName,string SectionName,unsigned char defaultuchar)
{
	vector<CIniFile::Record> content = GetRecord(KeyName,SectionName);		// Get the Record

	if(!content.empty())													// Make sure there is a value to return
		return (unsigned char)atoi(content[0].Value.c_str());										// And return the value

	return defaultuchar;
}


vector<CIniFile::Record> CIniFile::GetSections()
{
	vector<CIniFile::Record> data; // Holds the return data
	vector<CIniFile::Record>& content = m_iniFileRecordVec; // Holds the current record
	for (int i = 0; i < (int) content.size(); i++) // Loop through the content
	{
		if (content[i].Key == "") // If this is a section
			data.push_back(content[i]); // Add the record to the return data
	}
	return data;
}


