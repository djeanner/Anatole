# Compilation


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
echo "Compilation of the last version by the authors of the Anatolia paper"
g++ -std=c++11 -O3 ../src/anatolia.cpp ./gsl-2.8/.libs/libgsl.a ./gsl-2.8/cblas/.libs/libgslcblas.a -I./gsl-2.8 -o ANATOLIA
echo "Compilation of the new version testing all combinations of signs for the couplings"
g++ -std=c++11 -O3 ../src/anatolia1.2plus.cpp ./gsl-2.8/.libs/libgsl.a ./gsl-2.8/cblas/.libs/libgslcblas.a -I./gsl-2.8 -o ANATOLIAplus
cd ..
```