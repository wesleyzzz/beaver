#
# Example problem showing how to use the DerivativeParsedMaterial with SplitCHParsed.
# The free energy is identical to that from SplitCHMath, f_bulk = 1/4*(1-c)^2*(1+c)^2.
# um

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 150
  ny = 150
  xmax = 300
  ymax = 300
[]

[Variables]
  [./c]
    initial_condition = 0.39
  [../]
  [./w]
    scaling = 1.0e2
  [../]
  [./T]
    initial_condition = 1400
    #scaling = 1e-3
  [../]
[]

[Kernels]
  [./c_res]
    type = SplitCHParsed
    variable = c
    kappa_name = kappa_c
    w = w
    f_name = fbulk
  [../]
  [./w_res]
    type = SplitCHWRes
    variable = w
    mob_name = Mc
  [../]
  [./w_res_th]
    type = ThermoDiffusion
    variable = w
    c = c
    T = T
    MQ_name = MQ
  [../]
  [./time]
    type = CoupledTimeDerivative
    variable = w
    v = c
  [../]
  [./diff]
    type = MatDiffusion
    variable = T
    D_name = thermal_conductivity
  [../]
[]
    

[AuxVariables]
  [./local_energy]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

#[ICs]
#  [./cIC]
#    type = RandomIC
#    variable = c
#    min =  0.1
#    max = 0.4
#  [../]
#[]

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
    prop_names  = 'kappa_c'
    prop_values = '0.0'
  [../]
  [./Mc]
    type = DerivativeParsedMaterial
    f_name = Mc
    args = 'c T'
    constant_names = 'Vm beta0U beta0Zr R HU HZr scaling'
    constant_expressions = '13.18e-6 1.2e13 5.14e14 8.314 128000.0 195000.0 1.0e30' # SI unit scaling to um
    function = 'scaling*Vm*c*(1-c)*(c*beta0U*exp(-HU/R/T)+(1-c)*beta0Zr*exp(-HZr/R/T))'
    derivative_order = 2
    outputs = exodus
  [../]
  [./MQ]
    type = DerivativeParsedMaterial
    f_name = MQ
    args = 'c T'
    constant_names = 'beta0U beta0Zr R QU QZr HU HZr scaling'
    constant_expressions = '1.2e13 5.14e14 8.314 2.5e-20 -17.5e-20 128000.0 195000.0 1.0e24' # SI unit scaling to um
    function = 'scaling*c*(1-c)*(beta0U*exp(-HU/R/T)*QU-beta0Zr*exp(-HZr/R/T)*QZr)'
    derivative_order = 2
    outputs = exodus
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
  [./thcond]
    type = ParsedMaterial
    block = 0
    args = 'c'
    function = 'if(c>0.7,4e-8,4e-8)'
    f_name = thermal_conductivity
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
