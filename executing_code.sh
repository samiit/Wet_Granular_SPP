#############----------------------#############
#	This is the input file for the code.
#	It does the following:
#		a) Compiles the code
#		b) Creates a directory particle_data. If the directory is already present, it leaves it as it is.
#		c) Executes the program with the inputs specified below
#	WARNING: Please ensure there are no spaces before/after the equal to (=) sign in the following statements
#############----------------------#############
# Compiling and generating the executable (machine level instruction)
g++ Sam_gran_compact_*.cpp -std=c++11 -O3

mkdir particle_data

# There are 17 arguments along with the executable: 
t_initial=0.0 #1 Initial time = 0.0 (default)
t_final=201.0 #2 Final time = 410.0 (default)

# Specify whether initial data file present or not. If then, the files should be named "interior_ini.txt" and "wall_ini.txt", respectively, and kept in the same directory as the executable 'a.out'

IntDataFile_isThere=0 #3 Initial interior particles data file present or not (Yes = 1, No = 0) = 0 (default)
WallDataFile_isThere=0 #4 Initial wall particles data file present or not (Yes = 1, No = 0) = 0 (default) # If provided, the walls must be the 4 sides of a rectangular enclosure. The bottom and top walls can only have x-velocity while the side walls can have only the y-component of velocity

########### Ignore these inputs if the initial interior and wall particles files are provided ################

R_m=0.0025 #5 Mean radius of the particles (m) = 0.0025 (default)
N_conc=0.0 #6 Number concentration of secondary particles = 0.0
r_m=0.00125 #7 Mean radius of the small (secondary, impurity) particles (m) = 0.00125 (default) 

interiorParticleType=3 #8 Type of poly-dispersity for the particles in the domain. (1) Mono/bi-disperse (CONC. based) (2) Uniform random within r_small and r_big, (3) Gaussian random distribution, with standard deviation percentage below
gauss_stdDev_percRm=0.1 #9 Standard deviation of the Gaussian distribution for radius (in terms of % mean radius, R_m). Relevant only if Gaussian distribution is active

r_small=0.001 #10 Minimum radius of the particles in the domain
r_big=0.004 #11 Maximum radius of the particles in the domain

vel_BOT_wall=0.0 #12 Bottom belt's x-velocity (m/s) = 0.5 (default)
vel_RIGHT_wall=0.0 #13 Right wall's y-velocity (m/s) = 0.0 (default)
vel_TOP_wall=0.0 #14 Top wall's x-velocity (m/s) = 0.0 (default)
vel_LEFT_wall=0.0 #15 Left wall's y-velocity (m/s) = 0.0 (default)
time_BELT_starts=0.0 #16 Belt starts at time (s) = 10.0 (default)

########### *************************************************************** ################

LVA=0.01 #17 Local Velocity Anarchy = 0.0 (default)
time_SPP_starts=1 #18 Self-propelling particles get active at time (s) = 0.0 (default)


./a.out $t_initial $t_final $IntDataFile_isThere $WallDataFile_isThere $R_m $N_conc $r_m $interiorParticleType $gauss_stdDev_percRm $r_small $r_big $vel_BOT_wall $vel_RIGHT_wall $vel_TOP_wall $vel_LEFT_wall $time_BELT_starts $LVA $time_SPP_starts

#gnuplot Post_eg.gp
#gnuplot Post_graph.gp

for f in *.eps; do j=${f%.*}; convert -density 200 $j.eps $j.png; done

int_part=`cat ./particle_data/interior_0.05.txt| wc -l`
tot_files=`ls particle_data/interior* -1|wc -l`
sed -i 's/#define core.*/#define core '${int_part}'/' ./mixing_parameter_Beta_weak_strong.c
sed -i 's/#define total_files.*/#define total_files '${tot_files}'/' ./mixing_parameter_Beta_weak_strong.c

gcc mixing_parameter_Beta_weak_strong.c -lm -llapack -O2 -o mix_param

./mix_param

