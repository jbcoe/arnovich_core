from distutils.core import setup, Extension
import sys

name = None
isname = False
majorv = None
ismajorv = False
minorv = None
isminorv = False
isfiles = False
files = []
use_core = False
use_math = False
for arg in sys.argv[1:]:
    if arg == "--name":
        isname = True
        sys.argv.remove(arg)
    elif isname == True:
        isname = False
        name = arg
        sys.argv.remove(arg)
    elif arg == "--majorv":
        ismajorv = True
        sys.argv.remove(arg)
    elif ismajorv == True:
        ismajorv = False
        majorv = arg
        sys.argv.remove(arg)
    elif arg == "--minorv":
        isminorv = True
        sys.argv.remove(arg)
    elif isminorv == True:
        isminorv = False
        minorv = arg
        sys.argv.remove(arg)
    elif arg == "--files":
        isfiles = True
        sys.argv.remove(arg)
    elif arg == "--use_core":
        use_core = True
        sys.argv.remove(arg)
    elif arg == "--use_math":
        use_math = True
        sys.argv.remove(arg)
    elif isfiles == True:
        files.append(arg)
        sys.argv.remove(arg)

if (name == None) or (majorv == None) or (minorv == None):
    raise Exception("problem....")
# if no _ in name then throw

libname = name
dotname = name.replace('_','.')
shortname = '_'+name.split('_')[1]

includes = ['/usr/local/include', 'include/', 'src/pylib/']

if use_core == True:
    includes.append('../core/include')
if use_math == True:
    includes.append('../math/include')

module1 = Extension(shortname,
                    define_macros = [('MAJOR_VERSION', majorv),
                                     ('MINOR_VERSION', minorv)],
                    include_dirs = includes,
                    libraries = [libname],
                    library_dirs = ['/usr/local/lib', './'],
                    sources = files)

setup (name = dotname,
       version = majorv+'.'+minorv,
       author = 'arnovich',
       package_dir = {'': 'src/pylib'},
       packages = ['arnovich'],
       ext_package = 'arnovich',
       ext_modules = [module1])

