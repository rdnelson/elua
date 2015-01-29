-- This is the platform specific board configuration file
-- It is used by the generic board configuration system (config/)

module( ..., package.seeall )
local comps = require "components"
local at = require "attributes"

-- Add specific components to the 'components' table
function add_platform_components( t, board, cpu )
  t.cdc = comps.cdc_uart()
  board = board:upper()
  if board == 'EK-TM4C123GXL' then
    t.tm4c_disp = { macro = 'ENABLE_DISP' }
  end
  t.tm4c_pio = { macro = 'ENABLE_TM4C_GPIO' }
end

-- Add specific configuration to the 'configs' table
function add_platform_configs( t, board, cpu )
  t.tm4c_adc_timers = {
    attrs = {
      first_timer = at.int_attr( 'ADC_TIMER_FIRST_ID', 0 ),
      num_timers = at.int_attr( 'ADC_NUM_TIMERS', 0 )
    },
    required = { first_timer = 0, num_timers = "NUM_TIMER" }
  }
end

-- Return an array of all the available platform modules for the given cpu
function get_platform_modules( board, cpu )
  -- m = { pio = { guards = { 'ENABLE_TM4C_GPIO' }, lib = '"pio"', map = "tm4c_pio_map", open = false } }
  -- board = board:upper()
  -- if board == 'EK-TM4C123GXL' then
  --   m.disp = { guards = { 'ENABLE_DISP' }, lib = '"disp"', open = false }
  -- end
  return {}
end

