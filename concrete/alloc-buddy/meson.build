project('alloc-buddy', 'c',
	default_options: [
		'werror=true',
		'optimization=g'
	]
)

add_project_arguments(
	'-Wall', '-Wextra',
	'-Wstrict-prototypes',
	'-Wold-style-definition',
	'-Wmissing-declarations',
	'-Wbad-function-cast',
	'-Wundef',
	'-Wshadow',
	'-Wcast-align',
	'-Wwrite-strings',
	language: 'c',
)

lib = library('alloc-buddy', 'alloc-buddy.c')

t1 = executable('t1', 't1.c',
	link_with: lib
)
test('t1', t1)
