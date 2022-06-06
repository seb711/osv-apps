from osv.modules import api

default = api.run(cmdline="/weed master -port 9333 -ip 0.0.0.0")
server = api.run(cmdline="/weed server -dir=/tmp -volume.max=5 -ip 0.0.0.0")
volume = api.run(cmdline="/weed volume -dir=/tmp -ip 0.0.0.0")
