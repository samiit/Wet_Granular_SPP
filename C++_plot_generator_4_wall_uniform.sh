rm -rf images
mkdir images

small_bottom=`sed '1q;d' bot_top_small_big_number.txt`
big_bottom=`sed '2q;d' bot_top_small_big_number.txt`
small_top=`sed '3q;d' bot_top_small_big_number.txt`
big_top=`sed '4q;d' bot_top_small_big_number.txt`

wall_bottom=`sed '1q;d' wall_number.txt`
wall_right=`sed '2q;d' wall_number.txt`
wall_top=`sed '3q;d' wall_number.txt`
wall_left=`sed '4q;d' wall_number.txt`

echo set term png font "'Times New Roman, 20'" size 2000,2000 fontscale 3.0 > gnu_plotting.gp
echo unset xlabel >> gnu_plotting.gp
echo unset ylabel >> gnu_plotting.gp
echo set xrange [-0.02:0.42] >> gnu_plotting.gp
echo set yrange [-0.02:0.42] >> gnu_plotting.gp
echo unset xtics >> gnu_plotting.gp
echo unset ytics >> gnu_plotting.gp


for file in `ls ./particle_data/interior_* -v`
do

j=${file%.txt}
time=${j#*interior_}
time_number=$(echo $time 20 |awk '{printf "%d\n",$1*$2}')
if [ $time_number -le 2001 ]
then

echo
echo set out "'"./images/data_$time.png"'";
echo set title "'"t = $time s"'"

echo plot "'"$file"'" u 1:2:3 every ::$small_bottom::$(($small_bottom+$big_bottom-1)) w circle lc rgb "'"#6495ED"'" fill solid noborder title "''", "'"$file"'" u 1:2:3 every ::$(($small_bottom+$big_bottom+$small_top))::$(($small_bottom+$big_bottom+$small_top+$big_top-1)) w circle lc rgb "'"pink"'" fill solid noborder title "''", "'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''","'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''","'"wall_data_stat_1.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_1.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_1.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_2.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_2.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_2.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::0::120 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::121::123 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::124 w circle lw 2 lc rgb "'"black"'" title "''"

echo unset out
echo

fi

done >> gnu_plotting.gp

gnuplot gnu_plotting.gp
python new_Python*

avconv -r 20 -i ./images/image_%4d.png Bot_belt.avi

rm -rf images

