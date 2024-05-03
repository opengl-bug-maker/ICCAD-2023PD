array=(
    "case01-input"
    "case02-input"
    "case03-input"
    "case04-input"
    "case05-input"
    "case06-input"
    "case07-input"
    "case08-input"
    "case09-input"
    "case10-input"
)
for a in "${array[@]}";
do
    ./iccad2023PD $a
done