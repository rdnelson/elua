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

-- Second test, Buttons

print("Beginning Button Test")

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
    print("Button test PASSED")
end

print("End Button Test")

os.exit()
