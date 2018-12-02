############# model NiCr alloy corrosion in molten salt ############3
###CA: Ni; CB: Cr###########
###Unit: micron

[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 25
  nx = 50
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
    scaling = 1.0e-4
    initial_condition = 7.2e10
  [../]
[]

[AuxVariables]
  [./CA_salt]
  [../]
[]

[Kernels]
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
    v = CA
    mat_coef = dBvOmega
    const_coef = 1.0
  [../]
  [./Cv_Jv]
    type = CoupledDiffusion
    variable = Cv
    mat_coef = dAvOmega  
    const_coef = 1.0
  [../]
  [./Cv_Ji]
    type = CoupledDiffusion
    variable = Cv
    mat_coef = dBvOmega  
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
  [../]

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
    v = CA
    mat_coef = dBiOmega
    const_coef = -1.0 
  [../]
  [./Ci_Ji]
    type = CoupledDiffusion
    variable = Ci
    mat_coef = dAiOmega 
    const_coef = 1.0
  [../]
  [./Ci_Jv]
    type = CoupledDiffusion
    variable = Ci
    mat_coef = dBiOmega 
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
  [../]

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
  [./CA_JAi]
    type = CoupledDiffusion
    variable = CA
    mat_coef = dAiOmega
    const_coef = 1.0 
  [../] 
  [./CA_JAv]
    type = CoupledDiffusion
    variable = CA
    mat_coef = dAvOmega
    const_coef = 1.0 
  [../] 
[]

[AuxKernels]
  [./CA_salt_ak]
    type = SaltSoluteConcentrationAux
    variable = CA_salt 
    solute_init = 0.0
  [../]
[]

[Functions]
  [./dose_rate]
    type = ParsedFunction
    value = 1.0e-5+x*1.0e-6
  [../]
[]


[BCs]
  active = 'Ci_left Ci_right Cv_left Cv_right CA_right'
  [./Ci_left]
    type = DirichletBC
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
    type = DirichletBC
    variable = Cv
    boundary = 'left'
    value = 1000.0
  [../]
  [./Cv_right]
    type = DirichletBC
    variable = Cv
    boundary = 'right'
    value = 0
  [../]

  [./CA_left]
    type = NeumannBC 
    variable = CA
    boundary = 'left'
    value = 0 #7.2e10
  [../]
  [./CA_right]
    type = CoupledConvectiveFlux
    variable = CA
    boundary = 'right'
    coefficient = 1.0
    C_infinity = CA_salt
  [../]
[]

[Materials]
  [./NiCr]
    type = RadiationNiCr
    Cv = Cv
    Ci = Ci
    CA = CA
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
  #petsc_options =  '-snes_mf_operator'
  petsc_options_iname =  '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value =  'hypre    boomeramg  51'
  #dt = 1.0e-9
  start_time = 0
  num_steps = 30
  l_max_its =  50
  nl_max_its =  30
  nl_abs_tol=  1e-5
  dtmin = 1.0e-10
  [./TimeStepper]
      cutback_factor = 0.4
      dt = 1e-8
      growth_factor = 2
      type = IterationAdaptiveDT
  [../]
[]

[Outputs]
  exodus = true
[]
