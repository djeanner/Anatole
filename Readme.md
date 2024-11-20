Copy for automatic run of the test samples of Anatolia
 
Orinig of data:  https://github.com/dcheshkov/ANATOLIA/


# Verions of anaotolia

The last version is src/anatolia.cpp
V 0.9 200?
V 1.0 2017
V 1.1 2019
V 1.2 2021

# Compilation

MacOS GSL instalation & ANATOLIA compilation commands:
This is for gsl version 2.8. Adjust if necessary
```
mkdir bin
cd bin
echo "bin/" >> ../.gitignore
curl -O ftp://ftp.gnu.org/gnu/gsl/gsl-latest.tar.gz
gzip -dc gsl-latest.tar.gz | tar xvf -
cd gsl-2.8 
./configure
make
rm gsl-latest.tar.gz
rm -rf gsl-*
cd ..
g++ -std=c++11 ../src/anatolia.cpp ./gsl-2.8/.libs/libgsl.a ./gsl-2.8/cblas/.libs/libgslcblas.a -I./gsl-2.8 -o ANATOLIA
cd ..
```
The binaray is bin/ANATOLIA
Note: We skiped the make install and call the library directly at the compilation step


# Update test data

We recommend to use the data as in this repository because some files were modified to avoid overwriting and consistency of the results.
We copied the `parameter.txt` file into `parameter_start.txt` and changed `OutputParameters` in
`data/THFCOOH/600/Input_Data.txt` accordingly.
For consitency of the file name oupt, we changed the `OutputParameters` to parameter.txt in
`data/THFCOOH/300/Input_Data.txt`
A copy of `Input_Data.txt` as  `Input_Data_refinement.txt` was written in `data/Styrene/1/` for the optimization.

To get the original version version:
```
mkdir data
cd data
wget https://github.com/dcheshkov/ANATOLIA/archive/refs/heads/master.zip
unzip master.zip
cp -rp ANATOLIA-master/Examples/ODCB .
cp -rp ANATOLIA-master/Examples/THFCOOH .
cp -rp ANATOLIA-master/Examples/Styrene .
cp -rp ANATOLIA-master/Examples/Naphtalene .
cp -rp ANATOLIA-master/Examples/Azobenzene .
rm -r ANATOLIA-master
rm master.zip
cd ..
```

# Run the tests

After compilation, run tests with:

```zsh
chmod +x demo/demo.zsh
demo/demo.zsh
```
The results are in the `demo` folder.

Note: we are missing the Input_Data.txt and parameter.txt for Naphtalene and Azobenzene.

# Acknowledgements

Many thanks to the authors for this great package!

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

 ANATOLIA: NMR software for spectral analysis of total lineshape
D.A. Cheshkova,b*, K.F. Sheberstova, D.O. Sinitsync, V.A. Chertkovd
  doi: 10.1002/mrc.4689

 https://www.sciencedirect.com/science/article/abs/pii/S0066410319300419?via%3Dihub#preview-section-introduction


# Quick run

```

echo "takes less than a second"

cat data/ODCB/1/Input_Data.txt
time bin/ANATOLIA data/ODCB/1
grep "SimMode" data/ODCB/1/Input_Data.txt
grep "InputParameters" data/ODCB/1/Input_Data.txt
grep "OutputParameters" data/ODCB/1/Input_Data.txt
procOutNumber=$(grep "CalcProcNo" ../data/ODCB/1/Input_Data.txt | sed 's/CalcProcNo//g' | sed 's/" "//g' | sed 's/\t//g')
echo "results in data/$DATASET/ODCB/1/$procOutNumber/1r"
echo "===================";

echo "takes a couple of minute"

cat data/Styrene/1/Input_Data.txt
time bin/ANATOLIA data/Styrene/1
grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt
echo "=====Refinement==============";
cp data/Styrene/1/Input_Data.txt data/Styrene/1/Input_Data.txtCOPY
cp data/Styrene/1/Input_Data_refinement.txt data/Styrene/1/Input_Data.txt
grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt
time bin/ANATOLIA data/Styrene/1
cp data/Styrene/1/Input_Data.txtCOPY data/Styrene/1/Input_Data.txt
rm data/Styrene/1/Input_Data.txtCOPY
echo "===================";

cat data/THFCOOH/300/Input_Data.txt
time bin/ANATOLIA data/THFCOOH/300
grep "SimMode" data/THFCOOH/300/Input_Data.txt
grep "InputParameters" data/THFCOOH/300/Input_Data.txt
grep "OutputParameters" data/THFCOOH/300/Input_Data.txt
grep "CalcProcNo" data/THFCOOH/300/Input_Data.txt

echo "===================";

cat data/THFCOOH/600/Input_Data.txt
time bin/ANATOLIA data/THFCOOH/600
grep "SimMode" data/THFCOOH/600/Input_Data.txt
grep "InputParameters" data/THFCOOH/600/Input_Data.txt
grep "OutputParameters" data/THFCOOH/600/Input_Data.txt
grep "CalcProcNo" data/THFCOOH/600/Input_Data.txt
echo "===================";

```
