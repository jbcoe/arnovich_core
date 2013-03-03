
import arnovich._core
from arnovich._core import _set_debug_level as set_debug_level, _set_debug_group as set_debug_group, _debug as debug, DEBUG_OFF, DEBUG_ERROR, DEBUG_LOW, DEBUG_MEDIUM, DEBUG_HIGH, DEBUG_PRETTYHIGH, DEBUG_HIGHEST

class connection(arnovich._core.Connection):
    def __init__(self, *args, **kwargs):
        super(connection, self).__init__(args, kwargs)

class tick(arnovich._core.Tick):
    def __init__(self, *args, **kwargs):
        super(tick, self).__init__(args, kwargs)
