############# model NiCr alloy corrosion in molten salt ############
###CA: Cr; CB: Ni###########
###Unit: micron

[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 0.01
  nx = 1000
[]

[Variables]
  [./Cv]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1000.0
  [../]
  [./Ci]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./CA]
    order = FIRST
    family = LAGRANGE
    #scaling = 1.0e-10
    initial_condition = 17857142857  # 0.2 fraction
  [../]
  [./CB]
    order = FIRST
    family = LAGRANGE
    #scaling = 1.0e-10
    initial_condition = 71428571428 # 0.8 fraction
  [../]
[]

[AuxVariables]
  [./CA_salt]
  [../]
  [./Di]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./Dv]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./DA]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./DB]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./Xv]
  [../]
  [./Xi]
  [../]
  [./XA]
  [../]
  [./XB]
  [../]
  [./K0]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  ########### vacancy #############
  [./Cv_dt]
    type = TimeDerivative 
    variable = Cv
  [../]
  [./Cv_vA]
    type = CoupledAdvection
    variable = Cv
    v = CA
    mat_coef = dAvOmega
    const_coef = -1.0
  [../]
  [./Cv_vB]
    type = CoupledAdvection
    variable = Cv
    v = CB
    mat_coef = dBvOmega
    const_coef = -1.0
  [../]
  [./Cv_Jv]
    type = CoupledDiffusion
    variable = Cv
    mat_coef = VacancyDiffusivity
    const_coef = 1.0
  [../]
  [./Cv_K0]
    type = DefectProductionRate
    variable = Cv
  [../]
  [./Cv_R]
    type = VacancyInterstitialReaction
    variable = Cv
    v = Ci
    mat_coef = vi_reaction_rate 
    value = 1.0
  [../]

  ########### interstitial #############
  [./Ci_dt]
    type = TimeDerivative 
    variable = Ci
  [../]
  [./Ci_iA]
    type = CoupledAdvection
    variable = Ci
    v = CA
    mat_coef = dAiOmega
    const_coef = 1.0 
  [../]
  [./Ci_iB]
    type = CoupledAdvection
    variable = Ci
    v = CB
    mat_coef = dBiOmega
    const_coef = 1.0 
  [../]
  [./Ci_Ji]
    type = CoupledDiffusion
    variable = Ci
    mat_coef = InterstitialDiffusivity
    const_coef = 1.0
  [../]
  [./Ci_K0]
    type = DefectProductionRate
    variable = Ci
  [../]
  [./Ci_R]
    type = VacancyInterstitialReaction
    variable = Ci
    v = Cv
    mat_coef = vi_reaction_rate 
    value = 1.0
  [../]

  ########### Cr #############
  [./CA_dt]
    type = TimeDerivative 
    variable = CA
  [../]
  [./CA_iA]
    type = CoupledAdvection
    variable = CA
    v = Ci
    mat_coef = dAiOmega
    const_coef = 1.0
  [../]
  [./CA_vA]
    type = CoupledAdvection
    variable = CA
    v = Cv
    mat_coef = dAvOmega
    const_coef = -1.0
  [../]
  [./CA_JA]
    type = CoupledDiffusion
    variable = CA
    mat_coef = ElemADiffusivity
    const_coef = 1.0 
  [../] 

  ########### Ni #############
  [./CB_dt]
    type = TimeDerivative 
    variable = CB
  [../]
  [./CB_iB]
    type = CoupledAdvection
    variable = CB
    v = Ci
    mat_coef = dBiOmega
    const_coef = 1.0
  [../]
  [./CB_vB]
    type = CoupledAdvection
    variable = CB
    v = Cv
    mat_coef = dBvOmega
    const_coef = -1.0
  [../]
  [./CB_JB]
    type = CoupledDiffusion
    variable = CB
    mat_coef = ElemBDiffusivity
    const_coef = 1.0 
  [../] 
[]

[AuxKernels]
  [./CA_salt_ak]
    type = SaltSoluteConcentrationAux
    variable = CA_salt 
    solute_init = 0.0
  [../]
  [./K0]
    type = MaterialRealAux
    variable = K0
    property = point_defect_rate
  [../]
  [./Di]
    type = MaterialRealAux
    variable = Di
    property = InterstitialDiffusivity
  [../]
  [./Dv]
    type = MaterialRealAux
    variable = Dv
    property = VacancyDiffusivity
  [../]
  [./DA]
    type = MaterialRealAux
    variable = DA
    property = ElemADiffusivity
  [../]
  [./DB]
    type = MaterialRealAux
    variable = DB
    property = ElemBDiffusivity
  [../]
  [./Xv]
    type = ParsedAux
    variable = Xv
    args = 'Cv'
    function = 'Cv*1.12e-11'
  [../]
  [./Xi]
    type = ParsedAux
    variable = Xi
    args = 'Ci'
    function = 'Ci*1.12e-11'
  [../]
  [./XA]
    type = ParsedAux
    variable = XA
    args = 'CA'
    function = 'CA*1.12e-11'
  [../]
  [./XB]
    type = ParsedAux
    variable = XB
    args = 'CB'
    function = 'CB*1.12e-11'
  [../]
[]

[Functions]
  [./dose_rate]
    type = ParsedFunction
    value = 1.0e-4 #+x*1.0e-7 #*sin(x/25*3.1415926) #
  [../]
[]


[BCs]
  active = 'Ci_left Ci_right Cv_left Cv_right CA_left CA_right CB_left CB_right'
  [./Ci_left]
    type = NeumannBC
    variable = Ci
    boundary = 'left'
    value = 0
  [../]
  [./Ci_right]
    type = DirichletBC
    variable = Ci
    boundary = 'right'
    value = 0
  [../]

  [./Cv_left]
    type = NeumannBC
    variable = Cv
    boundary = 'left'
    value = 0.0
  [../]
  [./Cv_right]
    type = DirichletBC
    variable = Cv
    boundary = 'right'
    value = 1000.0
  [../]
  [./Cv_right2]
    type = CoupledDirichletBC
    variable = Cv
    v = 'CA CB'
    boundary = 'right'
    value = 89285714290.0
  [../]
  [./CA_left]
    type = NeumannBC 
    variable = CA
    boundary = 'left'
    value = 0.0
  [../]
  [./CA_right]
    type = CoupledConvectiveFlux
    variable = CA
    boundary = 'right'
    coefficient = 100
    C_infinity = CA_salt
  [../]
  [./CB_left]
    type = NeumannBC 
    variable = CB
    boundary = 'left'
    value = 0.0
  [../]
  [./CB_right]
    type = NeumannBC 
    variable = CB
    boundary = 'right'
    value = 0.0
  [../]
[]

[Materials]
  [./NiCr]
    type = RadiationNiCr
    Cv = Cv
    Ci = Ci
    CA = CA
    CB = CB
    temperature = 700 #K
    #dose_rate = 1.0e-4 #dpa/s
    dose_rate_function = dose_rate 
    debug_mode = false
  [../]
[]

[Debug]
  show_material_props     = 1       # Print out the material properties supplied for each block, face, neighbor,
  show_top_residuals      = 1       # The number of top residuals to print out (0 = no output)
  show_var_residual       = CA       # Variables for which residuals will be sent to the output file
  show_var_residual_norms = 1       # Print the residual norms of the individual solution variables at each
                                    # nonlinear iteration
[]

[Executioner]
  type = Transient
  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'
  petsc_options =  '-snes_mf_operator'
  petsc_options_iname =  '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value =  'hypre    boomeramg  51'
  #dt = 1.0e-8
  start_time = 0
  end_time = 18000
  dtmax = 100
  num_steps = 100
  l_max_its =  50
  nl_max_its =  30
  #nl_abs_tol=  1e-5
  #nl_rel_tol = 1.0e-8
  dtmin = 1.0e-10
  [./TimeStepper]
      cutback_factor = 0.4
      dt = 1e-4
      growth_factor = 2
      type = IterationAdaptiveDT
  [../]
[]

[Outputs]
  exodus = true
[]
