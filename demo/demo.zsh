#!/bin/zsh
Version=${1:-1}
VersionComplementText=""
echo "version : $Version"
LogTimeTxt=("logTime.txt")
# Check if Version is set to 1
if [[ $Version -gt 1 ]]; then
  LogTimeTxt=("logTime_v$Version.txt")
  VersionComplementText="_v$Version"
fi
echo "LogTime : $LogTimeTxt"

DATASETS2=("ODCB")
DATASETS=("ODCB" "THFCOOH" "Styrene" "Naphtalene" "Azobenzene")
echo > $LogTimeTxt
# Loop through each dataset and copy its folder
for DATASET in "${DATASETS[@]}"; do
  if [[ "$DATASET" == "THFCOOH" ]]; then
    ACQUS_ALL=("300" "600")
  else
    ACQUS_ALL=("1")
  fi

  for ACQUS in "${ACQUS_ALL[@]}"; do
    echo "Running $DATASET-$ACQUS" >> $LogTimeTxt
    echo "running anatolia $VersionComplementText for $DATASET-$ACQUS..."
    start_time=$(date +%s)
    if [[ $Version -gt 1 ]]; then
         echo "call ../bin/ANATOLIAplus ../data/$DATASET/$ACQUS/"
        ../bin/ANATOLIAplus ../data/$DATASET/$ACQUS/ >> log_$DATASET-$ACQUS-V$Version.txt
    else   
         echo "call ../bin/ANATOLIA ../data/$DATASET/$ACQUS/"
        ../bin/ANATOLIA ../data/$DATASET/$ACQUS/ >> log_$DATASET-$ACQUS.txt
    fi
    end_time=$(date +%s);elapsed_time=$((end_time - start_time));echo "Elapsed time: $elapsed_time seconds" >> $LogTimeTxt
    cp ../data/$DATASET/$ACQUS/parameters.txt $DATASET-$ACQUS$VersionComplementText.txt
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
    if [[ $Version -gt 1 ]]; then
    else   
    fi
    if [[ -f "./$DATASET-$ACQUS$VersionComplementText.txt" ]]; then
      echo "Compare results for $DATASET-$ACQUS$VersionComplementText"
      echo "diff referenceResults/$DATASET-$ACQUS.txt ./$DATASET-$ACQUS$VersionComplementText.txt"
      echo "===================================================="
      diff referenceResults/$DATASET-$ACQUS.txt ./$DATASET-$ACQUS$VersionComplementText.txt
      echo "===================================================="
    else
      echo "Skipping $DATASET-$ACQUS$VersionComplementText: ./$DATASET-$ACQUS$VersionComplementText.txt does not exist."
    fi
    echo
  done
done

