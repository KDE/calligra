

for i in `ls ../common_desktop/*desktop` 
do
j=`basename $i` 
echo $i $j
ln -s $i $j
done
