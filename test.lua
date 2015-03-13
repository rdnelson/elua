rpc = rpc.connect("/dev/tivac")

button = rpc.tm4c.proxima.buttons
accel = rpc.tm4c.proxima.accel
dac = rpc.tm4c.proxima.dac
gyro = rpc.tm4c.proxima.gyro
led = rpc.tm4c.proxima.led

function pause()
    rpc.tmr.delay(1, 1000000)
end

-- First test, LED flashing

print("Beginning LED Test")

print("Red")
led.setred(1)
pause()
print("Green")
led.setred(0)
led.setgreen(1)
pause()
print("Blue")
led.setgreen(0)
led.setblue(1)
pause()
led.setblue(0)

print("LED Test Complete.")
