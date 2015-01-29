-- EK-TM4C123GXL build configuration

return {
  cpu = 'tm4c123gxl',
  components = {
    sercon = { uart = 0, speed = 115200 },
    wofs = true,
    romfs = true,
    shell = true,
    term = { lines = 25, cols = 80 },
    cints = true,
    tm4c_disp = true,
    tm4c_pio = true,
    rpc = { uart = 0, speed = 115200 },
    adc = { buf_size = 2 },
    xmodem = true,
  },
  config = {
    vtmr = { num = 4, freq = 4 },
  },
  modules = {
    generic = { 'all', '-adc', '-pwm', '-i2c', '-net', '-can' },
    platform = 'all',
  }
}

