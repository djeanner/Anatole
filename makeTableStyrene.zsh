echo "Label"  > tableStyrene0.txt
cat data/Styrene/1/parameters_start.txt | cut -c4-10 | sed s/","/"-"/g >> tableStyrene0.txt
echo "R-Factor" >> tableStyrene0.txt


echo "1step"  > tableStyrene2.txt
cat data/Styrene/1/parameters.txt| head -26 |cut -c30-45 >> tableStyrene2.txt
cat data/Styrene/1/parameters.txt| grep "R-Factor" |cut -c11-15 >> tableStyrene2.txt

echo "best Combi"  > tableStyrene3.txt
cat data/Styrene/1/allCombiTested_parameters.txt| head -26 |cut -c30-45 >> tableStyrene3.txt
cat data/Styrene/1/allCombiTested_parameters.txt| grep "R-Factor" |cut -c11-15 >> tableStyrene3.txt


echo "refinement From BestCombi"  > tableStyrene4.txt
cat data/Styrene/1/parameters_refinement_afterBestCombi.txt| head -26 |cut -c30-45 >> tableStyrene4.txt
cat data/Styrene/1/parameters_refinement_afterBestCombi.txt| grep "R-Factor" |cut -c11-15 >> tableStyrene4.txt

echo "Cheeting Starting Values1"  > tableStyrene6.txt
cat data/Styrene/1/allCombiTested_parametersCheet1.txt| head -26 |cut -c30-45 >> tableStyrene6.txt
echo "100">> tableStyrene6.txt

echo "after Cheeting Optim1"  > tableStyrene7.txt
cat data/Styrene/1/parameters_refinement_afterBestCombiCheet1.txt| head -26 |cut -c30-45 >> tableStyrene7.txt
cat data/Styrene/1/parameters_refinement_afterBestCombiCheet1.txt| grep "R-Factor" |cut -c11-15 >> tableStyrene7.txt

echo "Cheeting Starting Values2"  > tableStyrene8.txt
cat data/Styrene/1/allCombiTested_parametersCheet2.txt| head -26 |cut -c30-45 >> tableStyrene8.txt
echo "100">> tableStyrene8.txt

echo "after Cheeting Optim2"  > tableStyrene9.txt
cat data/Styrene/1/parameters_refinement_afterBestCombiCheet2.txt| head -26 |cut -c30-45 >> tableStyrene9.txt
cat data/Styrene/1/parameters_refinement_afterBestCombiCheet2.txt| grep "R-Factor" |cut -c11-15 >> tableStyrene9.txt

paste -d',' tableStyrene0.txt tableStyrene1.txt tableStyrene2.txt tableStyrene3.txt tableStyrene4.txt tableStyrene6.txt tableStyrene7.txt tableStyrene8.txt tableStyrene9.txt  > tableStyrene.csv
paste -d',' tableStyrene0.txt tableStyrene1.txt tableStyrene2.txt tableStyrene3.txt tableStyrene4.txt tableStyrene8.txt tableStyrene9.txt  > tableStyrene.csv

./makeMDtable.zsh tableStyrene.csv > comparisonTableStyrene.md
