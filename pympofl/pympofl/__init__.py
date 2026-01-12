import os
from . import inst_cfg
from . import uniform

class StartingException(RuntimeError): pass
class InnerException(RuntimeError): pass

inst_cfg.init()
uniform.__setup__(inst_cfg.CFG)

tasks = []

def add_task(fun): 
    tasks.append(fun)

def call(main_f=None):
    """
    Used as the finishing line of the py script
    """
    
    if not uniform.__insets["st_complete"]: raise StartingException("Failed to start script... Setup incomplete!")

    if main_f != None: add_task(main_f)
    if "rtf" in inst_cfg.CFG["flags"]:
        # TODO: You will understand when see this part
        print("#0: Finished :: Will be implemented soon")
        pass
    else:
        for v in tasks: v()
        print("#0: Finished")
    pass




