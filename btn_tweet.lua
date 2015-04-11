led = tm4c.proxima.led
cs = tm4c.proxima.spi.chipselect
wait = tm4c.proxima.buttons.wait

while true do
    wait()
    cs(5)
    tmr.delay(0, 1000000)
    cs(0)
    led.setred(1)
    for i=1,60 do
        tmr.delay(0, 1000000)
    end
    led.setred(0)
end
