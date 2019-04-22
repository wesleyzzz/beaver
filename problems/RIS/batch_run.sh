

: << 'END'
for K0 in $1  ; do
    #name=${meshfile##*/}
    #base=${name%.e}
    ../../beaver-opt -i debug_NiCr_radiation.i Variables/CA/initial_condition=${K0} 
done 
END


: << 'END'
kb=8.617e-5 
Kb=$(printf "%.16f" ${kb})
omega=1.12e-11
Omega=$(printf "%.16f" ${omega})
Ef=1.6 #formulation energy of vacancy
for T in $(seq 900 50 950)  ; do
    echo ${kb} ${Ef} $T
    C0=$(echo "e(-${Ef}/${T}/${Kb})/${Omega}" | bc -l)
    echo 'Vacancy thermal equlibrium: /um^3' ${C0} 

    ../../beaver-opt -i NiCr_radiation.i Variables/Cv/initial_condition=${C0} BCs/Cv_right/value=${C0} Materials/NiCr/temperature=${T} 
    mv NiCr_radiation_out.e NiCr_radiation_out_${T}K.e

    ../../beaver-opt -i NiCr_no_radiation.i Variables/Cv/initial_condition=${C0} BCs/Cv_right/value=${C0} Materials/NiCr/temperature=${T} 
    mv NiCr_no_radiation_out.e NiCr_no_radiation_out_${T}K.e
done 
END

for bc in -100000 -1000 -10 0 10 1000 100000 ; do
  mpirun -np 6 ../../beaver-opt -i NiCr_radiation_corrosion.i BCs/CB_right/value=${bc}
  mv NiCr_radiation_corrosion_out.e NiCr_radiation_corrosion_out_${bc}.e
done
#../../beaver-opt -i NiCr_radiation.i
#../../beaver-opt -i NiCr_no_radiation.i



