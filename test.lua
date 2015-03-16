rpc = rpc.connect("/dev/tivac")

adc = rpc.adc
button = rpc.tm4c.proxima.buttons
accel = rpc.tm4c.proxima.accel
dac = rpc.tm4c.proxima.dac
gyro = rpc.tm4c.proxima.gyro
led = rpc.tm4c.proxima.led

function pause()
    print("Press Enter to continue")
    io.read()
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

-- Second test, Buttons

--print("Beginning Button Test")
--
--bttn = {}
--oldb = -1
--b = -1
--print("Push each button once")
--for i=1,6 do
--    while oldb == b do
--        b = button.wait()
--    end
--    oldb = b
--
--    if bttn[b] == nil then
--        bttn[b] = 1
--    else
--        bttn[b] = bttn[b] + 1
--    end
--end
--
--c = 0
--for i=1,6 do
--    if bttn[i] then
--        c = c + 1
--        print(string.format("Button %d pushed %d times.", i, bttn[i]))
--    else
--        print(string.format("Button %d was not pushed.", i))
--    end
--end
--
--if c == 6 then
--    print("Button test PASSED")
--end
--
--print("End Button Test")

-- Third test, Accelerometer

print("Beginning Accelerometer Test")

accel.init()

if accel.getid() ~= 104 then
    print("INVALID MPU ID - Test FAILED")
end

print("Place board horizontally")
pause()

x = accel.getx()
y = accel.gety()
z = accel.getz()

if math.abs(x) < 1000 then
    print("X Axis ok")
else
    print("Unexpected X value")
end
if math.abs(y) < 1000 then
    print("Y Axis ok")
else
    print("Unexpected Y value")
end
if math.abs(z) > 10000 then
    print("Z Axis ok")
else
    print("Unexpected Z value")
end

print("Ending Accelerometer Test")

-- Fourth test, DAC + ADC

print("Beginning DAC test")
print("Please connect PE3 to DAC-A")
print("Please connect PE2 to DAC-B")
pause()

print("Setting both channels to zero")
dac.seta(0)
dac.setb(0)

print("Verifying output of channel A")
adc.sample(8, 1)
a = adc.getsample(8)
print(a)

print("Verifying output of channel B")
adc.sample(10, 1)
b = adc.getsample(10)
print(b)

pause()

print("Setting both channels to max")
dac.seta(0x3ff)
dac.setb(0x3ff)

pause()

print("Verifying output of channel A and B")
adc.sample(8, 1)
a = adc.getsample(8)

adc.sample(10, 1)
b = adc.getsample(10)
print(a)
print(b)

if a < 100 and b < 100 then
    print("Zero value PASSED")
end

os.exit()
