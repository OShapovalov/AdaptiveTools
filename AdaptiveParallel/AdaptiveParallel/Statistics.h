#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

static int numberOfParallelSection = 0;

static std::vector<int> numberForParallelSections;

static bool write = false;

bool FileExists(std::string fname)
{
	return std::ifstream(fname, std::ios::in | std::ios::_Nocreate) != NULL;
}

void ReadSettingsFromFile(std::string fname = "Settings.ini")
{
	if (!FileExists(fname))
	{
		write = true;
		return;
	}

	std::ifstream input(fname);

	while (! input.eof())
	{
		std::string s;
		std::getline(input,s);

		if (s.empty())
			break;
		
		std::istringstream stream(s);
		int number = 0;
		stream >> number;

		numberForParallelSections.push_back(number);
	} 
}

void WriteToFile(int n, std::string fname = "Settings.ini")
{
	std::ofstream output(fname, std::ios_base::app);
	output << n << std::endl;
}

bool GetWrite()
{
	if (write)
		return true;
	else if (numberForParallelSections.empty())
	{
		ReadSettingsFromFile();
	}

	return write;
}

//void WriteSettingsToFile()
//{
//	// TODO: заполнить numberForParallelSections
//}

inline int GetNumberForParallelSection(int index)
{
	//if (index+1 > (int)numberForParallelSections.size())
	//	ReadSettingsFromFile();
	return numberForParallelSections[index];
}

inline int GetNumberForParallelSection()
{
	return GetNumberForParallelSection(numberOfParallelSection++);
}