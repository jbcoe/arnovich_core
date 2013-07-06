from distutils.core import setup, Extension
import sys, os, shutil


name = None
isname = False
majorv = None
ismajorv = False
minorv = None
isminorv = False
isfiles = False
file_dirs = []
isdatafiles = False
datafiles = []
isbuilddir = False
builddir = None
use_core = False
use_math = False
create_package = False
cp_modules = False
modules_dir = None
is_libdir = False
lib_dir = None
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
    elif arg == "--create_package":
        create_package = True
        sys.argv.remove(arg)
    elif arg == "--module_dir":
        cp_modules = True
        sys.argv.remove(arg)
    elif cp_modules == True:
        modules_dir = arg
        sys.argv.remove(arg)
        cp_modules = False
    elif arg == "--lib_dir":
        is_libdir = True
        sys.argv.remove(arg)
    elif is_libdir == True:
        lib_dir = arg
        sys.argv.remove(arg)
        is_libdir = False
    elif arg == "--files":
        isfiles = True
        sys.argv.remove(arg)
    elif isfiles == True:
        file_dirs.append(arg)
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

libname = name
dotname = name.replace('_','.')
shortname = name.split('_')[1]

includes = ['/usr/local/include', 'include/', 'src/pylib/']

if use_core == True:
    includes.append('../core/include')
if use_math == True:
    includes.append('../math/include')

if create_package is True:
    if not os.access(builddir + "/arnovich/", os.F_OK):
        os.makedirs(builddir + "/arnovich/")
    fd = os.open(builddir + "/arnovich/__init__.py", os.O_CREAT)
    os.close(fd)
    fd = os.open(builddir + "/arnovich/" + shortname + ".py", os.O_CREAT | os.O_WRONLY)
    os.write(fd, '\nfrom arnovich._' + shortname + ' import *\n')
    os.close(fd)

if modules_dir is not None:
    if os.access(modules_dir, os.F_OK):
        for p in os.listdir(modules_dir):
            pf = os.path.join(modules_dir, p)
            if os.path.isdir(pf):
                p_name = basename(pf)
                shutil.copytree(pf,  builddir + "/arnovich/" + p_name)
            elif os.path.isfile(pf):
                shutil.copy(pf, builddir + "/arnovich/")

def define_packages(package, package_dir):
    packages = [package]
    for d in os.listdir(package_dir):
        if os.path.isdir(os.path.join(package_dir, d)):
            packages.extend(define_packages(package + "." + d, os.path.join(package_dir, d)))
    return packages

package_dir = 'build/pylib'
package = 'arnovich'
packages = define_packages(package, os.path.join(package_dir, package))

def get_files(datadir, ext):
    dirs = []
    for d in datadir:
        for p in os.listdir(d):
            pf = os.path.join(d, p)
            if os.path.isdir(pf):
                dirs.extend(get_files(pf, ext))
            elif os.path.isfile(pf):
                if ext == os.path.splitext(p)[1]:
                    dirs.append(pf)
    return dirs

data_files = get_files(datafiles, '.xsl')
ext_dir = os.path.join(package, 'etc')
if os.access(ext_dir, os.F_OK):
    data_files.append(ext_dir)

files = get_files(file_dirs, '.cpp')
files.extend(get_files(file_dirs, '.c'))

print "packages:" + str(packages)
print "datafiles:" + str(data_files)
print "files:" + str(files)

libs = libname
if lib_dir is not None:
    libs = os.path.join(lib_dir, libs)

module1 = Extension('_' + shortname,
                    define_macros = [('MAJOR_VERSION', majorv),
                                     ('MINOR_VERSION', minorv)],
                    include_dirs = includes,
                    libraries = [libs],
                    library_dirs = ['/usr/local/lib', './'],
                    sources = files)

setup (name = dotname,
       version = majorv + '.' + minorv,
       author = 'arnovich',
       package_dir = {'': 'build/pylib'},
       packages = packages,
       ext_package = 'arnovich',
       ext_modules = [module1],
       data_files = data_files)

