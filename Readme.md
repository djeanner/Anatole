This allows to see the changes of version of Anatolia NMR software

The last version is src/anatolia.cpp
V 0.9 200?
V 1.0 2017
V 1.1 2019
V 1.2 2021

MacOS GSL instalation & ANATOLIA compilation commands:
for  gsl version 2.8, but adjust if necessary
We skiped the make install
```
mkdir bin
cd bin
echo "bin" >> ../.gitignore
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

load tests:
```
mkdir data
cd data
echo "data/**/1r" >> ../.gitignore
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


run tests ODCB:
```
cd demo
../bin/ANATOLIA ../data/ODCB/1/
```


# Acknowledgements

Many thanks to the authors for this great package!

# Reference

D.A. Cheshkov, K.F. Sheberstov, D.O. Sinitsyn, V.A. Chertkov, ANATOLIA: NMR
software for spectral analysis of total lineshape. Magn. Reson. Chem., 2018,
56, 449, DOI: 10.1002/mrc.4689.
