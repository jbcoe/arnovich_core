
import arnovich.core as Core

Core.set_debug_level(Core.DEBUG_ERROR)
Core.debug(Core.DEBUG_ERROR,"test1")
Core.debug(Core.DEBUG_LOW,"test2")

Core.set_debug_level(Core.DEBUG_LOW)
Core.debug(Core.DEBUG_LOW,"test3")
