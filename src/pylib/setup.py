from distutils.core import setup, Extension

module1 = Extension('_core',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include', 'include/','src/pylib/'],
                    libraries = ['arnovich_core'],
                    library_dirs = ['/usr/local/lib', './'],
                    sources = ['src/pylib/core_main.c'])

setup (name = 'arnovich.core',
       version = '1.0',
       description = 'Arnovich Core python module',
       author = 'Yerik Arnovich',
	   package_dir = {'': 'src/pylib'},
	   packages = ['arnovich'],
	   ext_package = 'arnovich',
       ext_modules = [module1])
