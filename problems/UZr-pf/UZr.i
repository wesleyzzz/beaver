#
# Example problem showing how to use the DerivativeParsedMaterial with SplitCHParsed.
# The free energy is identical to that from SplitCHMath, f_bulk = 1/4*(1-c)^2*(1+c)^2.
#

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 150
  ny = 150
  xmax = 60
  ymax = 60
[]

[Modules]
  [./PhaseField]
    [./Conserved]
      [./c]
        free_energy = fbulk
        mobility = M
        kappa = kappa_c
        solve_type = REVERSE_SPLIT
      [../]
    [../]
  [../]
[]

[Variables]
  [./T]
    order = FIRST
    family = LAGRANGE
    #scaling = 1e-3
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = T
  [../]
[]
    

[AuxVariables]
  [./local_energy]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[ICs]
  [./cIC]
    type = RandomIC
    variable = c
    min = -0.1
    max =  0.1
  [../]
[]

[AuxKernels]
  [./local_energy]
    type = TotalFreeEnergy
    variable = local_energy
    f_name = fbulk
    interfacial_vars = c
    kappa_names = kappa_c
    execute_on = timestep_end
  [../]
[]

[BCs]
  active = 'Periodic'
  [./Periodic]
    [./l1]
      variable = c
      auto_direction = 'x y'
    [../]
    [./l2]
      variable = chem_pot_c
      auto_direction = 'x y'
    [../]
  [../]
  [./temp_left]
    type = DirichletBC
    variable = T
    value = 1050
    boundary = left
  [../]
  [./temp_right]
    type = DirichletBC
    variable = T
    value = 1405
    boundary = right
  [../]
[]

[Materials]
  [./mat]
    type = GenericConstantMaterial
    prop_names  = 'M kappa_c'
    prop_values = '1.0 0.5'
  [../]
  [./free_energy]
    type = DerivativeParsedMaterial
    f_name = fbulk
    args = c
    constant_names = W
    constant_expressions = 1.0/2^2
    function = W*(1-c)^2*(1+c)^2
    enable_jit = true
    outputs = exodus
  [../]
[]

[Postprocessors]
  [./top]
    type = SideIntegralVariablePostprocessor
    variable = c
    boundary = top
  [../]
  [./total_free_energy]
    type = ElementIntegralVariablePostprocessor
    variable = local_energy
  [../]
[]

[Preconditioning]
  [./cw_coupling]
    type = SMP
    full = true
  [../]
[]

[Debug]
  show_top_residuals      = 1       # The number of top residuals to print out (0 = no output)
  show_var_residual       = 'c'       # Variables for which residuals will be sent to the output file
  show_var_residual_norms = 1       # Print the residual norms of the individual solution variables at each
                                    # nonlinear iteration
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  scheme = bdf2

  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm      lu          '

  l_max_its = 30
  l_tol = 1e-4
  nl_max_its = 20
  nl_rel_tol = 1e-9
  nl_abs_tol = 1e-10


  dt = 2.0
  end_time = 20.0
[]

[Outputs]
  exodus = true
  perf_graph = true
[]
