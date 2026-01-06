import os
import inst_cfg
import uniform
uniform.__setup__(inst_cfg.CFG)

tasks = []

def add_task(fun): 
    tasks.append(fun)

def call(main_f=None):
    """
    Used as the finishing line of the py script
    """
    


    add_task(main_f)
    for v in tasks:
        v()

def crash(reason):
    # TODO: Crash logic
    pass


