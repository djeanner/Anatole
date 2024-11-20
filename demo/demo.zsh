#!/bin/zsh

DATASETS2=("ODCB")
DATASETS=("ODCB" "THFCOOH" "Styrene" "Naphtalene" "Azobenzene")
echo > logTime.txt
# Loop through each dataset and copy its folder
for DATASET in "${DATASETS[@]}"; do
  if [[ "$DATASET" == "THFCOOH" ]]; then
    ACQUS_ALL=("300" "600")
  else
    ACQUS_ALL=("1")
  fi

  for ACQUS in "${ACQUS_ALL[@]}"; do
    echo "Running $DATASET-$ACQUS" >> logTime.txt
    echo -n "start at : " >> logTime.txt
    date >> logTime.txt
    echo "running anatolia for $DATASET-$ACQUS..."
    ../bin/ANATOLIA ../data/$DATASET/$ACQUS/ >> log_$DATASET-$ACQUS.txt
    echo -n "ends at  : " >> logTime.txt
    date >> logTime.txt
    cp ../data/$DATASET/$ACQUS/parameters.txt $DATASET-$ACQUS.txt
    # Dump some results
    echo "results in data/$DATASET/$ACQUS"
    grep "SimMode" ../data/$DATASET/$ACQUS/Input_Data.txt
    InputParameters=$(grep "InputParameters" ../data/$DATASET/$ACQUS/Input_Data.txt | sed 's/InputParameters//g' | sed 's/" "//g' | sed 's/\t//g')
    OutputParameters=$(grep "OutputParameters" ../data/$DATASET/$ACQUS/Input_Data.txt | sed 's/OutputParameters//g' | sed 's/" "//g' | sed 's/\t//g')
    echo "Input  param : data/$DATASET/$ACQUS/$InputParameters";
    echo "Output param : data/$DATASET/$ACQUS/$OutputParameters";
    grep "CalcProcNo" ../data/$DATASET/$ACQUS/Input_Data.txt
    ExpProcNo=$(grep "ExpProcNo" ../data/$DATASET/$ACQUS/Input_Data.txt | sed 's/ExpProcNo//g' | sed 's/" "//g' | sed 's/\t//g')
    ExpBroadedProcNo=$(grep "ExpBroadedProcNo" ../data/$DATASET/$ACQUS/Input_Data.txt | sed 's/ExpBroadedProcNo//g' | sed 's/" "//g' | sed 's/\t//g')
    CalcProcNo=$(grep "CalcProcNo" ../data/$DATASET/$ACQUS/Input_Data.txt | sed 's/CalcProcNo//g' | sed 's/" "//g' | sed 's/\t//g')
    echo "Fitting to            data/$DATASET/$ACQUS/pdata/$ExpProcNo/1r"
    echo "ExpBroadedProcNo in   data/$DATASET/$ACQUS/pdata/$ExpBroadedProcNo/1r"
    echo "Resulting spectrum in data/$DATASET/$ACQUS/pdata/$CalcProcNo/1r"
    echo
  done 
done
echo "All datasets copied!"
for DATASET in "${DATASETS[@]}"; do
 if [[ "$DATASET" == "THFCOOH" ]]; then
    ACQUS_ALL=("300" "600")
  else
    ACQUS_ALL=("1")
  fi

  for ACQUS in "${ACQUS_ALL[@]}"; do
    # Check if the dataset file exists
    if [[ -f "./$DATASET-$ACQUS.txt" ]]; then
      echo "Compare results for $DATASET-$ACQUS"
      echo "diff referenceResults/$DATASET-$ACQUS.txt ./$DATASET-$ACQUS.txt"
      echo "===================================================="
      diff referenceResults/$DATASET-$ACQUS.txt ./$DATASET-$ACQUS.txt
      echo "===================================================="
    else
      echo "Skipping $DATASET-$ACQUS: ./$DATASET-$ACQUS.txt does not exist."
    fi
    echo
  done
done

