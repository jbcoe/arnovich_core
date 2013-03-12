from distutils.core import setup, Extension
import sys, os

name = None
isname = False
majorv = None
ismajorv = False
minorv = None
isminorv = False
isfiles = False
files = []
isdatafiles = False
datafiles = []
isbuilddir = False
builddir = None
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
    elif arg == "--builddir":
        isbuilddir = True
        sys.argv.remove(arg)
    elif isbuilddir == True:
        isbuilddir = False
        builddir = arg
        sys.argv.remove(arg)
    elif arg == "--use_core":
        use_core = True
        sys.argv.remove(arg)
    elif arg == "--use_math":
        use_math = True
        sys.argv.remove(arg)
    elif arg == "--files":
        isfiles = True
        sys.argv.remove(arg)
    elif isfiles == True:
        files.append(arg)
        sys.argv.remove(arg)
    elif arg == "--datafiles":
        isdatafiles = True
        sys.argv.remove(arg)
    elif isdatafiles == True:
        datafiles.append(arg)
        sys.argv.remove(arg)
        isdatafiles = False

if (name == None) or (majorv == None) or (minorv == None) or (builddir == None):
    raise Exception("problem....")
# if no _ in name then throw


def define_packages(package, package_dir):
    packages = [package]
    for d in os.listdir(package_dir):
        if os.path.isdir(os.path.join(package_dir, d)):
            packages.extend(define_packages(package + "." + d, os.path.join(package_dir, d)))
    return packages

package_dir = 'build/pylib'
package = 'arnovich'
packages = define_packages(package, os.path.join(package_dir, package))
data_files = [(os.path.join(package, 'etc'), datafiles)]

print "packages:" + str(packages)
print "datafiles:" + str(datafiles)

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
       package_dir = {'': 'build/pylib'},
       packages = packages,
       ext_package = 'arnovich',
       ext_modules = [module1],
       data_files = data_files)

