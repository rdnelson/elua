t = rpc.connect('/dev/tivac')

u = t.uart
u.setup(1, 115200, 8, u.PAR_NONE, u.STOP_1)

function blast()
    for i=1,1000 do
        u.write(1, 0xaa)
    end
end
