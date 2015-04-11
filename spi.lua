tiva = rpc.connect("/dev/tivac")
spi = tiva.spi
pio = tiva.pio

print("Connected")

spi.setup(2, spi.MASTER, 20000000, 0, 0, 16)

function sel(n)
    pio.pin.setval(bit.band(n, 1), pio.PA_2)
    pio.pin.setval(bit.band(n, 2)/2, pio.PA_3)
    pio.pin.setval(bit.band(n, 4)/4, pio.PA_4)
end

function setup()
    pio.pin.setdir(pio.OUTPUT, pio.PA_2, pio.PA_3, pio.PA_4)
    sel(0)
end

function dac(a, b)
    local val = bit.bor(0x3000, bit.lshift(bit.band(0x3FF, a), 2))
    print(val)
    sel(2)
    spi.write(2, val)
    sel(0)
    val = bit.bor(0xB000, bit.lshift(bit.band(0x3FF, b), 2))
    print(val)
    sel(2)
    spi.write(2, val)
    sel(0)
end

function test()
    sel(6)
    spi.write(2, 0xde, 0xad, 0xbe, 0xef)
    sel(0)
end

setup()
