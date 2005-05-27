from distutils.core import setup, Extension

setup(
    name = 'KrossRestrictedPython',
    version = '1.0',
    maintainer = 'Sebastian Sauer',
    maintainer_email = 'mail@dipe.org',
    url = 'http://www.dipe.org/kross',
    description = 'This is the Kross wrapper around the python Zope restricted python module.',
    packages = ["RestrictedPython"],
)
