from distutils.core import setup, Extension

module1 = Extension('Core',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include', 'include/','src/pylib/'],
                    libraries = ['arnovich_core'],
                    library_dirs = ['/usr/local/lib', './'],
                    sources = ['src/pylib/core_main.c'])

setup (name = 'Core',
       version = '1.0',
       description = 'Core python module',
       author = 'Arnovich',
       ext_modules = [module1])