-- Configuration file for the TM4C microcontroller

addi( sf( 'src/platform/%s/inc', platform ) )
addi( sf( 'src/platform/%s/driverlib', platform ) )
local cpu = comp.cpu:upper()
local board = comp.board:upper()


-- Only include USB headers/paths for boards which support it
if cpu == 'TM4C123GXL' then
  addi( sf( 'src/platform/%s/usblib', platform ) )
  addi( sf( 'src/platform/%s/usblib/device', platform ) )
end

specific_files = "startup_gcc.c platform.c platform_int.c mpu_help.c proxima.c" -- tm4c_pio.c"
local fwlib_files = utils.get_files( "src/platform/" .. platform .. "/driverlib", ".*%.c$" )


if cpu == 'TM4C123GXL' then
   fwlib_files = fwlib_files .. " " .. utils.get_files( "src/platform/" .. platform .. "/usblib", ".*%.c$" )
   fwlib_files = fwlib_files .. " " .. utils.get_files( "src/platform/" .. platform .. "/usblib/device", ".*%.c$" )
   specific_files = specific_files .. "  usb_serial_structs.c"
end

ldscript = "tm4c.ld"

-- Prepend with path
specific_files = fwlib_files .. " " .. utils.prepend_path( specific_files, "src/platform/" .. platform )
specific_files = specific_files .. " src/platform/cortex_utils.s src/platform/arm_cortex_interrupts.c"
ldscript = sf( "src/platform/%s/%s", platform, ldscript )

addm{ "FOR" .. comp.cpu:upper(), 'gcc', 'CORTEX_M4' }

-- Standard GCC flags
addcf{ '-g', '-DTM4C123GH6PM', '-ffunction-sections', '-fdata-sections', '-fno-strict-aliasing', '-Wall' }
addlf{ '-g', '-nostartfiles', '-nostdlib', '-T', ldscript, '-Wl,--gc-sections', '-Wl,--allow-multiple-definition' }
addaf{ '-x', 'assembler-with-cpp', '-Wall' }
addlib{ 'c','gcc','m' }

local target_flags =  {'-mcpu=cortex-m4','-mthumb' }

-- Configure general flags for target
addcf{ target_flags, '-mlittle-endian' }
addlf{ target_flags, '-Wl,-e,ResetISR', '-Wl,-static' }
addaf( target_flags )

-- Toolset data
tools.tm4c = {}

-- Array of file names that will be checked against the 'prog' target; their absence will force a rebuild
tools.tm4c.prog_flist = { output .. ".bin" }

-- We use 'gcc' as the assembler
toolset.asm = toolset.compile

