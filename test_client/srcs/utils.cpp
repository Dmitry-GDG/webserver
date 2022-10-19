#include "main.hpp"

void  stringToUpper(std::string & str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::string unsignedToString99(unsigned x)
{
    std::string s;
	if (x > 99)
	{
		while (x > 99)
			x %= 100;
	}
	s += x / 10 + '0';
	s += x % 10 + '0';
    return s;
}

std::string timestamp()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return("[" + unsignedToString99(ltm->tm_hour) +  ":" + unsignedToString99(ltm->tm_min) + ":" +  unsignedToString99(ltm->tm_sec) + "]: ");
}

std::string replaceAllDelimeters(std::string &msg)
{
    size_t start_pos = 0;
    while((start_pos = msg.find(DELIMETER)) != std::string::npos)
	{
        msg.replace(start_pos, DELIMETER_LENGHT, " ");
        start_pos++;
    }
    return msg;
}

void readFileToStrInBinary(const char * pathChar, std::string & outp)
{
	std::string line;
	std::vector<std::string> outpVector;

	outp.clear();
	outpVector.clear();
	std::ifstream in(pathChar, std::ios::binary);
	if (in.is_open())
	{
		while (getline(in, line))
			outpVector.push_back(line);
		in.close();
	}
	// in.close();
	std::vector<std::string>::iterator iter = outpVector.begin();
	if  (iter != outpVector.end())
	{
		while (iter != outpVector.end())
		{
			outp += *iter;
			iter++;
			if (iter != outpVector.end())
				outp += "\n";
		}
	}
}
