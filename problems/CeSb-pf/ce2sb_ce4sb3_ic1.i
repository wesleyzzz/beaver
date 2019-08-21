# KKS system for two phases (Ce2Sb in Ce4Sb3 matrix)
# c: Ce
# eta: Ce2Sb


[GlobalParams]
  k = 8.0e7
  temperature = 1000
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 80
  ny = 80
  nz = 0
  xmin = 0
  xmax = 40
  ymin = 0
  ymax = 40
  zmin = 0
  zmax = 0
  elem_type = QUAD4
[]

[AuxVariables]
  [./Fglobal]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Variables]
  # order parameter
  [./eta]
    order = FIRST
    family = LAGRANGE
    #scaling = 1.0e-3
  [../]

  # hydrogen concentration
  [./c]
    order = FIRST
    family = LAGRANGE
  [../]

  # chemical potential
  [./w]
    order = FIRST
    family = LAGRANGE
  [../]

  # hydrogen phase concentration (matrix)
  [./cm]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.5714
  [../]
  # hydrogen phase concentration (delta phase)
  [./cd]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.6667
  [../]
[]

[ICs]
  active = 'eta c'
  [./eta]
    variable = eta
    type = SmoothCircleIC
    x1 = 20.0
    y1 = 20.0
    radius = 10
    invalue = 1.0
    outvalue = 0.0
    int_width = 2
  [../]
  [./c]
    variable = c
    type = SmoothCircleIC
    x1 = 20.0
    y1 = 20.0
    radius = 10
    invalue = 0.6667
    outvalue = 0.5714
    int_width = 2
  [../]
  [./cm]
    variable = cm
    type = SmoothCircleIC
    x1 = 0.0
    y1 = 0.0
    radius = 1.5
    invalue = 0.0 
    outvalue = 0.5714
    int_width = 0.1
  [../]
  [./cd]
    variable = cd
    type = SmoothCircleIC
    x1 = 0.0
    y1 = 0.0
    radius = 1.5
    invalue = 0.6667
    outvalue = 0.0
    int_width = 0.1
  [../]
[]


[BCs]
  [./Periodic]
    [./all]
      variable = 'eta w c cm cd'
      auto_direction = 'x y'
    [../]
  [../]
[]

[Materials]
  # Free energy of the matrix
  [./fm]
    type = CeSbFreeEnergy
    f_name = fm
    stoi = 'Ce4Sb3'
    c = cm   # Ce concentration
  [../]
  
  # Free energy of the delta phase
  [./fd]
    type = CeSbFreeEnergy
    f_name = fd
    stoi = 'Ce2Sb'
    c = cd   # Ce concentration
  [../]

  # h(eta)
  [./h_eta]
    type = SwitchingFunctionMaterial
    h_order = HIGH
    eta = eta
  [../]

  # g(eta)
  [./g_eta]
    type = BarrierFunctionMaterial
    g_order = SIMPLE
    eta = eta
  [../]

  # constant properties
  [./constants]
    type = GenericConstantMaterial
    prop_names  = 'M   L   kappa'
    prop_values = '0.7 0.7 0.4  '
  [../]
[]

[Kernels]
  # full transient
  active = 'PhaseConc ChemPotVacancies CHBulk ACBulkF ACBulkC ACInterface dcdt detadt ckernel'

  # enforce c = (1-h(eta))*cm + h(eta)*cd
  [./PhaseConc]
    type = KKSPhaseConcentration
    ca       = cm
    variable = cd
    c        = c
    eta      = eta
  [../]

  # enforce pointwise equality of chemical potentials
  [./ChemPotVacancies]
    type = KKSPhaseChemicalPotential
    variable = cm
    cb       = cd
    fa_name  = fm
    fb_name  = fd
  [../]

  #
  # Cahn-Hilliard Equation
  #
  [./CHBulk]
    type = KKSSplitCHCRes
    variable = c
    ca       = cm
    cb       = cd
    fa_name  = fm
    fb_name  = fd
    w        = w
  [../]

  [./dcdt]
    type = CoupledTimeDerivative
    variable = w
    v = c
  [../]
  [./ckernel]
    type = SplitCHWRes
    mob_name = M
    variable = w
  [../]

  #
  # Allen-Cahn Equation
  #
  [./ACBulkF]
    type = KKSACBulkF
    variable = eta
    fa_name  = fm
    fb_name  = fd
    args     = 'cm cd'
    w        = 0.4
  [../]
  [./ACBulkC]
    type = KKSACBulkC
    variable = eta
    ca       = cm
    cb       = cd
    fa_name  = fm
    fb_name  = fd
  [../]
  [./ACInterface]
    type = ACInterface
    variable = eta
    kappa_name = kappa
  [../]
  [./detadt]
    type = TimeDerivative
    variable = eta
  [../]
[]

[AuxKernels]
  [./GlobalFreeEnergy]
    variable = Fglobal
    type = KKSGlobalFreeEnergy
    fa_name = fm
    fb_name = fd
    w = 0.4
  [../]
[]

[Executioner]
  type = Transient
  #solve_type = 'PJFNK'
  scheme = 'BDF2'
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type  -sub_pc_type   -sub_pc_factor_shift_type'
  petsc_options_value = 'asm       ilu            nonzero'
  l_max_its = 30
  nl_max_its = 40
  l_tol = 1.0e-4
  nl_rel_tol = 1.0e-10
  #nl_abs_tol = 1.0e-10
  num_steps = 20
  dt = 1.0e-2
  active = ''
  [./TimeStepper]
      cutback_factor = 0.4
      dt = 1e-4
      growth_factor = 2
      type = IterationAdaptiveDT
  [../]
[]

#
# Precondition using handcoded off-diagonal terms
#
[Preconditioning]
  [./full]
    type = SMP
    full = true
  [../]
[]


[Outputs]
  exodus = true
  console = false
[]
