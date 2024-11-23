We recommend to use the data as in this repository because some files were modified to avoid overwriting and consistency of the results.
We copied the `parameter.txt` file into `parameter_start.txt` and changed `OutputParameters` in
`data/THFCOOH/600/Input_Data.txt` accordingly.
For consitency of the file name oupt, we changed the `OutputParameters` to parameter.txt in
`data/THFCOOH/300/Input_Data.txt`
A copy of `Input_Data.txt` as  `Input_Data_refinement.txt` was written in `data/Styrene/1/` for the optimization.

For Styrene we replaced 
```
16  J{3,6}Meta,Alpha  ---->  J{3,6}Meta,Alpha        
17  J{3,7}Meta,BettaC ---->  J{3,7}Meta,BettaC            
18  J{3,8}Meta,BettaT ---->  J{3,8}Meta,BettaT 
```

Running the code below will overwrite these changes. But you can track the changes.

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