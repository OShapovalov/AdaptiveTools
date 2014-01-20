#include "AbstractParallel.h"
#include "omp.h"
#include <vector>
#include <iostream>

double AbstractParallel::GetTimeForOmpOrSerial( std::function<bool (void)> f, int n, bool omp)
{
	auto start = GetTime();

	std::vector<bool> flags(n,true);
	int i;

	if (omp)
	{
#pragma omp parallel for shared(flags) private(i)
		for (i=0; i < n; ++i )
		{
			flags[i] = f();
		}
	}
	else
	{
		for (i=0; i < n; ++i )
		{
			flags[i] = f();
		}
	}

//#pragma omp parallel for if (omp)
//	for (i=0; i < n; ++i )
//	{
//		flags[i] = f();
//	}

	auto time = GetTime() - start;

	int nTrue = 0;
	for (i=0;i<n;++i)
	{
		if (flags[i])
			++nTrue;
	}

	return time + (double)nTrue/n;
}

int AbstractParallel::GetNumberOMP( std::function<bool (void)> f )
{
	int n = 100;
	double percent;

	do
	{	
		double timeOMP = GetTimeForOmpOrSerial(f, n, true);	
		double timeSerial = GetTimeForOmpOrSerial(f, n, false);

		std::cout << "Time OMP: " << timeOMP << std::endl;
		std::cout << "Time Serial: " << timeSerial << std::endl;

		if (timeSerial > 0)
			percent = (timeOMP - timeSerial)/timeSerial;
		else percent = 0;

		n = (int) ((1+percent)*n);

		std::cout << "N: " << n << std::endl;

		n = std::max(n, 5);
		n = std::min(n, 100000);
	}
	while ( std::abs(percent) > 0.05 && n > 5 && n < 1e5);

	return n;
}

double AbstractParallel::GetTime()
{
    return omp_get_wtime();
}