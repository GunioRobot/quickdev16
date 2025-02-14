class Scheduler {
public:
  cothread_t thread_snes;
  cothread_t thread_cpu;
  cothread_t thread_smp;
  cothread_t thread_ppu;
  cothread_t thread_dsp;

  enum ActiveThread {
    THREAD_CPU,
    THREAD_SMP,
    THREAD_PPU,
    THREAD_DSP,
  };

  struct {
    unsigned cpu_freq;
    unsigned smp_freq;

    ActiveThread active;
    int64 cpuppu;
    int64 cpusmp;
    int64 smpdsp;
  } clock;

  //==========
  //CPU <> PPU
  //==========

  alwaysinline void sync_cpuppu() {
    if(clock.cpuppu < 0) {
      clock.active = THREAD_PPU;
      co_switch(thread_ppu);
    }
  }

  alwaysinline void sync_ppucpu() {
    if(clock.cpuppu >= 0) {
      clock.active = THREAD_CPU;
      co_switch(thread_cpu);
    }
  }

  //==========
  //CPU <> SMP
  //==========

  alwaysinline void sync_cpusmp() {
    if(clock.cpusmp < 0) {
      clock.active = THREAD_SMP;
      co_switch(thread_smp);
    }
  }

  alwaysinline void sync_smpcpu() {
    if(clock.cpusmp >= 0) {
      clock.active = THREAD_CPU;
      co_switch(thread_cpu);
    }
  }

  //==========
  //SMP <> DSP
  //==========

  alwaysinline void sync_smpdsp() {
    if(clock.smpdsp < 0) {
      clock.active = THREAD_DSP;
      co_switch(thread_dsp);
    }
  }

  alwaysinline void sync_dspsmp() {
    if(clock.smpdsp >= 0) {
      clock.active = THREAD_SMP;
      co_switch(thread_smp);
    }
  }

  //======
  //timing
  //======

  alwaysinline void addclocks_cpu(unsigned clocks) {
    clock.cpuppu -= clocks;
    sync_cpuppu();

    clock.cpusmp -= clocks * (uint64)clock.smp_freq;
    if(clock.cpusmp < -(20000 * (int64)24000000)) sync_cpusmp();
  }

  alwaysinline void addclocks_ppu(unsigned clocks) {
    clock.cpuppu += clocks;
    sync_ppucpu();
  }

  alwaysinline void addclocks_smp(unsigned clocks) {
    clock.cpusmp += clocks * (uint64)clock.cpu_freq;
    if(clock.cpusmp > +(20000 * (int64)24000000)) sync_smpcpu();

    clock.smpdsp -= clocks;
    #if !defined(USE_STATE_MACHINE)
    sync_smpdsp();
    #else
    while(clock.smpdsp < 0) dsp.enter();
    #endif
  }

  alwaysinline void addclocks_dsp(unsigned clocks) {
    clock.smpdsp += clocks;
    #if !defined(USE_STATE_MACHINE)
    sync_dspsmp();
    #endif
  }

  void enter();
  void exit();
  void init();

  Scheduler();
};

extern Scheduler scheduler;
