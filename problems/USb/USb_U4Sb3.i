# KKS system for two phases (U4Sb3 in USb matrix)
# c: U
# eta: U4Sb3
#
# Precondition using handcoded off-diagonal terms
#
[GlobalParams]
  k = '8.0e7'
  temperature = '900'
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
  [Fglobal]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[Variables]
  # order parameter
  # uranium concentration
  # chemical potential
  # USb phase concentration (matrix)
  # U4Sb3 precipitate phase concentration
  [eta]
    # scaling = 1.0e-3
    order = FIRST
    family = LAGRANGE
  []
  [c]
    order = FIRST
    family = LAGRANGE
  []
  [w]
    order = FIRST
    family = LAGRANGE
  []
  [cm]
    order = FIRST
    family = LAGRANGE
    initial_condition = '0.5'
  []
  [cp]
    order = FIRST
    family = LAGRANGE
    initial_condition = '0.5714'
  []
[]

[ICs]
  [eta]
    type = SmoothCircleIC
    variable = eta
    x1 = 20.0
    y1 = 20.0
    radius = 10
    invalue = 1.0
    outvalue = 0.0
    int_width = 2
  []
  [c]
    type = SmoothCircleIC
    variable = c
    x1 = 20.0
    y1 = 20.0
    radius = 10
    invalue = 0.5714
    outvalue = 0.5
    int_width = 2
  []
[]

[BCs]
  [Periodic]
    [all]
      variable = 'eta w c cm cp'
      auto_direction = 'x y'
    []
  []
[]

[Materials]
  # Free energy of the matrix
  # Free energy of the U4Sb3 phase
  # h(eta)
  # g(eta)
  # constant properties
  [fm]
    type = USbFreeEnergy
    f_name = fm
    stoi = USb
    c = 'cm' # U concentration in matrix
  []
  [fp]
    type = USbFreeEnergy
    f_name = fp
    stoi = U4Sb3
    c = 'cp' # U concentration in U4Sb3 precipitate
  []
  [h_eta]
    type = SwitchingFunctionMaterial
    h_order = HIGH
    eta = 'eta'
  []
  [g_eta]
    type = BarrierFunctionMaterial
    g_order = SIMPLE
    eta = 'eta'
  []
  [constants]
    type = GenericConstantMaterial
    prop_names = 'M   L   kappa'
    prop_values = '0.7 0.7 0.4  '
  []
[]

[Kernels]
  # full transient
  # enforce c = (1-h(eta))*cm + h(eta)*cp
  # enforce pointwise equality of chemical potentials
  #
  # Cahn-Hilliard Equation
  #
  #
  # Allen-Cahn Equation
  #
  [PhaseConc]
    type = KKSPhaseConcentration
    ca = 'cm'
    variable = cp
    c = 'c'
    eta = 'eta'
  []
  [ChemPotVacancies]
    type = KKSPhaseChemicalPotential
    variable = cm
    cb = 'cp'
    fa_name = fm
    fb_name = fp
  []
  [CHBulk]
    type = KKSSplitCHCRes
    cb = 'cp'
    fb_name = 'fp'
    variable = c
    ca = 'cm'
    fa_name = fm
    w = 'w'
  []
  [dcdt]
    type = CoupledTimeDerivative
    variable = w
    v = 'c'
  []
  [ckernel]
    type = SplitCHWRes
    mob_name = M
    variable = w
  []
  [ACBulkF]
    type = KKSACBulkF
    variable = eta
    fa_name = fm
    fb_name = fp
    args = 'cm cp'
    w = 0.4
  []
  [ACBulkC]
    type = KKSACBulkC
    fb_name = 'fp'
    variable = eta
    ca = 'cm'
    cb = 'cp'
    fa_name = fm
  []
  [ACInterface]
    type = ACInterface
    variable = eta
    kappa_name = kappa
  []
  [detadt]
    type = TimeDerivative
    variable = eta
  []
[]

[AuxKernels]
  [GlobalFreeEnergy]
    type = KKSGlobalFreeEnergy
    variable = Fglobal
    fa_name = fm
    fb_name = fp
    w = 0.4
  []
[]

[Executioner]
  # solve_type = 'PJFNK'
  # nl_abs_tol = 1.0e-10
  type = Transient
  scheme = BDF2
  solve_type = NEWTON
  petsc_options_iname = '-pc_type  -sub_pc_type   -sub_pc_factor_shift_type'
  petsc_options_value = 'asm       ilu            nonzero'
  l_max_its = 30
  nl_max_its = 40
  l_tol = 1.0e-4
  nl_rel_tol = 1.0e-10
  num_steps = 200
  dt = 1.0e-2
  inactive = 'TimeStepper'
  [TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.4
    dt = 1e-4
    growth_factor = 2
  []
[]

[Preconditioning]
  [full]
    type = SMP
    full = true
  []
[]

[Outputs]
  exodus = true
  console = false
[]
