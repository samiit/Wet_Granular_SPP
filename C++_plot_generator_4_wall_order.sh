rm -rf images
mkdir images

echo set term png font "'Times New Roman, 20'" size 2000,2000 fontscale 2.0 > gnu_plotting_2.gp
echo unset xlabel >> gnu_plotting_2.gp
echo unset ylabel >> gnu_plotting_2.gp
echo set xrange [-0.04:0.44] >> gnu_plotting_2.gp
echo set yrange [-0.04:0.44] >> gnu_plotting_2.gp
echo unset xtics >> gnu_plotting_2.gp
echo unset ytics >> gnu_plotting_2.gp
echo set cbrange [0:1] >> gnu_plotting_2.gp
echo set zrange [0:1]  >> gnu_plotting_2.gp
echo set loadpath '"'/home/sam/.gnuplotting'"' >> gnu_plotting_2.gp
echo load '"'default.plt'"' >> gnu_plotting_2.gp
echo set colorbox horizontal user origin 0.1,0.04 size .8,0.02  >> gnu_plotting_2.gp

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

echo plot "'"$file"'" u 1:2:3:6  w circles lc palette fill solid noborder title "''", "'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''","'"./particle_data/wall_1_$time.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_1.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_1.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_1.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''","'"wall_data_stat_2.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_2.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_2.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::0::121 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::122::124 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_3.txt"'" u 1:2:3 every ::125 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::0::120 w circle lw 2 lc rgb "'"black"'" title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::121::123 w circle lc rgb "'"black"'" fill solid noborder title "''", "'"wall_data_stat_4.txt"'" u 1:2:3 every ::124 w circle lw 2 lc rgb "'"black"'" title "''"


echo unset out
echo
fi
done >> gnu_plotting_2.gp

gnuplot gnu_plotting_2.gp
python new_Python*

avconv -r 20 -i ./images/image_%4d.png Mono_bot_belt_mermin.avi

cp images/image_0201.png local_order_0201.png
cp images/image_2001.png local_order_2001.png

#rm -rf images

