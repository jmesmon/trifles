project('gtk-dialog', 'c',
  default_options: [
    'werror=true',
    'optimization=g'
  ]
)

add_global_arguments(
  '-Wall', '-Wextra',
  '-ggdb', '-fvar-tracking-assignments',
  language: 'c'
)

gtk = dependency('gtk+-3.0')

executable('gtk-dialog',
  'gtk-dialog.c',
  dependencies: [gtk],
)
