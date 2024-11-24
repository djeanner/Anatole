# ODCB

```
echo "========== This is fine at first attempt =========";

echo "takes less than a second"
cp -rp data/ODCB/1/pdata/3/ data/ODCB/1/pdata/4/

cat data/ODCB/1/Input_Data.txt
time bin/ANATOLIAplus data/ODCB/1 >> last_log_ODCB.txt
grep "SimMode" data/ODCB/1/Input_Data.txt
grep "InputParameters" data/ODCB/1/Input_Data.txt
grep "OutputParameters" data/ODCB/1/Input_Data.txt
procOutNumber=$(grep "CalcProcNo" ../data/ODCB/1/Input_Data.txt | sed 's/CalcProcNo//g' | sed 's/" "//g' | sed 's/\t//g')
echo "results in data/$DATASET/ODCB/1/$procOutNumber/1r"
echo "===================";
```
# Styrene

```
echo "takes a couple of minute"

cp -rp data/Styrene/1/pdata/3/ data/Styrene/1/pdata/4/
cp -rp data/Styrene/1/pdata/3/ data/Styrene/1/pdata/5/
cp -rp data/Styrene/1/pdata/3/ data/Styrene/1/pdata/6/
cp -rp data/Styrene/1/pdata/3/ data/Styrene/1/pdata/7/
cp -rp data/Styrene/1/pdata/3/ data/Styrene/1/pdata/8/

cat data/Styrene/1/Input_Data.txt
time bin/ANATOLIAplus data/Styrene/1  >> last_log_Styrene.txt
grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt

echo "=====Refinement of first step (before all combinations) - Does not find a better min ==============";

cp data/Styrene/1/Input_Data.txt data/Styrene/1/Input_Data.txtCOPY
cp data/Styrene/1/Input_Data_refinement.txt data/Styrene/1/Input_Data.txt
grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt
time bin/ANATOLIA data/Styrene/1 >> last_log_Styrene.txt
cp data/Styrene/1/Input_Data.txtCOPY data/Styrene/1/Input_Data.txt
rm data/Styrene/1/Input_Data.txtCOPY

echo "=====Refinement after all combi from result above = Does find a real min but not the best one ==============";
cp data/Styrene/1/Input_Data.txt data/Styrene/1/Input_Data.txtCOPY

cp data/Styrene/1/Input_Data_refineBestCombi.txt data/Styrene/1/Input_Data.txt

grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt
time bin/ANATOLIAplus data/Styrene/1 >> last_log_Styrene.txt
cp data/Styrene/1/Input_Data.txtCOPY data/Styrene/1/Input_Data.txt

echo "=====Refinement after manual set of values Cheet2 == This finds the min of the paper at 5.9 % ==============";

cp data/Styrene/1/Input_Data_refineBestCombiCheet2.txt data/Styrene/1/Input_Data.txt
grep "SimMode" data/Styrene/1/Input_Data.txt
grep "InputParameters" data/Styrene/1/Input_Data.txt
grep "OutputParameters" data/Styrene/1/Input_Data.txt
grep "CalcProcNo" data/Styrene/1/Input_Data.txt
time bin/ANATOLIAplus data/Styrene/1 >> last_log_Styrene.txt
cp data/Styrene/1/Input_Data.txtCOPY data/Styrene/1/Input_Data.txt
```
# THFCOOH

```

date > THFCOOH.log
echo "===================Input_Data" >> THFCOOH.log
cat data/THFCOOH/300/Input_Data.txt >> THFCOOH.log
echo "===================Start ANATOLIAplus" >> THFCOOH.log
time bin/ANATOLIAplus data/THFCOOH/300 >> THFCOOH.log
echo "===================End ANATOLIAplus" >> THFCOOH.log
grep "SimMode" data/THFCOOH/300/Input_Data.txt >> THFCOOH.log
grep "InputParameters" data/THFCOOH/300/Input_Data.txt >> THFCOOH.log
grep "OutputParameters" data/THFCOOH/300/Input_Data.txt >> THFCOOH.log
grep "CalcProcNo" data/THFCOOH/300/Input_Data.txt >> THFCOOH.log
echo "===================";
date >> THFCOOH.log
echo "===================" >> THFCOOH.log

cat data/THFCOOH/600/Input_Data.txt
echo "===================Input_Data" >> THFCOOH.log
cat data/THFCOOH/600/Input_Data.txt >> THFCOOH.log
echo "===================Start ANATOLIAplus" >> THFCOOH.log
time bin/ANATOLIAplus data/THFCOOH/600 >> THFCOOH.log
echo "===================End ANATOLIAplus" >> THFCOOH.log
grep "SimMode" data/THFCOOH/600/Input_Data.txt >> THFCOOH.log
grep "InputParameters" data/THFCOOH/600/Input_Data.txt >> THFCOOH.log
grep "OutputParameters" data/THFCOOH/600/Input_Data.txt >> THFCOOH.log
grep "CalcProcNo" data/THFCOOH/600/Input_Data.txt >> THFCOOH.log
echo "===================";
date >> THFCOOH.log
echo "===================" >> THFCOOH.log

```
