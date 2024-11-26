// This was written by DJ to make chemedata just using the parser from:
/*****************************************************/
/***                ANATOLIA V1.2                  ***/
/***     parser of Anatolia files    based on      ***/
/*****************************************************/
/***          http://anatolia.nmrclub.ru           ***/
/***     https://github.com/dcheshkov/ANATOLIA     ***/
/*****************************************************/
/***  This program is free software: you can       ***/
/***  redistribute it and/or modify it under the   ***/
/***  terms of the GNU General Public License as   ***/
/***  published by the Free Software Foundation,   ***/
/***  either version 3 of the License, or (at      ***/
/***  your option) any later version.              ***/
/*****************************************************/
/***  This program is distributed in the hope that ***/
/***  it will be useful,but WITHOUT ANY WARRANTY;  ***/
/***  without even the implied warranty of         ***/
/***  MERCHANTABILITY or FITNESS FOR A PARTICULAR  ***/
/***  PURPOSE. See the GNU General Public License  ***/
/***  for more details.                            ***/
/*****************************************************/
/***  You should have received a copy of the GNU   ***/
/***  General Public License along with this       ***/
/***  program.                                     ***/
/***  If not, see <http://www.gnu.org/licenses/>.  ***/
/*****************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cmath>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

char Title[256];
int nSpins = 0;
int* Offs = NULL;
int** JCoups = NULL;
int nSSParams = 0;
double* SSParams = NULL;
int nFreqsFiltered = 0;
double* FreqsFiltered = NULL;
double* IntensFiltered = NULL;

#define defaultprecision 6
#define IntensityThreshold 0.005

int nBroadenings = 0;
double* LBs = NULL;

char textline[256];

#define uint_t uint64_t
#define BF_t uint64_t

#ifdef _WIN32
#define chdir _chdir
#define exit_ exit_with_pause()
bool ExitWithPause = true;
void exit_with_pause(void)
{

	if (!ExitWithPause) exit(0);
	char textline[8];
	cout << "Press any key..." << endl;
	cin.getline(textline, 8);
	exit(0);

}
#else
#define exit_ exit(0)
#endif

long long int fact(const int input) {
	const long long int max = static_cast<long long int>(input);
	long long int a = 1;
	for (long long int i = 1; i <= max; i++) {
		a *= i;
	}
	return a;
}

void print_citation(ostream& ostr)
{

	ostr
	//	<< "D.A. Cheshkov, K.F. Sheberstov, D.O. Sinitsyn, V.A. Chertkov, ANATOLIA: NMR" << endl
	//	<< "software for spectral analysis of total lineshape. Magn. Reson. Chem., 2018," << endl
	//	<< "56, 449, DOI: 10.1002/mrc.4689." << endl
	<< "Here eventually add a citation for CHEMeDATA:" << endl;

}

bool isunsignint(char* text)
{

	bool res = int(text[0]) != 0;
	for (int i = 0; int(text[i]) != 0; i++)
		res = res && isdigit(text[i]);
	return(res);

}

bool isunsignreal(char* text)
{

	bool res = int(text[0]) != 0;
	int point = 0;
	for (int i = 0; int(text[i]) != 0; i++)
	{
		if (text[i] == '.') point++;
		res = res && (point < 2) && (isdigit(text[i]) || text[i] == '.');
	}
	return(res);

}

bool isreal(char* text)
{

	int j = (text[0] == '-') || (text[0] == '+') ? 1 : 0;
	return isunsignreal(text + j);

}

bool isemptyline(char* text)
{

	bool res = true;
	for (int i = 0; int(text[i]) != 0; i++)
		res = res && isspace(text[i]);
	return(res);

}

inline int getbit(BF_t number, int position)
{
    if (number & ((BF_t)1 << (position - 1))) return 1;
    return 0;
}

inline int getbitsum(BF_t number)
{
    int result = 0;
    BF_t mask = (BF_t)1 << (nSpins - 1);
    for (int i = 1; i <= nSpins; i++)
    {
        if (number & mask) result++;
        mask /= 2;
    }
    return result;
}
class Data
{
public:
	int nPoints;
	double Magnitude;
	double* Points;
	char* Filename;
	char* refinementFilename;
	char* saveInputFilename;
	char* saveOutputFilename;

	Data(void)
	{
		Filename = new char[256];
		refinementFilename = new char[256];
		saveInputFilename = new char[256];
		saveOutputFilename = new char[256];
		strcpy(Filename, "");
		strcpy(refinementFilename, "");
		strcpy(saveInputFilename, "");
		strcpy(saveOutputFilename, "");
		nPoints = 0;
		Magnitude = 0;
		Points = NULL;

	}

	void Create(void)
	{

		if (Points != NULL) { delete[] Points; Points = NULL; }

		if (nPoints > 0)
		{
			Points = new double[(uint_t)nPoints + 1];
			for (int i = 0; i <= nPoints; i++)
				Points[i] = 0;
		}

	}

	void Clean(void)
	{

		if (Points != NULL) { delete[] Points; Points = NULL; }
		nPoints = 0;
		Magnitude = 0;

	}

	void Rescale(int PivotPointIndex)
	{

		double rescaling = Magnitude / (PivotPointIndex == 0 ? CalcMagnitude() : Points[PivotPointIndex]);
		for (int i = 1; i <= nPoints; i++)
			Points[i] = rescaling * Points[i];

	}

	double CalcMagnitude(void)
	{

		double MaxIntens = 0;
		for (int i = 1; i <= nPoints; i++)
			if (MaxIntens < abs(Points[i])) MaxIntens = abs(Points[i]);
		return(MaxIntens);

	}

	void SaveSpecToFile(int which = 0)
	{
		if (Filename[0] == '-') return;
		int point = 0;
		char* trueFilename;
		if (which == 0) trueFilename = saveOutputFilename;
		if (which == 1) trueFilename = saveInputFilename;
		if (which == 2) trueFilename = refinementFilename;
		ofstream ostr(trueFilename);
		for (int i = 1; i <= nPoints; i++)
		{
			if (i > 1) ostr << ",";
			ostr << Points[i] ;
			if ((i % 1024) == 0) ostr << endl;

		}
		ostr.close();
	}

	void LoadSpecFromFile(void)
	{

		int point = 0;

		if (int(Filename[0]) == 0) { cout << "Can't load spectrum, filename is empty!" << endl; exit_; }
		ifstream istr(Filename, ios::in | ios::binary | ios::ate);
		if (!istr) { cout << "File " << Filename << " does not exists!" << endl; exit_; }

		nPoints = (int)istr.tellg() / 4;
		istr.seekg(0);
		if (Points != NULL) delete[] Points;
		Points = new double[(uint_t)nPoints + 1];
		Points[0] = 0;

		Magnitude = 0;
		for (int i = 1; i <= nPoints; i++)
		{
			istr.read((char*)&point, 4);
			Points[i] = double(point);
			if (Magnitude < abs(Points[i])) Magnitude = abs(Points[i]);
		}
		istr.close();

	}

	void ZeroData(void)
	{

		for (int i = 0; i <= nPoints; i++)
			Points[i] = 0;

	}

};

class Spectrum
{

public:
	char* DatasetPath;
	int ExpProcNo;
	int BrExpProcNo;
	int TheorProcNo;
	Data ExperimentalSpec;
	Data ExperimentalSpecWithBroadening;
	Data TheoreticalSpec;
	//double LineWidth; => Global LineWidth
	double Offset;
	double FreqStep;
	double SF;
	double LB;
	int nIntervals;
	int* StartPoint;
	int* EndPoint;
	int nPointsRated;
	double* ExpSpecPointsOnIntervals;
	double* TheorSpecPointsOnIntervals;
	double* FreqsOnIntervals;
	double* Lorentz;
	double SumOfExpSquaresOnIntervals;
	char* SpectraTextOutputFilename;
	bool ScaleOpt;
	bool FineCalc;

	Spectrum(ifstream& istr)
	{

		DatasetPath = new char[256];
		SpectraTextOutputFilename = new char[256];
		SF = 0;
		LB = 0;
		ScaleOpt = false;
		FineCalc = true;
		SumOfExpSquaresOnIntervals = 0;
		Lorentz = NULL;

		istr.getline(textline, 256); // Spectra parameters
		istr >> textline >> DatasetPath;
		for (int i = 0; int(DatasetPath[i]) != 0; i++) if (DatasetPath[i] == '\\') DatasetPath[i] = char('/');

		// Reading data from ACQUS
		double BF = 0, O1_h = 0, SW_h = 0;
		sprintf(textline, "%s/acqus", DatasetPath);
		ifstream dataset(textline);
		if (!dataset) { cout << "File ACQUS does not exists!" << endl; exit_; }
		while (dataset)
		{
			dataset >> textline;
			if (strstr(textline, "$BF1=") != NULL)  dataset >> BF;
			if (strstr(textline, "$O1=") != NULL)   dataset >> O1_h;
			if (strstr(textline, "$SW_h=") != NULL) dataset >> SW_h;
			if (strstr(textline, "$SW_H=") != NULL) dataset >> SW_h;
		}
		dataset.close();
		if(BF == 0 || SW_h == 0) { cout << "File ACQUS corrupted!" << endl; exit_; }

		istr >> textline >> textline;
		if (!isunsignint(textline)) { cout << "Wrong experimental spectrum processing number." << endl; exit_; }
		ExpProcNo = atoi(textline);

		// Reading data from PROCS of experimental spectrum
		sprintf(ExperimentalSpec.Filename, "%s/pdata/%i/procs", DatasetPath, ExpProcNo);
		dataset.open(ExperimentalSpec.Filename);
		if (!dataset) { cout << "File PROCS in processing number " << ExpProcNo << " does not exists!" << endl; exit_; }
		sprintf(ExperimentalSpec.Filename, "%s/pdata/%i/1r", DatasetPath, ExpProcNo);
		while (dataset)
		{
			dataset >> textline;
			if (strstr(textline, "$SF=") != NULL) dataset >> SF;
			if (strstr(textline, "$SI=") != NULL) dataset >> TheoreticalSpec.nPoints;
		}
		dataset.close();

		if(SF == 0 || TheoreticalSpec.nPoints == 0) { cout << "File PROCS in processing number " << ExpProcNo << " corrupted!" << endl; exit_; }
		ExperimentalSpecWithBroadening.nPoints = TheoreticalSpec.nPoints;

		Offset = (BF - SF) * 1e6 + O1_h + 0.5 * SW_h; // SR = SF - BF; SR *= 1e6; Offset = O1_h - SR + 0.5 * SW_h;
		FreqStep = SW_h / TheoreticalSpec.nPoints;

		istr >> textline >> textline;
		if (textline[0] == '-')
			strcpy(ExperimentalSpecWithBroadening.Filename, "-");
		else
		{
			if (!isunsignint(textline)) { cout << "Check the processing number for experimental spectrum with broadening." << endl; exit_; }
			BrExpProcNo = atoi(textline);
			sprintf(ExperimentalSpecWithBroadening.Filename, "%s/pdata/%i/1r", DatasetPath, BrExpProcNo);
		}

		istr >> textline >> textline;
		if (textline[0] == '-')
			strcpy(TheoreticalSpec.Filename, "-");
		else
		{
			if (!isunsignint(textline)) { cout << "Check the processing number for theoretical spectrum." << endl; exit_; }
			TheorProcNo = atoi(textline);
			sprintf(TheoreticalSpec.Filename, "%s/pdata/%i/1r", DatasetPath, TheorProcNo);
			sprintf(TheoreticalSpec.refinementFilename, "%s/pdata/%i/refinementSpectrum.txt", DatasetPath, ExpProcNo);
			sprintf(TheoreticalSpec.saveOutputFilename, "%s/pdata/%i/outputSpectrum.txt", DatasetPath, ExpProcNo);
			sprintf(TheoreticalSpec.saveInputFilename, "%s/pdata/%i/inputSpectrum.txt", DatasetPath, ExpProcNo);
		}
		istr.getline(textline, 256); // Rest of proc. no. line
		istr.getline(textline, 256); // Empty line
		if (!isemptyline(textline)) { cout << "Empty line should follow the section with spectra parameters!" << endl; exit_; }

		LoadIntervals();

		ExperimentalSpec.LoadSpecFromFile();
		if (ExperimentalSpec.nPoints != TheoreticalSpec.nPoints) { cout << "File with experimental spectrum is corrupted." << endl; exit_; }

		//CalcExpSpecMagnOnIntervals
		ExperimentalSpec.Magnitude = 0;
		for (int i = 1; i <= nIntervals; i++)
			for (int j = StartPoint[i]; j <= EndPoint[i]; j++)
				if (ExperimentalSpec.Magnitude < abs(ExperimentalSpec.Points[j])) ExperimentalSpec.Magnitude = abs(ExperimentalSpec.Points[j]);

		ExperimentalSpecWithBroadening.Magnitude = ExperimentalSpec.Magnitude;

		TheoreticalSpec.Create();
		ExperimentalSpecWithBroadening.Create();

	}

	void LoadIntervals(void)
	{

		stringstream parse;
		int position = 0;
		bool ok = false;
		sprintf(textline, "%s/pdata/%i/integrals.txt", DatasetPath, ExpProcNo);
		ifstream istr(textline);
		if (!istr) { cout << "File integrals.txt does not exist in the processing folder with experimental spectrum!" << endl; exit_; }

		nIntervals = 0;

		while (istr)
		{
			istr.getline(textline, 256); position++;
			if (strstr(textline, "Integral") != NULL) { ok = true; break; }
		}
		if (!ok) { cout << "File integrals.txt in the processing folder with experimental spectrum is corrupted!" << endl; exit_; }

		int i = 0;
		istr.getline(textline, 256);
		while (istr)
		{
			i++;
			parse << textline;
			parse >> nIntervals;
			if (nIntervals != i) { cout << "File integrals.txt in the processing folder with experimental spectrum is corrupted!" << endl; exit_; }
			parse.str(""); parse.clear();
			istr.getline(textline, 256);
		}

		if (nIntervals < 1) { cout << "There is no defined intervals in the integrals.txt file for the experimental spectrum!" << endl; exit_; }

		istr.clear(); istr.seekg(0);
		for (i = 1; i <= position; i++) istr.getline(textline, 256);

		int j = 0;
		double StartPPM = 0, EndPPM = 0, IntVal = 0;
		double PPMOffset = Offset / SF;
		double PPMStep = FreqStep / SF;

		StartPoint = new int[(uint_t)nIntervals + 1];
		EndPoint = new int[(uint_t)nIntervals + 1];
		StartPoint[0] = 0;
		EndPoint[0] = 0;
		nPointsRated = 0;

		for (i = 1; i <= nIntervals; i++)
		{
			istr.getline(textline, 256);
			parse << textline;
			parse >> j >> StartPPM >> EndPPM >> IntVal;
			parse.str(""); parse.clear();
			StartPoint[i] = int((PPMOffset - StartPPM) / PPMStep) - 1;
			if (StartPoint[i] < 1) StartPoint[i] = 1;
			if (StartPoint[i] > TheoreticalSpec.nPoints) StartPoint[i] = TheoreticalSpec.nPoints;
			EndPoint[i] = int((PPMOffset - EndPPM) / PPMStep) + 1;
			if (EndPoint[i] < 1) EndPoint[i] = 1;
			if (EndPoint[i] > TheoreticalSpec.nPoints) EndPoint[i] = TheoreticalSpec.nPoints;
			if (EndPoint[i] - StartPoint[i] <= 2) { cout << "Spectrum interval " << i << " is too small. Please increase the number of points." << endl; exit_; }
			nPointsRated += (EndPoint[i] - StartPoint[i] + 1);
		}

		istr.close();

		ExpSpecPointsOnIntervals = new double[(uint_t)nPointsRated + 1];
		TheorSpecPointsOnIntervals = new double[(uint_t)nPointsRated + 1];
		FreqsOnIntervals = new double[(uint_t)nPointsRated + 1];
		for (i = 0; i <= nPointsRated; i++)
		{
			ExpSpecPointsOnIntervals[i] = 0;
			TheorSpecPointsOnIntervals[i] = 0;
			FreqsOnIntervals[i] = 0;
		}

		int n = 1;
		for (int i = 1; i <= nIntervals; i++)
			for (int j = StartPoint[i]; j <= EndPoint[i]; j++)
				FreqsOnIntervals[n++] = Offset - FreqStep * ((double)j - 1);

	}
	void CheckSpinOffsets(ostream& ostr)
	{

		int MaxOffs = Offs[nSpins];

		bool check = false;

		for (int i = 1; i <= MaxOffs; i++)
		{
			double CurrOffset = SSParams[i];
			check = false;
			for (int j = 1; j <= nIntervals; j++)
				if (CurrOffset <= (Offset - FreqStep * (StartPoint[j] - 1)) && CurrOffset >= (Offset - FreqStep * (EndPoint[j] - 1))) check = true;
			if (!check) ostr << "Warning! Chemical shift no. " << i << " (" << CurrOffset << ") does not fall into any of defined spectral intervals." << endl;
		}

		for (int i = 1; i <= nIntervals; i++)
		{
			double StrartFreq = Offset - FreqStep * (StartPoint[i] - 1);
			double EndFreq = Offset - FreqStep * (EndPoint[i] - 1);
			check = false;
			for (int j = 1; j <= MaxOffs; j++)
				if (SSParams[j] <= StrartFreq && SSParams[j] >= EndFreq) check = true;
			if (!check) ostr << "Warning! Spectral interval no. " << i << " does not contain any chemical shift." << endl;
		}

		if (!check) ostr << endl;

	}
	void fixSpinOffsets(ostream& ostr)
	{
		int MaxOffs = Offs[nSpins];
		bool check = false;

		for (int i = 1; i <= MaxOffs; i++)
		{
			double CurrOffset = SSParams[i];
			check = false;
			for (int j = 1; j <= nIntervals; j++) {
				if (CurrOffset <= (Offset - FreqStep * (StartPoint[j] - 1)) && CurrOffset >= (Offset - FreqStep * (EndPoint[j] - 1))) {
					check = true;
				}
			}			
			if (!check) {
			ostr << "Warning! Chemical shift no. " << i << " (" << CurrOffset << ") does not fall into any of defined spectral intervals." << endl;
			for (int j = 0; j <= nIntervals ; j++) {
				if (j > nIntervals) continue; // just making sure value of nIntervals is not constant in the loop
				
				if (j > 0) 
					if (CurrOffset <= (Offset - FreqStep * (StartPoint[j] - 1)) && CurrOffset >= (Offset - FreqStep * (EndPoint[j] - 1))) 
						continue;
				
				int first;
				int last;
				if 	(j == 0) {
					first = 1;
					last = EndPoint[j + 1];
				}
				if 	(j == nIntervals) {
					first = StartPoint[j];
					last = ExperimentalSpec.nPoints;
				}
				if 	(j > 0 && j < nIntervals) {
					first = StartPoint[j];
					last = EndPoint[j + 1];
				}
				const double start = Offset - FreqStep * (first - 1);
				const double end = Offset - FreqStep * (last - 1);

				if (CurrOffset <= start && CurrOffset >= end) {
					if 	(j == 0) {
						StartPoint[j + 1] = first;
					}
					if 	(j == nIntervals) {
						EndPoint[j] = last;
					}
					if 	(j > 0 && j < nIntervals) {
						StartPoint[j] = first;
						EndPoint[j] = last;
					}
					ostr << "New interval region:  " 
					<< Offset - FreqStep *(first - 1)<< " : "
					<< Offset - FreqStep *(last - 1) << " "
					<< endl;
					if 	(j > 0 && j < nIntervals) {
						for (int k = j + 1; k <= nIntervals; k++) {
							StartPoint[k] = StartPoint[k + 1];
							EndPoint[k] = EndPoint[k + 1];
						}
						nIntervals -= 1;
					}
					break;
				}				
			}
			}
		}
		if (!check) {
			for (int j = 1; j <= nIntervals; j++) {
					ostr << "defined spectral intervals " << j << " ("  
					<< " " << Offset - FreqStep * (StartPoint[j] - 1)
					<< " .. " << Offset - FreqStep * (EndPoint[j] - 1) 
					<< ")"
					<< endl;
				}
		}
		bool firstTime = true;
		for (int i = 1; i <= nIntervals; i++)
		{
			double StrartFreq = Offset - FreqStep * (StartPoint[i] - 1);
			double EndFreq = Offset - FreqStep * (EndPoint[i] - 1);
			check = false;
			for (int j = 1; j <= MaxOffs; j++)
				if (SSParams[j] <= StrartFreq && SSParams[j] >= EndFreq) check = true;
			if (!check) {
				ostr << "Warning! Spectral interval no. " << i << " " << StrartFreq << " ..  " << EndFreq << " does not contain any chemical shift." << endl;
				if (firstTime) for (int j = 1; j <= MaxOffs; j++) {
					ostr << "chemical shifts " << j << " ("  
					<< " " << SSParams[j]
					<< ")"
					<< endl;
					firstTime = false;
				}
			} 
		}
		
		if (!check) ostr << endl;

	}

	void InitBroadening(double MaxLb)
	{

		int nPoints = int(ceill(sqrt(199) * MaxLb / (2 * FreqStep)));
		if (nPoints > 3) Lorentz = new double[(uint_t)nPoints + 1];

	}

	void BroadOnIntervals(double LWBroad)
	{

		LB = LWBroad;
		ExperimentalSpecWithBroadening.ZeroData();
		int step = 1;

		int nPoints = int(ceill(sqrt(199) * LB / (2 * FreqStep)));
		if (nPoints > 3)
		{
			double Betta = 2 * FreqStep / LB; Betta *= Betta;
			for (int i = 0; i <= nPoints; i++)
				Lorentz[i] = 1 / (1 + Betta * i * i);

			for (int i = 1; i <= nIntervals; i++)
				for (int j = StartPoint[i]; j <= EndPoint[i]; j++)
				{
					double* Point = ExperimentalSpecWithBroadening.Points + j;
					int startk = j - nPoints; if (startk <= 0) startk = 1;
					int stopk = j + nPoints;  if (stopk > ExperimentalSpec.nPoints) stopk = ExperimentalSpec.nPoints;
					for (int k = startk; k <= stopk; k++)
						*Point += ExperimentalSpec.Points[k] * Lorentz[abs(j - k)];
				}
			ExperimentalSpecWithBroadening.Rescale(0);
			step = int(LB / (2 * FreqStep)); if ((step == 0) || FineCalc) step = 1;
		}
		else
		{
			LB = 0;
			for (int i = 1; i <= nIntervals; i++)
				for (int j = StartPoint[i]; j <= EndPoint[i]; j++)
					ExperimentalSpecWithBroadening.Points[j] = ExperimentalSpec.Points[j];
		}

		SumOfExpSquaresOnIntervals = 0;
		int n = 1;
		double tmp = 0;
		for (int i = 1; i <= nIntervals; i++)
			for (int j = StartPoint[i]; j <= EndPoint[i]; j += step)
			{
				FreqsOnIntervals[n] = Offset - FreqStep * ((double)j - 1);
				tmp = ExperimentalSpecWithBroadening.Points[j];
				ExpSpecPointsOnIntervals[n++] = tmp; tmp *= tmp;
				SumOfExpSquaresOnIntervals += tmp;
			}
		nPointsRated = n - 1;

	}

	void CalcSpecOnIntervals(void)
	{
		if ((!FineCalc) && ScaleOpt) { CalcSpecOnIntervalsRoughWithScOpt(); return; }
		if (!FineCalc) { CalcSpecOnIntervalsRoughFixedSc(); return; }
		if (ScaleOpt) { CalcSpecOnIntervalsFineWithScOpt(); return; }
		else { CalcSpecOnIntervalsFineFixedSc(); return; }
	}

	void CalcSpecOnIntervalsRoughWithScOpt(void)
	{

		double LW = (abs(SSParams[nSSParams]) + LB) / 2, tmp = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, CurrFreq = 0;
		double sqLW = LW * LW;

		tmp2 = 25 * LW;
		for (int i = 1; i <= nPointsRated; i++)
		{
			CurrFreq = FreqsOnIntervals[i];
			tmp1 = 0;
			for (int j = 1; j <= nFreqsFiltered; j++)
			{
				tmp = abs(CurrFreq - FreqsFiltered[j]);
				if (tmp < tmp2)
				{
					tmp *= tmp;
					tmp1 += IntensFiltered[j] / (tmp + sqLW);
				}
			}
			TheorSpecPointsOnIntervals[i] = tmp1;
			tmp3 += tmp1 * tmp1;
			tmp4 += ExpSpecPointsOnIntervals[i] * tmp1;
		}

		// Rescale TheorSpec on intervals
		tmp = tmp4 / tmp3;
		for (int i = 1; i <= nPointsRated; i++)
			TheorSpecPointsOnIntervals[i] *= tmp;

	}

	void CalcSpecOnIntervalsRoughFixedSc(void)
	{

		double LW = (abs(SSParams[nSSParams]) + LB) / 2, tmp = 0, tmp1 = 0, tmp2 = 0, CurrFreq = 0;
		double sqLW = LW * LW;
		double MaxIntense = 0;

		tmp2 = 25 * LW;
		for (int i = 1; i <= nPointsRated; i++)
		{
			CurrFreq = FreqsOnIntervals[i];
			tmp1 = 0;
			for (int j = 1; j <= nFreqsFiltered; j++)
			{
				tmp = abs(CurrFreq - FreqsFiltered[j]);
				if (tmp < tmp2)
				{
					tmp *= tmp;
					tmp1 += IntensFiltered[j] / (tmp + sqLW);
				}
			}
			TheorSpecPointsOnIntervals[i] = tmp1;
			if (MaxIntense < tmp1) MaxIntense = tmp1;
		}

		// Rescale TheorSpec on intervals
		tmp = abs(TheoreticalSpec.Magnitude / MaxIntense);
		for (int i = 1; i <= nPointsRated; i++)
			TheorSpecPointsOnIntervals[i] *= tmp;

	}

	void CalcSpecOnIntervalsFineWithScOpt(void)
	{

		double LW = (abs(SSParams[nSSParams]) + LB) / 2, tmp = 0, tmp1 = 0, tmp2 = 0, tmp3 = 0, CurrFreq = 0;
		double sqLW = LW * LW;

		for (int i = 1; i <= nPointsRated; i++)
		{
			CurrFreq = FreqsOnIntervals[i];
			tmp1 = 0;
			for (int j = 1; j <= nFreqsFiltered; j++)
			{
				tmp = CurrFreq - FreqsFiltered[j]; tmp *= tmp;
				tmp1 += IntensFiltered[j] / (tmp + sqLW);
			}
			TheorSpecPointsOnIntervals[i] = tmp1;
			tmp2 += tmp1 * tmp1;
			tmp3 += ExpSpecPointsOnIntervals[i] * tmp1;
		}

		// Rescale TheorSpec on intervals
		tmp = tmp3 / tmp2;
		for (int i = 1; i <= nPointsRated; i++)
			TheorSpecPointsOnIntervals[i] *= tmp;

	}

	void CalcSpecOnIntervalsFineFixedSc(void)
	{

		double LW = (abs(SSParams[nSSParams]) + LB) / 2, tmp = 0, tmp1 = 0, CurrFreq = 0;
		double sqLW = LW * LW;
		double MaxIntense = 0;

			for (int i = 1; i <= nPointsRated; i++)
			{
				CurrFreq = FreqsOnIntervals[i];
				tmp1 = 0;
				for (int j = 1; j <= nFreqsFiltered; j++)
				{
					tmp = CurrFreq - FreqsFiltered[j]; tmp *= tmp;
					tmp1 += IntensFiltered[j] / (tmp + sqLW);
				}
				TheorSpecPointsOnIntervals[i] = tmp1;
				if (MaxIntense < tmp1) MaxIntense = tmp1;
			}

		// Rescale TheorSpec on intervals
		tmp = abs(TheoreticalSpec.Magnitude / MaxIntense);
		for (int i = 1; i <= nPointsRated; i++)
			TheorSpecPointsOnIntervals[i] *= tmp;

	}

	void UpdateTheorSpecMagnitude(void)
	{

		double MaxIntense = 0;
		for (int i = 1; i <= nPointsRated; i++)
			if (MaxIntense < TheorSpecPointsOnIntervals[i]) MaxIntense = TheorSpecPointsOnIntervals[i];
		TheoreticalSpec.Magnitude = MaxIntense;

	}

	double Badness(void)
	{
		CalcSpecOnIntervals();
		return SumOfSquareDeviationsOnIntervals();
	}

	double SumOfSquareDeviationsOnIntervals(void)
	{
		double tmp = 0, res = 0;

		for (int i = 1; i <= nPointsRated; i++)
		{
			tmp = ExpSpecPointsOnIntervals[i] - TheorSpecPointsOnIntervals[i]; tmp *= tmp;
			res += tmp;
		}
		return res;
	}

	void CalcFullSpectrum(void)
	{

		double LW = (abs(SSParams[nSSParams]) + LB) / 2;
		double tmp = 0, tmp1 = 0, sqLW = LW * LW, CurrFreq = 0;

		for (int i = 1; i <= TheoreticalSpec.nPoints; i++)
		{
			CurrFreq = Offset - FreqStep * ((double)i - 1);
			tmp1 = 0;
			for (int j = 1; j <= nFreqsFiltered; j++)
			{
				tmp = CurrFreq - FreqsFiltered[j]; tmp *= tmp;
				tmp1 += IntensFiltered[j] / (tmp + sqLW);
			}
			TheoreticalSpec.Points[i] = tmp1;
		}

		// Rescaling of the full spectrum taking into account spectral intervals.
		int index = 0;
		tmp = 0;
		for (int i = 1; i <= nIntervals; i++)
			for (int j = StartPoint[i]; j <= EndPoint[i]; j++)
				if (tmp < TheoreticalSpec.Points[j]) { tmp = TheoreticalSpec.Points[j]; index = j; }
		TheoreticalSpec.Rescale(index);

	}

	double CalcRFactor(void)
	{

		double rfactor = 1;
		if (SumOfExpSquaresOnIntervals > 0)
			rfactor = SumOfSquareDeviationsOnIntervals() / SumOfExpSquaresOnIntervals;
		return(100 * sqrt(rfactor));

	}

	double CalcSpectraColleration(void)
	{

		if (SumOfExpSquaresOnIntervals > 0)
		{
			double SumOfTheorSquaresOnIntervals = 0;
			double tmp = 0;
			for (int i = 1; i <= nPointsRated; i++)
			{
				SumOfTheorSquaresOnIntervals += TheorSpecPointsOnIntervals[i] * TheorSpecPointsOnIntervals[i];
				tmp += TheorSpecPointsOnIntervals[i] * ExpSpecPointsOnIntervals[i];
			}
			return tmp / sqrt(SumOfTheorSquaresOnIntervals * SumOfExpSquaresOnIntervals);
		}

		return 0;

	}

	void SaveSpecsOnIntervalsTXT(void)
	{

		if (SpectraTextOutputFilename[0] == '-') return;

		ofstream ostr(SpectraTextOutputFilename);
		ostr << setw(16) << left << "Freq(Hz)";
		ostr << setw(14) << right << "Exp.Intens.";
		ostr << setw(16) << right << "Theor.Intens.";
		ostr << setw(13) << right << "Diff." << endl;

		ostr.precision(defaultprecision);
		ostr << fixed;
		for (int i = 1; i <= nPointsRated; i++)
		{
			ostr << setw(11) << left << FreqsOnIntervals[i];
			ostr << setw(16) << right << int(ExpSpecPointsOnIntervals[i]);
			ostr << setw(16) << right << int(TheorSpecPointsOnIntervals[i]);
			ostr << setw(16) << right << int(ExpSpecPointsOnIntervals[i]) - int(TheorSpecPointsOnIntervals[i]) << endl;
		}

		ostr.close();

	}

};

class GenericHamiltonian
{
public:
	virtual void ComputeFreqIntens(void) = 0;
};

class Hamiltonian : public GenericHamiltonian
{
public:
	int nFunc;
	int nBlocks;
	BF_t** bFunc;
	int* BlockSize;
	int MaxBlockSize;
	int nFreqs;
	double* Freqs;
	double* Intens;
	gsl_matrix** Ham;
	gsl_matrix** EVec;
	gsl_vector** EVal;
	gsl_eigen_symmv_workspace** W;
	int*** OffDiagJ;
	bool*** Perturbation;
	int* SpinsIz;
	double* perturb;

	Hamiltonian(void)
	{

		nFunc = (unsigned int)1 << (unsigned int)nSpins;
		nBlocks = nSpins + 1;

		BlockSize = new int[(uint_t)nBlocks + 1];
		for (int i = 0; i <= nBlocks; i++)
			BlockSize[i] = 0;
		for (int i = 0; i <= nFunc - 1; i++)
			BlockSize[getbitsum(i) + 1]++;

		MaxBlockSize = 0;
		for (int i = 1; i <= nBlocks; i++)
			if (MaxBlockSize < BlockSize[i])
				MaxBlockSize = BlockSize[i];

		bFunc = new BF_t*[(uint_t)nBlocks + 1]; bFunc[0] = NULL;
		for (int i = 1; i <= nBlocks; i++)
		{
			int tmp = 1;
			bFunc[i] = new BF_t[(uint_t)BlockSize[i] + 1];
			bFunc[i][0] = 0;
			for (BF_t j = 0; j <= (BF_t)nFunc - 1; j++)
				if (getbitsum(j) + 1 == i) { bFunc[i][tmp] = j; tmp++; }
		}

		SpinsIz = new int[(uint_t)nSpins + 1];
		for (int i = 0; i <= nSpins; i++) SpinsIz[i] = false;

		nFreqs = 0;
		nFreqsFiltered = 0;
		for (int i = 1; i <= nBlocks - 1; i++)
			nFreqs += BlockSize[i] * BlockSize[i + 1];
		Freqs = new double[(uint_t)nFreqs + 1];
		Intens = new double[(uint_t)nFreqs + 1];
		FreqsFiltered = new double[(uint_t)nFreqs + 1];
		IntensFiltered = new double[(uint_t)nFreqs + 1];

		for (int i = 0; i <= nFreqs; i++)
		{
			Freqs[i] = 0;
			Intens[i] = 0;
			FreqsFiltered[i] = 0;
			IntensFiltered[i] = 0;
		}

		Ham = new gsl_matrix * [(uint_t)nBlocks + 1];
		EVec = new gsl_matrix * [(uint_t)nBlocks + 1];
		EVal = new gsl_vector * [(uint_t)nBlocks + 1];
		OffDiagJ = new int** [(uint_t)nBlocks + 1];
		W = new gsl_eigen_symmv_workspace * [(uint_t)nBlocks + 1];
		Ham[0] = NULL; EVec[0] = NULL; EVal[0] = NULL; W[0] = NULL;
		OffDiagJ[0] = NULL;
		for (int i = 1; i <= nBlocks; i++)
		{
			Ham[i] = gsl_matrix_alloc(BlockSize[i], BlockSize[i]);
			EVec[i] = gsl_matrix_alloc(BlockSize[i], BlockSize[i]);
			EVal[i] = gsl_vector_alloc(BlockSize[i]);
			if (BlockSize[i] > 1) W[i] = gsl_eigen_symmv_alloc(BlockSize[i]);
			else W[i] = NULL;
			OffDiagJ[i] = new int* [BlockSize[i]];
			for (int j = 0; j < BlockSize[i]; j++)
			{
				OffDiagJ[i][j] = new int[BlockSize[i]];
				for (int k = 0; k < BlockSize[i]; k++)
					OffDiagJ[i][j][k] = 0;
			}
		}

		for (int i = 1; i <= nBlocks; i++)
			for (int j = 0; j < BlockSize[i]; j++)
				for (int k = 0; k < j; k++)
				{
					BF_t bFdiff = bFunc[i][j + 1] ^ bFunc[i][k + 1];
					if (getbitsum(bFdiff) == 2)
					{
						int tmp = 0; int location[2] = { 0, 0 };
						for (int l = 1; l <= nSpins; l++)
							if (getbit(bFdiff, nSpins - l + 1) == 1)
								location[tmp++] = l;
						OffDiagJ[i][j][k] = JCoups[location[0]][location[1]];
					}
				}

		Perturbation = new bool** [nBlocks];
		Perturbation[0] = NULL;
		for (int i = 1; i <= nBlocks - 1; i++)
		{
			Perturbation[i] = new bool* [(uint_t)BlockSize[i + 1] + 1];
			Perturbation[i][0] = NULL;
			for (int j = 1; j <= BlockSize[i + 1]; j++)
			{
				Perturbation[i][j] = new bool[(uint_t)BlockSize[i] + 1];
				Perturbation[i][j][0] = false;
			}
		}

		for (int i = 1; i <= nBlocks - 1; i++)
			for (int j = 1; j <= BlockSize[i + 1]; j++)
			{
				bool* Perturbij = Perturbation[i][j];
				BF_t* bFi = bFunc[i];
				BF_t bFippj = bFunc[i + 1][j];
				for (int k = 1; k <= BlockSize[i]; k++)
					Perturbij[k] = getbitsum(bFippj ^ bFi[k]) == 1;
			}

		perturb = new double[(uint_t)MaxBlockSize + 1];
		for (int i = 0; i <= MaxBlockSize; i++)
			perturb[i] = 0;

	}

	void Build(void)
	{
		for (int i = 1; i <= nBlocks; i++)
		{
			int bs = BlockSize[i];
			BF_t* bFi = bFunc[i];
			gsl_matrix* Hami = Ham[i];
			int** OffDiagJi = OffDiagJ[i];

			for (int j = 0; j < bs; j++)
			{
				int bsj = bs * j;

				BF_t bFij = bFi[j + 1];
				BF_t tmp = (BF_t)1 << (nSpins - 1);
				for (int k = 1; k <= nSpins; k++)
					{ SpinsIz[k] = bFij & tmp ? -1 : 1; tmp /= 2; }

				double element = 0;
				for (int k = 1; k <= nSpins; k++)
				{
					int signk = SpinsIz[k];
					element += SSParams[Offs[k]] * signk;
					for (int l = k + 1; l <= nSpins; l++)
						element += 0.5 * signk * SpinsIz[l] * SSParams[JCoups[k][l]];
				}
				Hami->data[bsj + j] = element;

				int* OffDiagJij = OffDiagJi[j];
				for (int k = 0; k < j; k++)
				{
					int tmp = OffDiagJij[k];
					Hami->data[bsj + k] = tmp ? SSParams[tmp] : 0;
				}
			}
		}
	}

	void FindEigensystem(void)
	{

		for (int i = 1; i <= nBlocks; i++)
			if (BlockSize[i] > 1)
				gsl_eigen_symmv(Ham[i], EVal[i], EVec[i], W[i]);
			else
			{
				EVal[i]->data[0] = Ham[i]->data[0];
				EVec[i]->data[0] = 1;
			}

	}

	void CalcFreqIntens(void)
	{

		for (int i = 1; i <= nBlocks; i++) // EVec matrices transpose
		{
			int bs = BlockSize[i];
			if (bs > 1)
			{
				double* matrix = EVec[i]->data;
				for(int j = 0; j < bs; j++)
					for(int k = j + 1; k < bs; k++)
						{
							double* a = matrix + j + (uint_t)k * bs;
							double* b = matrix + (uint_t)j * bs + k;
							double tmp = *a;
							*a = *b;
							*b = tmp;
						}
			}
		}

		double threshold = 0;
		int freqnum = 0;
		nFreqsFiltered = 0;
		for (int i = 1; i <= nBlocks - 1; i++)
		{
			double* EVecCur = EVec[i]->data;
			double* EVecNext = EVec[i + 1]->data;
			double* EValCur = EVal[i]->data;
			double* EValNext = EVal[i + 1]->data;
			int bs = BlockSize[i];
			int bs_next = BlockSize[i + 1];
			bool** Perturbi = Perturbation[i];

			for (int j = 0; j < bs; j++)
			{

				int tmp = bs * j;
				for (int k = 1; k <= bs_next; k++)
				{
					bool* Perturbik = Perturbi[k];
					double *perturbk = perturb + k; *perturbk = 0;
					for (int l = 1; l <= bs; l++)
						if (Perturbik[l])
							*perturbk += EVecCur[tmp + l - 1];
				}

				double EValCurJ = EValCur[j];
				for (int k = 0; k < bs_next; k++)
				{
					tmp = bs_next * k - 1;
					double intensity = 0;
					for (int l = 1; l <= bs_next; l++)
						intensity += perturb[l] * EVecNext[tmp + l];
					if(abs(intensity) > 1.0e-5)
					{
						freqnum++;
						intensity *= intensity;
						if (threshold < intensity) threshold = intensity;
						Intens[freqnum] = intensity;
						Freqs[freqnum] = EValCurJ - EValNext[k];
					}
				}
			}
		}

		threshold *= IntensityThreshold;
		int tmp = 0;
		for (int i = 1; i <= freqnum; i++)
		{
			double intensity = Intens[i];
			if (intensity > threshold)
			{
				tmp++;
				IntensFiltered[tmp] = intensity;
				FreqsFiltered[tmp] = 0.5 * Freqs[i];
			}
		}
		nFreqsFiltered = tmp;

	}

	void ComputeFreqIntens(void)
	{

		Build();
		FindEigensystem();
		CalcFreqIntens();

	}

};

Spectrum* Spec = NULL;
GenericHamiltonian* Hami = NULL;

double GlobalBadnessLW(double* x)
{
	SSParams[nSSParams] = x[1];
	return Spec->Badness();
}

class OptHamiltonian
{
public:
	//int nParams; => Global nSSParams
	int nVarParams;
	int* VarParamsIndx;
	double* VarParams;
	double* VarParamsUB;
	double* VarParamsLB;
	//int nBroadenings = 0; => Global nBroadenings
	//double* LBs; => Global LBs
	char* InputParameters;
	char* OutputParameters;
	//double* Parameters; => Global SSParams
	double* ParameterErrors;
	double** ParameterCorrelations;
	char** ParNames;
	bool ErrorsComputed;
	bool LWPreOpt;
	double* WS;

	OptHamiltonian(ifstream& istr)
	{

		nVarParams = 0;
		LWPreOpt = false;
		ErrorsComputed = false;

		// All Spin Systems parameters including Magnitude - nSSParams + 1
		ParNames = new char* [(uint_t)nSSParams + 2];
		ParNames[0] = NULL;
		ParameterErrors = new double[(uint_t)nSSParams + 2]; // + Magnitude
		ParameterCorrelations = new double* [(uint_t)nSSParams + 2]; // + Magnitude
		ParameterErrors[0] = 0; ParameterCorrelations[0] = NULL;
		VarParamsIndx = new int[(uint_t)nSSParams + 2]; VarParamsIndx[0] = 0;

		for (int i = 1; i <= nSSParams + 1; i++)
		{
			ParNames[i] = new char[256];
			ParameterErrors[i] = 0;
			ParameterCorrelations[i] = new double[(uint_t)nSSParams + 2];
			for (int j = 0; j <= nSSParams + 1; j++)
				ParameterCorrelations[i][j] = 0;
			strcpy(ParNames[i], "");
			VarParamsIndx[i] = 0;
		}

		VarParams = new double[(uint_t)nSSParams + 1];
		VarParamsUB = new double[(uint_t)nSSParams + 1];
		VarParamsLB = new double[(uint_t)nSSParams + 1];
		for (int i = 0; i <= nSSParams; i++)
		{
			VarParams[i] = 0;
			VarParamsUB[i] = 0;
			VarParamsLB[i] = 0;
		}

		InputParameters = new char[256];
		OutputParameters = new char[256];
		istr.getline(textline, 256);                         // Optimization parameters
		istr >> textline >> InputParameters;                 // Input parameters filename
		istr >> textline >> OutputParameters;                // Output parameters filename
		istr >> textline >> Spec->SpectraTextOutputFilename; // Filename for spectra in ASCII text format
		istr.getline(textline, 256);                         // Rest of SpectraTextOutputFilename line

		// Parsing and loading of Broadenings sequence
		char textline1[256];
		stringstream parse;
		istr.getline(textline, 256);   //  LBs
		parse << textline;
		while (parse) { parse >> textline1; nBroadenings++; }
		nBroadenings -= 2;
		if (nBroadenings < 1) { cout << "Wrong number of broadenings (" << nBroadenings << "), should be at least 1." << endl; exit_; }
		LBs = new double[(uint_t)nBroadenings + 1];
		LBs[0] = 0;
		parse.clear(); parse << textline;
		parse >> textline;

		double MaxLb = 0;
		for (int i = 1; i <= nBroadenings; i++)
		{
			parse >> textline;
			if (!isunsignreal(textline)) { cout << "Wrong value of broadening number " << i << "." << endl; exit_; }
			LBs[i] = atof(textline);
			if(MaxLb < LBs[i]) MaxLb = LBs[i];
		}

		Spec->InitBroadening(MaxLb);

		CheckBroadSequence(cout);

		LoadParameters();

		Spec->CheckSpinOffsets(cout);
		Spec->fixSpinOffsets(cout);

		// Magnitude from exp. spectrum
		bool tmp;
		istr >> textline >> tmp; // Magnitude from exp. spectrum
		if (istr.fail()) { cout << "Wrong MagnitudeFromExpSpec flag value, sould be 0 or 1." << endl; exit_; }
		if (tmp) Spec->TheoreticalSpec.Magnitude = Spec->ExperimentalSpec.Magnitude;

	}

	void CheckBroadSequence(ostream& ostr)
	{

		bool printwarn = false;
		for (int i = 2; i <= nBroadenings; i++)
			if (LBs[i] >= LBs[i - 1]) printwarn = true;
		if (printwarn) ostr << "Warning! The sequence of additional broadenings should monotonically decrease!" << endl << endl;

	}

	void LoadParameters(void)
	{

		int ParNum = 0;
		ifstream istr(InputParameters);
		if (!istr) { cout << "File " << InputParameters << " does not exists!" << endl; exit_; }

		for (int i = 1; i <= nSSParams - 1; i++)
		{
			istr >> textline;
			if (!isunsignint(textline)) { cout << "Incorrect number of parameter " << i << "." << endl; exit_; }
			ParNum = atoi(textline);
			if (ParNum != i) { cout << "Incorrect number of parameter " << i << "." << endl; exit_; }
			istr >> ParNames[i];
			istr >> textline;
			if (!isreal(textline)) { cout << "Incorrect value of parameter " << i << "." << endl; exit_; }
			SSParams[i] = atof(textline);
			istr.getline(textline, 256);
		}

		istr >> textline;
		if (!isunsignint(textline)) { cout << "Incorrect number of parameter " << nSSParams << " (linewidth)." << endl; exit_; }
		ParNum = atoi(textline);
		if (ParNum != nSSParams) { cout << "Incorrect number of parameter " << nSSParams << " (linewidth)." << endl; exit_; }
		istr >> ParNames[nSSParams];
		istr >> textline;
		if (!isunsignreal(textline)) { cout << "Incorrect value of parameter " << nSSParams << " (linewidth)." << endl; exit_; }
		SSParams[nSSParams] = atof(textline);
		if (SSParams[nSSParams] == 0) { cout << "Incorrect value of parameter " << nSSParams << " (linewidth)." << endl; exit_; }
		istr.getline(textline, 256);

		istr >> textline;
		if (!isunsignint(textline)) { cout << "Incorrect number of parameter " << nSSParams + 1 << " (spectrum magnitude)." << endl; exit_; }
		ParNum = atoi(textline);
		if (ParNum != nSSParams + 1) { cout << "Incorrect number of parameter " << nSSParams + 1 << " (spectrum magnitude)." << endl; exit_; }
		istr >> ParNames[nSSParams + 1];

		istr >> Spec->TheoreticalSpec.Magnitude;
		if (istr.fail() || Spec->TheoreticalSpec.Magnitude <= 0) { cout << "Incorrect value of parameter " << nSSParams + 1 << " (spectrum magnitude)." << endl; exit_; }

		istr.close();

	}

	void LoadVarParameters(ifstream& istr)
	{

		istr.getline(textline, 256); // Rest of MagnitudeFromExpSpec line
		istr.getline(textline, 256); // Empty line
		if (!isemptyline(textline)) { cout << "Empty line should follow the section with optimization parameters!" << endl; exit_; }
		istr.getline(textline, 256); // List of optimized parameters

		istr >> textline;
		while (istr && nVarParams < nSSParams + 1)
		{
			if (!isunsignint(textline)) { cout << "Wrong index of varied parameter " << nVarParams << "." << endl; exit_; }
			VarParamsIndx[++nVarParams] = atoi(textline);
			if (VarParamsIndx[nVarParams] > nSSParams + 1) { cout << "Wrong index of varied parameter " << nVarParams << "." << endl; exit_; }
			istr >> textline;
		}

		if (nVarParams == 0) { cout << "Nothing to optimize!" << endl; exit_; }

		for (int i = 2; i <= nVarParams; i++)
			if (VarParamsIndx[i] <= VarParamsIndx[i - 1]) { cout << "Wrong index of varied parameter " << i << "." << endl; exit_; }

		if (VarParamsIndx[nVarParams] == nSSParams + 1) { Spec->ScaleOpt = true; nVarParams--; } // By default Spec->ScaleOpt is false.

		if (nVarParams == 0) { cout << "Nothing to optimize!" << endl; exit_; }

		LWPreOpt = VarParamsIndx[nVarParams] == nSSParams && nVarParams > 1;

		for (int i = 1; i <= nVarParams; i++) VarParams[i] = SSParams[VarParamsIndx[i]];

		WS = new double[(uint_t)(5 * nVarParams * (3 * nVarParams + 11) / 2 + 6)];

	}

	double Badness(double* VarPars)
	{

		for (int i = 1; i <= nVarParams; i++)
			SSParams[VarParamsIndx[i]] = VarPars[i];
		Hami->ComputeFreqIntens();
		return Spec->Badness();

	}

	double Badness(void)
	{

		for (int i = 1; i <= nVarParams; i++)
			SSParams[VarParamsIndx[i]] = VarParams[i];
		Hami->ComputeFreqIntens();
		return Spec->Badness();

	}

	void PowellOpt(void);

	void UpdateParamsAfterOpt(void)
	{
		for (int i = 1; i <= nVarParams; i++) SSParams[VarParamsIndx[i]] = VarParams[i];
	}

	void ComputeErrors(void)
	{

		ErrorsComputed = false;

		double** TheorSpecDerivativesOnIntervals = new double* [(uint_t)nSSParams + 2];
		TheorSpecDerivativesOnIntervals[0] = NULL;
		int nPoints = Spec->nPointsRated;
		for (int i = 1; i <= nSSParams + 1; i++)
		{
			TheorSpecDerivativesOnIntervals[i] = new double[(uint_t)nPoints + 1];
			for (int j = 0; j <= nPoints; j++)
				TheorSpecDerivativesOnIntervals[i][j] = 0;
		}

		double* Data = new double[(uint_t)nPoints + 1];
		Data[0] = 0;

		double Badn = 0;
		for (int i = 1; i <= nPoints; i++)
		{
			double tmp = Spec->TheorSpecPointsOnIntervals[i];
			Badn += (tmp - Spec->ExpSpecPointsOnIntervals[i]) * (tmp - Spec->ExpSpecPointsOnIntervals[i]);
			Data[i] = tmp;
			TheorSpecDerivativesOnIntervals[nSSParams + 1][i] = tmp;
		}

		for (int i = 1; i <= nSSParams; i++)
		{
			double step = 1.0e-7;
			double Par = SSParams[i];
			SSParams[i] += step;
			Hami->ComputeFreqIntens();
			Spec->CalcSpecOnIntervals();
			SSParams[i] = Par;
			for (int j = 1; j <= nPoints; j++)
				TheorSpecDerivativesOnIntervals[i][j] = (Spec->TheorSpecPointsOnIntervals[j] - Data[j]) / step;
		}

		for (int i = 1; i <= nPoints; i++)
			Spec->TheorSpecPointsOnIntervals[i] = Data[i];
		delete[] Data;

		gsl_matrix* DTD = gsl_matrix_alloc((uint_t)nSSParams + 1, (uint_t)nSSParams + 1);

		for (int i = 1; i <= nSSParams + 1; i++)
			for (int j = i; j <= nSSParams + 1; j++)
			{
				double tmp = 0;
				for (int k = 1; k <= nPoints; k++)
					tmp += TheorSpecDerivativesOnIntervals[i][k] * TheorSpecDerivativesOnIntervals[j][k];
				DTD->data[(nSSParams + 1) * (i - 1) + j - 1] = tmp;
			}

		for (int i = 0; i < nSSParams + 1; i++)
			for (int j = i + 1; j < nSSParams + 1; j++)
				DTD->data[(nSSParams + 1) * j + i] = DTD->data[(nSSParams + 1) * i + j];

		for (int i = 1; i <= nSSParams + 1; i++)
			delete[] TheorSpecDerivativesOnIntervals[i];
		delete[] TheorSpecDerivativesOnIntervals;

		gsl_matrix* evec = gsl_matrix_alloc((uint_t)nSSParams + 1, (uint_t)nSSParams + 1);
		gsl_vector* eval = gsl_vector_alloc((uint_t)nSSParams + 1);
		gsl_eigen_symmv_workspace* w = gsl_eigen_symmv_alloc((uint_t)nSSParams + 1);

		gsl_eigen_symmv(DTD, eval, evec, w);
		gsl_eigen_symmv_free(w);

		gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_ABS_ASC);

		if (eval->data[0] > 1e-4)
		{
			gsl_matrix_set_all(DTD, 0);

			for (int i = 0; i < nSSParams + 1; i++)
				for (int j = 0; j < nSSParams + 1; j++)
					for (int k = 0; k < nSSParams + 1; k++)
						DTD->data[(nSSParams + 1) * i + j] += evec->data[(nSSParams + 1) * i + k] * evec->data[(nSSParams + 1) * j + k] / eval->data[k];

			for (int i = 0; i < nSSParams + 1; i++)
			{
				ParameterErrors[i + 1] = sqrt(DTD->data[i * (nSSParams + 1) + i] * Badn / (nPoints - nSSParams - 1));
				for (int j = 0; j <= i; j++)
					ParameterCorrelations[i + 1][j + 1] = DTD->data[i * (nSSParams + 1) + j] / sqrt(DTD->data[i * (nSSParams + 1) + i] * DTD->data[j * (nSSParams + 1) + j]);
			}

			ParameterErrors[nSSParams+1] *= Spec->TheoreticalSpec.Magnitude;

			ErrorsComputed = true;
		}

		gsl_matrix_free(evec);
		gsl_vector_free(eval);
		gsl_matrix_free(DTD);

	} 

std::string openArray(const std::string& input, const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "\"" << input << "\": [";
		if (input3)
			oss << endl;
	    return oss.str();
	}

	std::string closeArray(const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "]";
		if (input3)
			oss << endl;
	    return oss.str();
	}

std::string jsonVar(const std::string& input, const std::string& inputValue, const std::string& input2, const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "\"" << input << "\": ";
		oss << "\"" << inputValue << "\"";
		oss << input2;
		if (input3)
			oss << endl;
	    return oss.str();
	}
	
	std::string jsonVar(const std::string& input, const double inputValue, const std::string& input2, const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "\"" << input << "\": ";
		oss.precision(7);
		oss << fixed;
    	oss << std::showpoint; 
		oss << inputValue;
		oss << input2;
		if (input3)
			oss << endl;
	    return oss.str();
	}
	std::string jsonVarInt(const std::string& input, const int inputValue, const std::string& input2, const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "\"" << input << "\": ";
		oss.precision(7);
		oss << fixed;
    	oss << std::showpoint; 
		oss << inputValue;
		oss << input2;
		if (input3)
			oss << endl;
	    return oss.str();
	}

	std::string nextArrayElement(const bool notLast, const bool input3 = true) {
	    std::ostringstream oss;
	    if (notLast) 
			oss << ",";
		if (input3)
			oss << endl;
	    return oss.str();
	}
	std::string jsonCloseObj (const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "}";
		if (input3)
			oss << endl;
	    return oss.str();
	}
	std::string jsonOpenObj (const bool input3 = true) {
	    std::ostringstream oss;
	    oss << "{";
		if (input3)
			oss << endl;
	    return oss.str();
	}

	void writeCHEMeDATA() {
	int parnamelen = 0;
		char* trueOutputParameters = new char[256];
		
       	strcpy(trueOutputParameters, "CHEMeDATA.json");

		ofstream ostr(trueOutputParameters);

		for (int i = 1; i <= nSSParams + 1; i++)
		{
			int tmp = (int)strlen(ParNames[i]);
			if (parnamelen < tmp) parnamelen = tmp;
		}
		parnamelen += 4;

		if (SSParams[nSSParams] < 0) SSParams[nSSParams] *= -1;

		ostr << "{";
		ostr << openArray("optimizedVariables");
		for (int i = 1; i <= nSSParams + 1; i++) {
			ostr  << jsonOpenObj(false);
			ostr  << jsonVar("name", ParNames[i], ", ", false);

			if (i < nSSParams)
				ostr << openArray("spinSystemIndices", false);
			bool first = true;
			bool isCHemShift = false;
			bool isCoupling = false;
			bool isfactor = i == nSSParams + 1;
			bool islineWidth = i == nSSParams;
			bool iskurtosis = false;

			double dividor = 1.0;// for chemical shift divide by larmor
			for (int spin = 1; spin <= nSpins; spin++) {
				if (Offs[spin] == i) {
					if (first) {
						first = false;
					} else {
						ostr  << ", " ;
					}
					ostr << spin ;
					isCHemShift = true;
					dividor = Spec->SF;
				}
			}
							
						
			first = true;
			for (int i1 = 1; i1 < nSpins; i1++)	{
			for (int j1 = 1; j1 <= nSpins; j1++) {
				if (j1 > i1) {		
					if (JCoups[i1][j1] == i) {
						isCoupling = true;
					if (first) {
						first = false;
					} else {
						ostr  << ", " ;
					}
					ostr << "[" << i1 << ", " << j1 << "]";
					}
				}
			}
			}
			if (i < nSSParams) {
				ostr << closeArray(false);
				ostr << ", ";
			}
				
			if (isCHemShift) {
				ostr  << jsonVar("typeVariableString", "ChemicalShift", ", ", false);
			} 
			if (isCoupling) {
				ostr  << jsonVar("typeVariableString", "Jcoupling", ", ", false);
			}
			if (isfactor) {
				ostr  << jsonVar("typeVariableString", "factor", ", ", false);
			}
			if (islineWidth) {
				ostr  << jsonVar("typeVariableString", "lineWidth", ", ", false);
			}
			if (iskurtosis) {
				ostr  << jsonVar("typeVariableString", "kurtosis", ", ", false);
			}
			if (i < nSSParams + 1) {
				if (ErrorsComputed) ostr  << jsonVar("error", ParameterErrors[i], ", ", false);
				ostr  << jsonVar("value", SSParams[i] / dividor, "", false);
			} else {
				if (ErrorsComputed) ostr  << jsonVar("error", ParameterErrors[i], ", ", false);
				ostr  << jsonVar("value", Spec->TheoreticalSpec.Magnitude, "", false);
			}
			ostr  << jsonCloseObj(false);
			ostr << nextArrayElement(i < nSSParams + 1);
		}
		ostr << closeArray(false);
		ostr << "," << endl;

		if (!ErrorsComputed) ostr << jsonVar("ErrorMessage1","Errors are not calculated due to singularity of normal equations matrix!",  ", ");
		ostr << jsonVar("Title", Title ,  ", ");

		ostr << jsonVar("CheckBroadSequence", " no implemented" ,  ", ");
		//CheckBroadSequence(ostr);

		ostr << jsonVar("LineBroadening",  Spec->LB ,  ", ");
		ostr << jsonVar("Theoreticalspectrumlinewidth",  Spec->LB + SSParams[nSSParams]  ,  ", ");
		ostr << jsonVar("RSSValue",  Badness() ,  ", ");
		ostr << jsonVar("R-Factor_Percent",  Spec->CalcRFactor() ,  ", ");
		ostr << jsonVar("SpectraCorrelationCoefficient",  Spec->CalcSpectraColleration() ,  ", ");
		ostr << jsonVar("ParametersCorrelationCoefficients", "not implemented",  "");


		if (ErrorsComputed && false)
		{
			ostr << "Parameters correlation coefficients:" << endl;
			for (int i = 1; i <= nSSParams + 1; i++)
			{
				ostr << setw(3) << right << i << ' ';
				for (int j = 1; j <= i; j++)
					ostr << setw(10) << right << ParameterCorrelations[i][j];
				ostr << endl;

			}
			ostr << endl;
		}

		ostr  << jsonCloseObj();
		ostr.close();
	}

	void SaveParameters(const bool refine = false)
	{

		int parnamelen = 0;
		char* trueOutputParameters = new char[256];
		if (refine) {
        	strcpy(trueOutputParameters, "allCombiTested_");
		}
		strcat(trueOutputParameters, OutputParameters);
		
		ofstream ostr(trueOutputParameters);

		for (int i = 1; i <= nSSParams + 1; i++)
		{
			int tmp = (int)strlen(ParNames[i]);
			if (parnamelen < tmp) parnamelen = tmp;
		}
		parnamelen += 4;

		if (SSParams[nSSParams] < 0) SSParams[nSSParams] *= -1;

		ostr << fixed;
		for (int i = 1; i <= nSSParams; i++)
		{
			ostr << setw(4) << left << i << setw(parnamelen) << left << ParNames[i];
			ostr << setw(13) << right << SSParams[i];
			if (ErrorsComputed) ostr << setw(4) << ' ' << "+-" << ParameterErrors[i];
			ostr << endl;
		}

		ostr << scientific;
		ostr << setw(4) << left << nSSParams + 1 << setw(parnamelen) << left << ParNames[nSSParams + 1];
		ostr << setw(13) << right << Spec->TheoreticalSpec.Magnitude;
		if (ErrorsComputed) ostr << setw(4) << ' ' << "+-" << ParameterErrors[nSSParams + 1];
		ostr << fixed << endl;

		if (!ErrorsComputed) ostr << "Errors are not calculated due to singularity of normal equations matrix!" << endl;
		ostr << endl;

		ostr << Title << endl << endl;
		Spec->CheckSpinOffsets(ostr);
		CheckBroadSequence(ostr);

		ostr << "Line Broadening: " << fixed << Spec->LB << endl;
		ostr << "Theoretical spectrum linewidth: " << Spec->LB + SSParams[nSSParams] << endl;
		ostr << "RSS Value: " << scientific << Badness() << endl;
		ostr.precision(2);
		ostr << "R-Factor: " << fixed << Spec->CalcRFactor() << " %" << endl;
		ostr.precision(defaultprecision);
		ostr << "Spectra correlation coefficient: " << Spec->CalcSpectraColleration() << endl;
		ostr << endl;

		// Print Spin System
		ostr.precision(3);
		ostr << "Chemical shifts (ppm):" << endl;
		for (int i = 1; i <= nSpins; i++)
			ostr << setw(10) << right << SSParams[Offs[i]] / Spec->SF;
		ostr << endl;
		ostr.precision(4);
		ostr << "J-coupling constants (Hz):" << endl;
		for (int i = 1; i < nSpins; i++)
		{
			for (int j = 1; j <= nSpins; j++)
				if (j <= i) ostr << setw(10) << ' ';
				else ostr << setw(10) << right << SSParams[JCoups[i][j]];
			ostr << endl;
		}
		ostr.precision(defaultprecision);
		ostr << endl;

		if (ErrorsComputed)
		{
			ostr << "Parameters correlation coefficients:" << endl;
			for (int i = 1; i <= nSSParams + 1; i++)
			{
				ostr << setw(3) << right << i << ' ';
				for (int j = 1; j <= i; j++)
					ostr << setw(10) << right << ParameterCorrelations[i][j];
				ostr << endl;

			}
			ostr << endl;
		}

		print_citation(ostr);
		ostr.close();

	}

};

OptHamiltonian* HamOpt = NULL;

double GlobalBadness(double* x)
{
	return HamOpt->Badness((double*)x);
}

void OptHamiltonian :: PowellOpt(void)
{

	for (int i = 1; i <= nVarParams; i++)
	{
		VarParamsUB[i] = VarParams[i] + 100;
		VarParamsLB[i] = VarParams[i] - 100;
	}

	if (LWPreOpt)
	{
		cout << "Spectrum linewidth preoptimization" << endl;

		Hami->ComputeFreqIntens();

		int offset = nVarParams - 1;
		//bobyqa(1, 3, GlobalBadnessLW, VarParams + offset, VarParamsLB + offset, VarParamsUB + offset, 10.0, 1e-10, WS);

		VarParamsLB[nVarParams] = VarParams[nVarParams] - 100;
		VarParamsUB[nVarParams] = VarParams[nVarParams] + 100;

		cout << "Main optimization" << endl;
	}

	//bobyqa(nVarParams, 2 * nVarParams + 1, GlobalBadness, VarParams, VarParamsLB, VarParamsUB, 10.0, 1e-10, WS);

}




void LoadSpinSystem(ifstream& istr)
{

	// Reading SpinSystem => nSpins, Offs, JCoups
	istr.getline(textline, 256); // Spin System
	istr >> textline >> textline;
	if (!isunsignint(textline)) { cout << "Check the number of spins (Nspins)!" << endl; exit_; }
	nSpins = atoi(textline);
	if (nSpins > 8 * (int)sizeof(unsigned int)) { cout << "Number of spins (Nspins) exceeds the maximum value (" << 8 * sizeof(unsigned int) << ")!" << endl; exit_; }
	istr.getline(textline, 256);
	istr.getline(textline, 256); // Shifts indices

	Offs = new int[(uint_t)nSpins + 1]; Offs[0] = 0;
	JCoups = new int* [(uint_t)nSpins + 1]; JCoups[0] = NULL;

	for (int i = 1; i <= nSpins; i++)
	{
		Offs[i] = 0;
		JCoups[i] = new int[(uint_t)nSpins + 1];
		for (int j = 0; j <= nSpins; j++)
			JCoups[i][j] = 0;
	}

	// Spin offset indices reading
	for (int i = 1; i <= nSpins; i++)
	{
		istr >> textline;
		if (!isunsignint(textline)) { cout << "Wrong input of spin " << i << " chemical shift index." << endl; exit_; }
		Offs[i] = atoi(textline);
	}

	istr.getline(textline, 256); // Rest of the line
	istr.getline(textline, 256); // Coupling Indices

	// Spin offset indices checking
	int tmp = 1;
	if (Offs[1] != 1) { cout << "Spin 1 should have chemical shift index 1." << endl; exit_; }
	for (int i = 2; i <= nSpins; i++)
	{
		if (Offs[i] < tmp) { cout << "Wrong input of spin " << i << " chemical shift index." << endl; exit_; }
		if (Offs[i] > tmp + 1) { cout << "Wrong input of spin " << i << " chemical shift index." << endl; exit_; }
		tmp = Offs[i];
	}
	tmp++;

	// Coupling constant indices reading
	for (int i = 1; i <= nSpins; i++)
		for (int j = i + 1; j <= nSpins; j++)
		{
			istr >> textline;
			if (!isunsignint(textline)) { cout << "Wrong index for J-coupling constant " << i << "," << j << "." << endl; exit_; }
			JCoups[i][j] = atoi(textline);
		}
	istr.getline(textline, 256); // Rest of line
	istr.getline(textline, 256); // Empty line
	if (!isemptyline(textline)) { cout << "Empty line should follow the section with spin system description!" << endl; exit_; }

	// Coupling constant indices checking
	if (JCoups[1][2] != tmp) { cout << "J-coupling constant 1,2 should have index " << tmp << " instead of " << JCoups[1][2] << "." << endl; exit_; }
	for (int i = 1; i <= nSpins; i++)
		for (int j = i + 1; j <= nSpins; j++)
		{
			if (JCoups[i][j] <= Offs[nSpins]) { cout << "Wrong index for J-coupling constant " << i << "," << j << "." << endl; exit_; }
			if (JCoups[i][j] > tmp + 1) { cout << "Wrong index for J-coupling constant " << i << "," << j << "." << endl; exit_; }
			if (JCoups[i][j] > tmp) tmp = JCoups[i][j];
		}

	nSSParams = tmp + 1;

	SSParams = new double[(uint_t)nSSParams + 1];
	for (int i = 0; i <= nSSParams; i++)
		SSParams[i] = 0;

}

int main(int argc, char* argv[])
{

// If running under Windows Platform
#ifdef _WIN32
	char* env = NULL;
	if (getenv("XWINNMRHOME") != NULL) ExitWithPause = false; // Running under TopSpin
	if ((env = getenv("ExitWithoutPause")) != NULL)           // Checking ExitWithoutPause enviroment variable
		if (strcmp(env, "1") == 0) ExitWithPause = false;
#endif

	if (argc == 2)
	{
		strcpy(textline, argv[1]);
		for (int i = 0; int(textline[i]) != 0; i++) if (textline[i] == '\\') textline[i] = '/';
		int n = (int)strlen(textline);
		if ((textline[n - 1] != '/') && (n < 255)) { textline[n] = '/'; textline[n + 1] = 0; }
		if (chdir(textline) != 0) { cout << "Failed to change working directory!" << endl; exit_; }
	}
	if (argc > 2) { cout << "Wrong command line argument!" << endl; exit_; }

	bool SimMode = true;

	// Parsing input control file and initialize relevant data structures.
	ifstream input("Input_Data.txt");
	if (!input) { cout << "File Input_Data.txt does not exists!" << endl; exit_; }
	input.getline(Title, 256);    // Title
	input.getline(textline, 256); // Empty line
	if (!isemptyline(textline)) { cout << "Empty line should follow the title!" << endl; exit_; }
	input >> textline >> SimMode; // Sim mode
	if (input.fail()) { cout << "Wrong simulation mode value, sould be 0 or 1." << endl; exit_; }
	input.getline(textline, 256); // Rest of sim mode line
	input.getline(textline, 256); // Empty line
	if (!isemptyline(textline)) { cout << "Empty line should follow the SimMode line!" << endl; exit_; }

	LoadSpinSystem(input);

	Spec = new Spectrum(input);

	HamOpt = new OptHamiltonian(input);

	Hami = new Hamiltonian();

	if (SimMode)
	{
		Spec->BroadOnIntervals(0.0);
		Spec->ExperimentalSpecWithBroadening.SaveSpecToFile(1); // does not produce a spectrum
		Hami->ComputeFreqIntens();
		Spec->CalcSpecOnIntervals();
		Spec->SaveSpecsOnIntervalsTXT();
		Spec->LB = 0;
		Spec->CalcFullSpectrum();
		Spec->TheoreticalSpec.SaveSpecToFile();
		cout << setprecision(2) << fixed
		<< "UUUU R-Factor: " << Spec->CalcRFactor() << " %" << endl
		<< setprecision(defaultprecision);
		print_citation(cout);
		input.close();
		exit_;
	}


	HamOpt->LoadVarParameters(input);

	input.close(); // End of INPUT file parsing

	Spec->FineCalc = false;
	for (int i = 1; i <= nBroadenings; i++)
	{

		if (i == nBroadenings) Spec->FineCalc = true;

		cout << "Broadening:  " << LBs[i] << endl;

		Spec->BroadOnIntervals(LBs[i]);

		HamOpt->PowellOpt();

		cout << endl;

	}

	HamOpt->UpdateParamsAfterOpt();
	Hami->ComputeFreqIntens();
	Spec->CalcSpecOnIntervals();
	if (Spec->ScaleOpt)
	{
		Spec->UpdateTheorSpecMagnitude();
		Spec->ScaleOpt = false;
	}

	cout << setprecision(2) << fixed
		<< "ZZ R-Factor: " << Spec->CalcRFactor() << " %" << endl
		<< setprecision(defaultprecision);

	Spec->SaveSpecsOnIntervalsTXT();
	Spec->CalcFullSpectrum();
	Spec->TheoreticalSpec.SaveSpecToFile();
	Spec->ExperimentalSpecWithBroadening.SaveSpecToFile();
	HamOpt->ComputeErrors();
	HamOpt->SaveParameters();
	HamOpt->writeCHEMeDATA();
	cout << endl;

	print_citation(cout);
	exit_;


}

