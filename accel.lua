s = rpc.connect("/dev/tivac")
i = s.i2c
a = s.tm4c.proxima.accel
--a.init()

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
