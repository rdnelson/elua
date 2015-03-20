rpc = rpc.connect("/dev/ttyACM1")

tname = ""

adc = rpc.tm4c.proxima.adc
button = rpc.tm4c.proxima.buttons
accel = rpc.tm4c.proxima.accel
dac = rpc.tm4c.proxima.dac
gyro = rpc.tm4c.proxima.gyro
led = rpc.tm4c.proxima.led
motors = rpc.tm4c.proxima.motors
rtc = rpc.tm4c.proxima.clock
tmr = rpc.tmr

function pause()
    print("Press Enter to continue")
    io.read()
end

function tstart(name)
    tname = name
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    print("%                TEST START")
    print(string.format("%%                %s", tname))
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
end

function tend(result)
    if result == 1 then
        print("                                                         PASSED")
    elseif result == 2 then
        print("                                                         MANUAL")
    else
        print("                                                         FAILED")
    end
end

-- First test, LED flashing

tstart("LED")

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

tend(2)

-- Second test, Buttons

state = 0
tstart("Buttons")

bttn = {}
oldb = -1
b = -1
print("Push each button once")
for i=1,6 do
    while oldb == b do
        b = button.wait()
    end
    oldb = b

    if bttn[b] == nil then
        bttn[b] = 1
    else
        bttn[b] = bttn[b] + 1
    end
end

c = 0
for i=1,6 do
    if bttn[i] then
        c = c + 1
        print(string.format("Button %d pushed %d times.", i, bttn[i]))
    else
        print(string.format("Button %d was not pushed.", i))
    end
end

if c == 6 then
    state = 1
end

tend(state)

-- Third test, Accelerometer

tstart("Accelerometer")

accel.init()

if accel.getid() ~= 104 then
    print("INVALID MPU ID - Test FAILED")
end

print("Place board horizontally")
pause()

x = accel.getx()
y = accel.gety()
z = accel.getz()

state = 1
if math.abs(x) < 1000 then
    print("X Axis ok")
else
    print("Unexpected X value")
    state = 0
end
print(x)

if math.abs(y) < 1000 then
    print("Y Axis ok")
else
    print("Unexpected Y value")
    state = 0
end
print(y)

if math.abs(z) > 10000 then
    print("Z Axis ok")
else
    print("Unexpected Z value")
    state = 0
end
print(z)

tend(state)

-- Fourth test, DAC + ADC

state = 0
tstart("ADC and DAC Minimum Value")

print("Please connect ADC1 to DAC-A")
print("Please connect ADC2 to DAC-B")
pause()

print("Setting both channels to zero")
dac.seta(0)
dac.setb(0)

print("Verifying output of channel A and B")
a = adc.geta()
b = adc.getb()
print(string.format("A=%d, B=%d", a, b))

if a < 100 and b < 100 then
    state = 1
end

tend(state)

pause()
state = 0
tstart("ADC and DAC Maximum Value")

dac.seta(0x3ff)
dac.setb(0x3ff)

print("Verifying output of channel A and B")
a = adc.geta()
b = adc.getb()
print(string.format("A=%d, B=%d", a, b))

if a > 4000 and b > 4000 then
    state = 1
end

tend(state)

state = 0
tstart("Clock")

time = rtc.gettime()
print(string.format("Current time: %d:%d:%d", time[1], time[2], time[3]))

tmr.delay(0, 5000000)

time = rtc.gettime()
print(string.format("Current time: %d:%d:%d", time[1], time[2], time[3]))

tstart("Motors")

print("Driving forward")
motors.forward()
pause()

print("Driving Backwards")
motors.reverse()
pause()

print("Turning right")
motors.right()
pause()

print("Turning left")
motors.left()
pause()

print("Stopping")
motors.stop()
pause()

tend(2)

os.exit()
