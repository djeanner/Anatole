# What Anatolia does


## What is extracted from files

|File|Input|Content|variable|
|-|-|-|-|
|acqno/acqus| BF1 |Larmor frequency|`BF`|
|acqno/acqus| O1 |  |`O1_h`|
|acqno/acqus| SW_h or SW_H| Spectral width in Hz|`SW_h`|
|acqno/pdata/procno/procs| SF ||`SF`|
|acqno/pdata/procno/procs| SI |Number of points of spectra|`TheoreticalSpec`.`nPoints`|
|acqno/pdata/procno/integrals.txt| Integrated Region|Relevant part of spectra  | `StartPoint`[0 .. `nIntervals` - 1]  `EndPoint`[0 .. `nIntervals` - 1]|
|acqno/pdata/procno/1r|Whole file binary |Real part of the NMR spectrum |`ExpSpecPointsOnIntervals`, `TheorSpecPointsOnIntervals`, `FreqsOnIntervals`|
 
The `Integrated Region` are used to produce arrays of StartPoint-EndPoint in pt StartPoint to work only with the part of the spectrum that were integrated. (ignore the rest of the spectrum - this will probably speed up generation of spectra, comparision, etc.) The following are using compressed spectra: `ExpSpecPointsOnIntervals`, `TheorSpecPointsOnIntervals`, `FreqsOnIntervals`.

It seems Anatolia is ignoring "sr", the Bruker scale adjustment param, i.e. chemical shifts will be slightly wrong if sr was used ? Or are the ranges in integrals.txt corrected??

## Some key variables

Some key lines of the code to see the use of the variables
```
Offset = (BF - SF) * 1e6 + O1_h + 0.5 * SW_h;
FreqStep = SW_h / TheoreticalSpec.nPoints;
double PPMOffset = Offset / SF;
double PPMStep = FreqStep / SF;
StartPoint[i] = int((PPMOffset - StartPPM) / PPMStep) - 1;
if (StartPoint[i] < 1) StartPoint[i] = 1;
if (StartPoint[i] > TheoreticalSpec.nPoints) StartPoint[i] = TheoreticalSpec.nPoints;
EndPoint[i] = int((PPMOffset - EndPPM) / PPMStep) + 1;
FreqsOnIntervals[n++] = Offset - FreqStep * ((double)j - 1);
FreqsOnIntervals[n] = Offset - FreqStep * ((double)j - 1);
```

## Some key code

Code of Lorentzian lineshape generation of theoretical spectrum
```
LW = (abs(SSParams[nSSParams]) + LB) / 2;
sqLW = LW * LW;

for (int i = 1; i <= TheoreticalSpec.nPoints; i++) {
	CurrFreq = Offset - FreqStep * ((double)i - 1);
	tmp1 = 0;
	for (int j = 1; j <= nFreqsFiltered; j++) {
		tmp = CurrFreq - FreqsFiltered[j]; 
		tmp *= tmp;
		tmp1 += IntensFiltered[j] / (tmp + sqLW);
	}
	TheoreticalSpec.Points[i] = tmp1;
}
```
