############# model NiCr alloy corrosion in molten salt ############3
###CA: Ni; CB: Cr###########
###Unit: micron

# debug: the gradient of A atom is zero
[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 5
  nx = 100
[]

[Variables]
  [./Cv]
    order = FIRST
    family = LAGRANGE
    initial_condition = 1000
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
  [./Xv]
  [../]
  [./Xi]
  [../]
  [./XA]
  [../]
  [./XB]
  [../]
[]

[Kernels]
  [./Cv_dt]
    type = TimeDerivative 
    variable = Cv
  [../]

  [./Ci_dt]
    type = TimeDerivative 
    variable = Ci
  [../]

  [./CA_dt]
    type = TimeDerivative 
    variable = CA
  [../]

  [./CA_JA]
    type = CoupledDiffusion
    variable = CA
    mat_coef = ElemADiffusivity
    const_coef = 1.0 
  [../] 
[]

[AuxKernels]
  [./CA_salt_ak]
    type = SaltSoluteConcentrationAux
    variable = CA_salt 
    solute_init = 0.0
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
    args = 'CA'
    function = '1.0-CA*1.12e-11'
  [../]
[]

[BCs]
  active = 'Ci_left Ci_right Cv_left Cv_right CA_left CA_right'
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
    value = 1000
  [../]

  [./CA_left]
    type = NeumannBC 
    variable = CA
    boundary = 'left'
    value = 0.0
  [../]
  [./CA_right]
    type = NeumannBC
    variable = CA
    boundary = 'right'
    value = -100000
  [../]

#  [./CA_right]
#    type = CoupledConvectiveFlux
#    variable = CA
#    boundary = 'right'
#    coefficient = 1e-3
#    C_infinity = CA_salt
#  [../]
[]

[Materials]
  [./NiCr]
    type = RadiationNiCr
    Cv = Cv
    Ci = Ci
    CA = CA
    temperature = 700 #K
    dose_rate = 1.0e-4 #dpa/s
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
  end_time = 18000
  dtmax = 100
  num_steps = 500
  l_max_its =  100
  nl_max_its =  50
  nl_rel_tol=  1e-8
  l_tol = 1e-10
  dtmin = 1.0e-10
  [./TimeStepper]
      cutback_factor = 0.4
      dt = 1e-2
      growth_factor = 2
      type = IterationAdaptiveDT
  [../]
[]

[Outputs]
  exodus = true
[]
