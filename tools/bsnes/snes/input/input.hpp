class Input {
public:
  enum Device {
    DeviceNone,
    DeviceJoypad,
    DeviceMultitap,
    DeviceMouse,
    DeviceSuperScope,
    DeviceJustifier,
    DeviceJustifiers,
  };

  enum DeviceID {
    DeviceIDNone,
    DeviceIDJoypad1,
    DeviceIDJoypad2,
    DeviceIDMultitap1A,
    DeviceIDMultitap1B,
    DeviceIDMultitap1C,
    DeviceIDMultitap1D,
    DeviceIDMultitap2A,
    DeviceIDMultitap2B,
    DeviceIDMultitap2C,
    DeviceIDMultitap2D,
    DeviceIDMouse1,
    DeviceIDMouse2,
    DeviceIDSuperScope,
    DeviceIDJustifier1,
    DeviceIDJustifier2,
  };

  enum JoypadID {
    JoypadB      =  0, JoypadY      =  1,
    JoypadSelect =  2, JoypadStart  =  3,
    JoypadUp     =  4, JoypadDown   =  5,
    JoypadLeft   =  6, JoypadRight  =  7,
    JoypadA      =  8, JoypadX      =  9,
    JoypadL      = 10, JoypadR      = 11,
  };

  enum MouseID {
    MouseX    = 0, MouseY     = 1,
    MouseLeft = 2, MouseRight = 3,
  };

  enum SuperScopeID {
    SuperScopeX       = 0, SuperScopeY      = 1,
    SuperScopeTrigger = 2, SuperScopeCursor = 3,
    SuperScopeTurbo   = 4, SuperScopePause  = 5,
  };

  enum JustifierID {
    JustifierX       = 0, JustifierY     = 1,
    JustifierTrigger = 2, JustifierStart = 3,
  };

  uint8 port_read(bool port);
  void port_set_device(bool port, unsigned device);
  void init();
  void poll();
  void update();

  //light guns (Super Scope, Justifier(s)) strobe IOBit whenever the CRT
  //beam cannon is detected. this needs to be tested at the cycle level
  //(hence inlining here for speed) to avoid 'dead space' during DRAM refresh.
  //iobit is updated during port_set_device(),
  //latchx, latchy are updated during update() (once per frame)
  alwaysinline void tick() {
    //only test if Super Scope or Justifier is connected
    if(iobit) {
      if(ppu.vcounter() == latchy               //test Y cursor position
      && ppu.hcounter() == latchx << 2          //test X cursor position (cycles == pixels << 2)
      && latchy < (ppu.overscan() ? 240 : 225)  //verify Y is not offscreen
      && latchx < 256                           //verify X is not offscreen
      ) ppu.latch_counters();
    }
  }
  struct port_t {
    unsigned device;
    unsigned counter0;  //read counters
    unsigned counter1;

    struct superscope_t {
      int x, y;

      bool trigger;
      bool cursor;
      bool turbo;
      bool pause;
      bool offscreen;

      bool turbolock;
      bool triggerlock;
      bool pauselock;
    } superscope;

    struct justifier_t {
      bool active;

      int x1, x2;
      int y1, y2;

      bool trigger1, trigger2;
      bool start1, start2;
    } justifier;
  } port[2];


private:
  bool iobit;
  uint16_t latchx, latchy;

  friend class SNES;

} input;
