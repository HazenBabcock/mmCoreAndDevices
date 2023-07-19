bootstrap = R"raw("
# This file is formatted so that it can be run by the Python interpreter directly, and that it can also be included
# in the c++ code.
# See RunScript() in PyDevice.cpp for the code that loads this script as a c++ string.

import numpy as np
from typing import Protocol, runtime_checkable
import sys
import os

# When running this code in Python directly (for debugging), set up the SCRIPT_PATH
# Normally, this is done by the c++ code
if 'SCRIPT_PATH' not in locals():
    SCRIPT_PATH = os.path.dirname(__file__)
    SCRIPT_FILE = SCRIPT_PATH + '\\test.py'
    DEBUGGING = True
else:
    DEBUGGING = False

# Load and execute the script file. This script is expected to set up a dictionary object with the name 'devices',
# which holds name -> device pairs for all devices we want to expose in MM.
sys.path.append(SCRIPT_PATH)
code = open(SCRIPT_FILE)
exec(code.read())
code.close()


@runtime_checkable
class Camera(Protocol):
    data_shape: tuple[int]
    measurement_time: float

    def trigger(self) -> None:
        pass

    def read(self) -> np.ndarray:
        pass

    top: int
    left: int
    height: int
    width: int
    Binning: int


def extract_property_metadata(p):
    if not isinstance(p, property) or not hasattr(p, 'fget') or not hasattr(p.fget, '__annotations__'):
        return None

    return_type = p.fget.__annotations__.get('return', None)
    if return_type is None:
        return None

    min = None
    max = None
    options = None

    if hasattr(return_type, '__metadata__'):  # Annotated
        meta = return_type.__metadata__[0]
        min = meta.get('min', None)
        max = meta.get('max', None)
        ptype = return_type.__origin__.__name__

    elif issubclass(return_type, Enum):  # Enum
        options = {k.lower().capitalize(): v for (k, v) in return_type.__members__.items()}
        ptype = 'enum'

    else:
        min = None
        max = None
        options = None
        ptype = return_type.__name__

    return (ptype, min, max, options)


def set_metadata(obj):
    if isinstance(obj, Camera):
        dtype = "Camera"
    else:
        dtype = "Device"
    properties = [(k, extract_property_metadata(v)) for (k, v) in type(obj).__dict__.items()]
    properties = [(p[0], *p[1]) for p in properties if p[1] is not None]
    obj._MM_dtype = dtype
    obj._MM_properties = properties


for d in devices.values():
    set_metadata(d)
    if DEBUGGING:
        print(d._MM_properties)
# )raw";
