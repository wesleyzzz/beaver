
for K0 in $1  ; do
    #name=${meshfile##*/}
    #base=${name%.e}
    ../../beaver-opt -i debug_NiCr_radiation.i Variables/CA/initial_condition=${K0} 
done 

