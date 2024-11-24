# The `Input_data.txt` file

The `Input_data.txt` is the only input of Anatolia. The file includes:

|Name|Comment|typical values||
|-|-|-|-|
|SimMode|Flag for NMR parameter optimization (or 1:single-point calculation (simulation))|0||
|NSpins|Number of spins in the system||Spin System structure|
|.|Parameters of the NMR parameter optimization |see example below|Spin System structure|
|PathToDataSet| A pointer the location of the spectra spectrum |./||
|ExpProcNo |The procno of the experimental spectrum|1|experimental spectrum|
|ExpBroaded| ProcNoThe procno of the simulated spectrum|2|simulated spectrum|
|CalcProcNo |The procno of the broadened spectrum|2||
|InputParameters |Input file of the [starting](../data/Styrene/1/parameters_start.txt) NMR parameters|parameters_start.txt|NMR parameters|
|OutputParameters |Output file of the [refined](../data/Styrene/1/parameters.txt) NMR parameters|parameters.txt|NMR parameters|
|SpectraTextOutput |FileName|spectrum.txt|simulated spectrum|
|LBs |List of decreasing broadening|3 2 1 0.5 0.1 0.0| |
|MagnitudeFromExpSpec |rescale to exp if set to 1|1||
|List of optimized parameters |indices of the param to optimze|1.. max number of variables||


The [starting](../data/Styrene/1/parameters_start.txt) and [refined](../data/Styrene/1/parameters.txt) NMR parameters are  compatible in format, allowing one to use the results of a calculation to start another one.

The real parameter of the optimization are :
|Parameter|Type|Comment|Typical values|
|-|-|-|-|
|SimMode| 1/0|	If 1 will only simulate the spectra (not parameter optimization) |1	|
|LBs|list of double|List of decreasing line widths for the steps of optimization		|3 2 1 0.5 0.1 0.0|
|MagnitudeFromExpSpec|1/0|If 1 rescales according to top point of the experimental spectrum		|1|
|List of optimized parameters|list of integers|list of indices of the parameters that will be optimized|1 2 3 ...|

In indices run over:
- The chemical shifts
- The coupling constants
- Linewidth 
- Magnitude 

Example of `Input_data.txt` file
```
THFCOOH

SimMode	0

Spin System
NSpins 7
Chemical shift indices
1  2  3  4  5  6  7
J-coupling indices
   8  9 10 11 12 13
     14 15 16 17 18
        19 20 21 22
           23 24 25
              26 27
                 28

Spectra parameters
PathToDataSet			./
ExpProcNo			1
ExpBroadedProcNo		2
CalcProcNo			3

Optimization parameters
InputParameters			parameters_2.txt
OutputParameters		parameters.txt
SpectraTextOutput		-
LBs				3 2 1 0.5 0.1 0.0
MagnitudeFromExpSpec		1

List of optimized parameters
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
```

## MagnitudeFromExpSpec

The use of ‘MagnitudeFromExpSpec’ is not very clear. There is a ’Magnitude’ in the NMR parameter files. 

Here is the citation of the paper: 

"The option ‘MagnitudeFromExpSpec’ allows to replace the spectrum magnitude by one determined from the experimental spectrum right after reading parameters from the file. In this case magnitude is determined as maximal intensity on spectral intervals used for optimization. It is active in simulation and optimization modes. ‘MagnitudeFromExpSpec’ can be ‘1’ or ‘0.‘"
