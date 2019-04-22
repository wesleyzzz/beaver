[Mesh]
  type = GeneratedMesh
  dim = 1
  xmin = 0
  xmax = 1
  nx = 400
[]

[Variables]
  active = 'u'

  [./u]
    order = FIRST
    family = LAGRANGE
    initial_condition = 10
  [../]
[]

[Kernels]
  active = 'diff ie'

  [./diff]
    type = MatDiffusion
    variable = u
    D_name = Du
  [../]

  [./ie]
    type = TimeDerivative
    variable = u
  [../]
[]

[Materials]
  [./Dc]
    type = GenericConstantMaterial
    prop_names = Du
    prop_values = 1e-5 #2.16512e-6
  [../]
[]

[BCs]
  active = 'left right'

  [./left]
    type = NeumannBC
    variable = u
    boundary = left
    value = 0
  [../]

  [./right]
    type = NeumannBC
    variable = u
    boundary = right
    value = -1
  [../]
[]

[Executioner]
  type = Transient

  solve_type = 'PJFNK'

  num_steps = 50
  dt = 1.0
[]

[Outputs]
  exodus = true
[]
