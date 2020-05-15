# Revised from kks_example_noflux.i
# Track Sb concentration
# solid Ce4Sb3 left, liquid Ce2Sb right
# 

[GlobalParams]
  k = 8e7
  temperature = 1000
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 150
  ny = 15
  nz = 0
  xmin = -25
  xmax = 25
  ymin = -2.5
  ymax = 2.5
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
  # solute concentration
  # chemical potential
  # Liquid (Ce2Sb right) phase solute concentration
  # Solid (Ce4Sb3 left) phase solute concentration
  [eta]
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
  [cl] #c_right
    order = FIRST
    family = LAGRANGE
    initial_condition = '0.3333'
  []
  [cs] #c_left
    order = FIRST
    family = LAGRANGE
    initial_condition = '0.4286'
  []
[]

[Functions]
  [ic_func_eta]
    type = ParsedFunction
    value = '0.5*(1.0-tanh((x)/sqrt(2.0)))'
  []
  [ic_func_c]
    type = ParsedFunction
    value = '0.4286*(0.5*(1.0-tanh(x/sqrt(2.0))))^3*(6*(0.5*(1.0-tanh(x/sqrt(2.0))))^2-15*(0.5*(1.0-tanh(x/sqrt(2.0))))+10)+0.3333*(1-(0.5*(1.0-tanh(x/sqrt(2.0))))^3*(6*(0.5*(1.0-tanh(x/sqrt(2.0))))^2-15*(0.5*(1.0-tanh(x/sqrt(2.0))))+10))'
  []
[]

[ICs]
  [eta]
    type = FunctionIC
    variable = eta
    function = ic_func_eta
  []
  [c]
    type = FunctionIC
    variable = c
    function = ic_func_c
  []
[]

[Materials]
  # Free energy of the liquid Ce2Sb right
  # Free energy of the solid Ce4Sb3 left
  # h(eta)
  # g(eta)
  # constant properties
  [fl] #f_right
    type = CeSbFreeEnergy1
    f_name = fl
    stoi = 'Ce2Sb'
    c = cl   # Sb concentration
  []
  [fs] #f_left
    type = CeSbFreeEnergy1
    f_name = fs
    stoi = 'Ce4Sb3'
    c = cs   # Sb concentration
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
    prop_names = 'M   L   eps_sq'
    prop_values = '0.7 0.7 1.0  '
  []
[]

[Kernels]
  # enforce c = (1-h(eta))*cl + h(eta)*cs
  # enforce pointwise equality of chemical potentials
  #
  # Cahn-Hilliard Equation
  #
  #
  # Allen-Cahn Equation
  #
  [PhaseConc]
    type = KKSPhaseConcentration
    ca = 'cl' #c_right
    variable = cs #c_left
    c = 'c' 
    eta = 'eta'
  []
  [ChemPotSolute]
    type = KKSPhaseChemicalPotential
    variable = cl #c_right
    cb = 'cs' #c_left
    fa_name = fl #f_right
    fb_name = fs #f_left
  []
  [CHBulk]
    type = KKSSplitCHCRes
    variable = c #c_right
    ca = 'cl' 
    fa_name = fl #f_right
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
    fa_name = fl #f_right
    fb_name = fs #f_left
    w = 1.0
    args = 'cl cs' #c_right c_left
  []
  [ACBulkC]
    type = KKSACBulkC
    variable = eta
    ca = 'cl' #c_right
    cb = 'cs' #c_left
    fa_name = fl #f_right
  []
  [ACInterface]
    type = ACInterface
    variable = eta
    kappa_name = eps_sq
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
    fa_name = fl #f_right
    fb_name = fs #f_left
    w = 1.0
  []
[]

[Executioner]
  type = Transient
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -sub_pc_type -sub_pc_factor_shift_type'
  petsc_options_value = 'asm      ilu          nonzero'
  l_max_its = 100
  nl_max_its = 100
  num_steps = 50
  dt = 0.1
[]

[Preconditioning]
  [full]
    type = SMP
    full = true
  []
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]
