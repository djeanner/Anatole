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

