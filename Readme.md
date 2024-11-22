# Versions of Anatolia

We explored and extended (preliminary tests only) the Anatolia software by Cheshkova,  Sheberstova, Sinitsync, and Chertkovd.

In particular we worked on reproducing the results for the styrene spectrum presented in [1] and analyze the trick of changing the signs of all the coupling to fine-tune the fitting of the simulated spectrum over the experimental one.

Ref:
[1] "ANATOLIA: NMR software for spectral analysis of total lineshape", D.A. Cheshkova,b*, K.F. Sheberstova, D.O. Sinitsync, V.A. Chertkovd, doi: 10.1002/mrc.4689

# Versions of anaotolia

```
V 0.9 200? src/anatolia0.9.cpp
V 1.0 2017 src/anatolia1.0.cpp
V 1.1 2019 src/anatolia1.1.cpp
V 1.2 2021 src/anatolia1.2.cpp
V 1.2 2021 src/anatolia1.2plus.cpp // new version
```
The last version is src/anatolia.cpp and following its history shows its evolution.
The modification to test all combinations are [there](src/anatolia1.2plus.cpp)
```
V 1.2 2024 src/anatolia1.2plus.cpp // modified version by D. Jeannerat
```
Look at the [changes](https://github.com/djeanner/Anatole/commits/main/src/anatolia.cpp) of version.

# Compilation

MacOS GSL instalation & ANATOLIA compilation commands:
This is for gsl version 2.8. Adjust if necessary
[Commands for compilation](compile.md)
The binaraies are `bin/ANATOLIA` and `bin/ANATOLIAplus`.

# Update test data

We recommend to use the data as in this repository because some files were modified to avoid overwriting and consistency of the results.

We copied the `parameter.txt` file into `parameter_start.txt` and changed `OutputParameters` in
`data/THFCOOH/600/Input_Data.txt` accordingly.

For consitency of the file name oupt, we changed the `OutputParameters` to parameter.txt in
`data/THFCOOH/300/Input_Data.txt`

A copy of `Input_Data.txt` as  `Input_Data_refinement.txt` was written in `data/Styrene/1/` for the optimization.

For Styrene we replaced in all parameter files:
```
16  J{3,6}Meta,Alpha  ---->  J{3,6}Meta,Alpha        
17  J{3,7}Meta,BettaC ---->  J{3,7}Meta,BettaC            
18  J{3,8}Meta,BettaT ---->  J{3,8}Meta,BettaT 
```

To get the original version version of the data use this [script](getData.md)

# Run the tests

After compilation, run tests with:

```zsh
chmod +x demo/demo.zsh
cd demo
./demo.zsh
cd ..
```

The results are in the `demo` folder but files are also changed in the data folder.

Note: We are missing the Input_Data.txt and parameter.txt for Naphtalene and Azobenzene.


# The Input_Data.txt file

From ref [1]:
Typical decrease of the line broadening:
3.0; 2.0; 1.0; 0.8; 0.6; 0.4; 0.2; 0.1; 0.0 Hz

if SimMode is 1, it will only simulate (no optimization)

The first broadening should correspond to twice the maximal uncertainty in J-coupling values. No indication with respect to the chemical shifts.

# Reference

https://github.com/dcheshkov/ANATOLIA/

[1]D.A. Cheshkov, K.F. Sheberstov, D.O. Sinitsyn, V.A. Chertkov, ANATOLIA: NMR
software for spectral analysis of total lineshape. Magn. Reson. Chem., 2018,
56, 449, DOI: 10.1002/mrc.4689.

# Crude related information 

arXiv:2209.03708  https://doi.org/10.48550/arXiv.2209.03708
Total lineshape analysis of ALPHA-tetrahydrofuroic acid 1H NMR spectra

Total lineshape analysis of a-tetrahydrofuroic acid 1H NMR spectra - http://doi.org/10.48550/arXiv.2209.03708
Total line shape analysis of high-resolution NMR spectra -  http://doi.org/10.1016/bs.arnmr.2019.11.001



 https://www.sciencedirect.com/science/article/abs/pii/S0066410319300419?via%3Dihub#preview-section-introduction

# Addition to Anatolia 

A new version [src/anatolia1.2plus.cpp](src/anatolia1.2plus.cpp) was writted to
adds the search for all combination of J's.
It writes a proc file with the number following the one set as `CalcProcNo` parameter in the [Input_Data](data/Styrene/1/Input_Data.txt)
It writes the results file with the name starting with `allCombiTested_` compared to the filename given as  `OutputParameters` parameter in the [Input_Data](data/Styrene/1/Input_Data.txt)

# Quick run
 
[see](someScript.zsh) some shell script to can the calculation used here.


# Report of Styrene

The initial [spectrum](data/Styrene/1/pdata/1/1r).

The initial [run](data/Styrene/1/Input_Data.txtCOPY) from the [initial data](data/Styrene/1/parameters_start.txt)results in data corresponding to the first column of the paper but with different signs and some value with slightly different values 
[optimized value](data/Styrene/1/parameters.txt). It results to R-Factor: 13.67 %
The initial [optimized value spectrum](data/Styrene/1/pdata/3/1r).
Testing all combination of signs does not improve the situation much and the values are still not with the same sign and values as the paper:
[best of all combinations](data/Styrene/1/allCombiTested_parameters.txt) results to R-Factor: 13.50 %
The initial [best of all combinations spectrum](data/Styrene/1/pdata/4/1r).


Simply [restarting](data/Styrene/1/Input_Data_refinement.txt) with this "initial optimized" makes it worse [optimized value](data/Styrene/1/parameters_refinement.txt) results to R-Factor: 14.08 %

Starting [running](data/Styrene/1/Input_Data_refineBestCombi.txt) from the best of all combinations results in 
[optimized value](data/Styrene/1/parameters_refinement_afterBestCombi.txt) with 12.22
and 
[best of all combinations](data/Styrene/1/allCombiTested_parameters_refinement_afterBestCombi.txt) with 11.52

Only cheating the [input](data/Styrene/1/allCombiTested_parametersCheet2.txt) 
[running](data/Styrene/1/Input_Data_refineBestCombiCheet2.txt) results in 
[optimized value](data/Styrene/1/parameters_refinement_afterBestCombiCheet2.txt) with the R-Factor 5.85 % and the values of the table of the "Refined values" of the paper.
The spectrum is [best spectrum](data/Styrene/1/pdata/7/1r)
The best of all combinations does not find any better match.

Another step was this intermadiate results with 
Only cheating the [input](data/Styrene/1/allCombiTested_parametersCheet1.txt) 
[running](data/Styrene/1/Input_Data_refineBestCombiCheet1.txt) results in 
[optimized value](data/Styrene/1/parameters_refinement_afterBestCombiCheet1.txt) with the R-Factor 8.96 %. 
The best of all combinations does not find any better match.

[Comparison of the parameters for key set.](comparisonTableStyrene.md)


Main proc number in [data/Styrene/1/pdata](data/Styrene/1/pdata):

|procno|comment|
|------|-------|
|1 |inital exp spectrum|
|3 |intial optimized value |
|4 |intial after all combinations|
|7 |final best spectrum|
These four in Styrene.mnova for comparision.

See the images of the [spectra](spectra.md)

Note that exp numbers in the Input_Data files may cause spectrum overlap. Not all discussed here are in the repository.

## Conclusions

The alternation of the signs does not necessary finds the best combination if the minimum is not correct after the first step.

From the best combination we got almost the right set of signs (only a very small coupling has the sign different from the final best set). Here the "cheat" was only a change J{5-6} from 0.153164 to -0.23 and then it found a better match. But this value had to be set manually, in our hands (possibly not the case for the authors of the paper).

It may be that some coupling constants are still not absolutely perfect (value/sign). Some are either difficult or impossible to determine (if they do not manifest themselves in the spectrum).

# Acknowledgements

Many thanks to Kirill and his co-authors for this great package!