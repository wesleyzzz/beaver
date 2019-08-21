

: << 'END'
for K0 in $1  ; do
    #name=${meshfile##*/}
    #base=${name%.e}
    ../../beaver-opt -i debug_NiCr_radiation.i Variables/CA/initial_condition=${K0} 
done 
END

#inputs=(ce2sb_ce4sb3_ic1.i ce2sb_ce4sb3_ic2.i ce2sb_ce4sb3_kks_multiphase.i ce2sb_ce4sb3_liquid_ic1.i ce2sb_ce4sb3_liquid_ic2.i)
#inputs=(ce2sb_ce4sb3_liquid_ic1.i ce2sb_ce4sb3_liquid_ic2.i)
inputs=(ce2sb_ce4sb3_liquid_ic1.i)
#inputs=(ce2sb_ce4sb3_liquid_ic2.i)
for input in ${inputs[@]} ; do
  echo "################" ${input}
  mpirun -np 10 ../../beaver-opt -i ${input} Executioner/dtmin=1.0e-4 Executioner/num_steps=200 Outputs/console=false
done



