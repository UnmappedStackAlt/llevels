llevels (Lock Levels) is a simple utility in C for generating a C enum programmatically from a `lock.levels` file specifying lock ordering. The input can be of the following format:

```
# comment
LevelPassive
    LevelCore
        LevelTimers
            LevelSchedulerAPI
                LevelScheduler
        LevelIPIs
            LevelTLBShootdown
        LevelBringup
    LevelExecutive
        LevelPageFrameAllocation
            LevelSlab
                LevelRegionTracking
                LevelVMem
                    LevelPageFaultHandling
            LevelProcessManager
            LevelObjectManager
```

Indentation must be spaces where each indent is 4 spaces (yes, kind of bad to require this, but for my purposes of this tool its okay). Running llevels with a `lock.levels` file containing this tree will output to stdout (redirectable of course to wherever is needed) a C enum like so:

```C
typedef enum {
    LevelPassive=0,
        LevelCore=1,
            LevelTimers=2,
                LevelSchedulerAPI=3,
                    LevelScheduler=4,
            LevelIPIs=2,
                LevelTLBShootdown=3,
            LevelBringup=2,
        LevelExecutive=1,
            LevelPageFrameAllocation=2,
                LevelSlab=3,
                    LevelRegionTracking=4,
                    LevelVMem=4,
                        LevelPageFaultHandling=5,
                LevelProcessManager=3,
                LevelObjectManager=3,
} LockLevels;
```

The higher the level, the more it depends on, and you can do some deadlock avoidance by keeping track of the level of the highest level lock you currently own for a thread then asserting that you don't try acquire a lock lower than that, only higher.

Pleae see `LICENCE` for a copy of the Unlicence licence. This utility is under the public domain.
