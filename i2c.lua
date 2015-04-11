s = rpc.connect("/dev/tivac")
i = s.i2c
i.setup(1, i.SLOW)

function brake1()
    i.address(1, 0x60, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xab)
    i.stop(1)
end

function rev1()
    i.address(1, 0x60, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xa9)
    i.stop(1)
end

function fwd1()
    i.address(1, 0x60, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xaa)
    i.stop(1)
end

function clk()
    i.address(1, 0x33, i.RECEIVER)
    local d = i.read(1, 3)
    for i = 1,d:len() do
        print(d:byte(i))
    end
end

function status()
    i.address(1, 0x30, i.RECEIVER)
    local d = i.read(1, 1)
    for i = 1,d:len() do
        print(d:byte(i))
    end
end

function wfree(d)
    i.address(1, 0x37, i.TRANSMITTER)
    i.start(1)
    i.write(1, d)
    i.stop(1)
end

function rfree()
    i.address(1, 0x37, i.RECEIVER)
    print(i.read(1, 1):byte(1))
end

function brake2()
    i.address(1, 0x61, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xab)
    i.stop(1)
end

function rev2()
    i.address(1, 0x61, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xa9)
    i.stop(1)
end

function fwd2()
    i.address(1, 0x61, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x00, 0xaa)
    i.stop(1)
end

function mpu_wreg(r, d)
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, r)
    i.write(1, d)
    i.stop(1)
end

function mpu_rreg(r, n)
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, r)
    i.address(1, 0x68, i.RECEIVER)
    return i.read(1, n)
end

function mpu_init()
    mpu_wreg(0x6B, 0x01) -- Set clock to x gyro PLL
    mpu_wreg(0x6A, 0x01) -- Reset the signal paths
    mpu_wreg(0x1C, 0x00) -- 2g Sensitivity
    mpu_wreg(0x1A, 0x01) -- 188 Hz bandwidth
    mpu_wreg(0x19, 0x09) -- Sample rate divider = 10
end

function mpu_who()
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x75)
    i.address(1, 0x68, i.RECEIVER)
    b = i.read(1, 1)
    for i=1,b:len() do
        print(string.format("0x%x", b:byte(i)))
    end
end

function mpu_x()
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x3B)
    i.address(1, 0x68, i.RECEIVER)
    b = i.read(1, 2)
    for i=1,b:len() do
        print(string.format("0x%x", b:byte(i)))
    end
end

function mpu_y()
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x3D)
    i.address(1, 0x68, i.RECEIVER)
    b = i.read(1, 2)
    for i=1,b:len() do
        print(string.format("0x%x", b:byte(i)))
    end
end

function mpu_z()
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x3F)
    i.address(1, 0x68, i.RECEIVER)
    b = i.read(1, 2)
    for i=1,b:len() do
        print(string.format("0x%x", b:byte(i)))
    end
end

function mpu_tmp()
    i.address(1, 0x68, i.TRANSMITTER)
    i.start(1)
    i.write(1, 0x41)
    i.address(1, 0x68, i.RECEIVER)
    b = i.read(1, 2)
    for i=1,b:len() do
        print(string.format("0x%x", b:byte(i)))
    end
end

