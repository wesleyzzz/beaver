############# model NiCr alloy corrosion in molten salt ############3
###CA: Ni; CB: Cr###########

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 20
[]

[Variables]
  [./Cv]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./Ci]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.0
  [../]
  [./CA]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.8
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
    mat_coef = coef_v0A1 
  [../]
  [./Cv_v]
    type = CoupledDiffusion
    variable = Cv
    mat_coef = vacancy_diffusivity  
  [../]
  [./Cv_K0]
    type = BodyForce
    variable = Cv
    value = .001 
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
    mat_coef = coef_i0A1 
  [../]
  [./Ci_Ji]
    type = CoupledDiffusion
    variable = Ci
    v = CA
    mat_coef = interstitial_diffusivity 
  [../]
  [./Ci_K0]
    type = BodyForce
    variable = Ci
    value = .001
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
    mat_coef = coef_i1A0 
  [../]
  [./CA_vA]
    type = CoupledAdvection
    variable = CA
    v = Cv
    mat_coef = coef_v1A0 
  [../]
  [./CA_JA]
    type = CoupledDiffusion
    variable = CA
    mat_coef = mod_solute_diffusivity 
  [../] 
[]

[AuxKernels]
  [./CA_salt_ak]
    type = SaltSoluteConcentrationAux
    variable = CA_salt 
    solute_init = 0.0
  [../]
[]

[BCs]
  [./Ci_left]
    type = DirichletBC
    variable = Ci
    boundary = 'left'
    value = 2
  [../]
  [./Ci_right]
    type = DirichletBC
    variable = Ci
    boundary = 'right'
    value = 2
  [../]

  [./Cv_left]
    type = DirichletBC
    variable = Cv
    boundary = 'left'
    value = 2
  [../]
  [./Cv_right]
    type = DirichletBC
    variable = Cv
    boundary = 'right'
    value = 2
  [../]

  [./CA_left]
    type = DirichletBC 
    variable = CA
    boundary = 'left'
    value = 2
  [../]
  [./CA_right]
    type = CoupledConvectiveFlux
    variable = CA
    boundary = 'right'
    coefficient = 1.0e-5
    C_infinity = CA_salt
  [../]
[]

[Materials]
  [./NiCr]
    type = RadiationNiCr
    Cv = Cv
    Ci = Ci
    CA = CA
    temperature = 900
  [../]
[]

[Executioner]
  type = Transient
  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  dt = 0.1
  start_time = 0
  num_steps = 10
[]

[Outputs]
  exodus = true
[]
