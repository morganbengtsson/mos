import sys
import os
import subprocess

#Read the docs
#read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
#if read_the_docs_build:
#    subprocess.call('cd ../; make xml', shell=True)
def run_doxygen(folder):
    """Run the doxygen make command in the designated folder"""

    try:
        retcode = subprocess.call("cd %s; make xml" % folder, shell=True)
        if retcode < 0:
            sys.stderr.write("doxygen terminated by signal %s" % (-retcode))
    except OSError as e:
        sys.stderr.write("doxygen execution failed: %s" % e)


def generate_doxygen_xml(app):
    """Run the doxygen make commands if we're on the ReadTheDocs server"""

    read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

    if read_the_docs_build:
        run_doxygen("../")
        



# -- General configuration ------------------------------------------------

extensions = ["breathe"]
breathe_projects = {"MOS": "../build/xml/"}
breate_default_project = "MOS"

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# The suffix of source filenames.
source_suffix = '.rst'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = 'MOS'
copyright = '2015, Morgan Bengtsson'

version = '1.0.0'
# The full version, including alpha/beta/rc tags.
release = '1.0.0'

exclude_patterns = []

pygments_style = 'sphinx'

# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'default'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Output file base name for HTML help builder.
htmlhelp_basename = 'MOSdoc'
