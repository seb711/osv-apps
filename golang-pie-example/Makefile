.PHONY: module
module: hello.so
	echo '/hello.so: $${MODULE_DIR}/hello.so' > usr.manifest

hello.so: hello.go
	go build -buildmode=pie -ldflags "-linkmode external" -o hello.so hello.go
# Please note that executable built following methods would work as well:
#  'go build -buildmode=exe -ldflags "-linkmode external" -o hello.so hello.go'
#   - on both older 1.12.6 and newer 1.15.8 version of Golang
#  'go build -buildmode=pie -o hello.so hello.go' - on older 1.12.6

clean:
	rm -f hello*.so usr.manifest
