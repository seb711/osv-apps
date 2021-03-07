VERSION=3.25
configure_flags = --disable-numa --disable-rdma --disable-rados --disable-rbd \
	--disable-http --disable-gfapi --disable-pmem --disable-shm \
	--disable-tcmalloc


.PHONY: module clean distclean

module: fio/fio

fio:
	git clone --depth 1 --single-branch -b fio-$(VERSION) http://git.kernel.dk/fio.git
	cd fio; patch -p1 < ../patches/0001-configure-disable-POSIX-pshared-attribute.patch

fio/fio: fio
	cd fio; ./configure $(configure_flags); $(MAKE)


clean:
	cd fio; $(MAKE) clean

distclean:
	rm -rf fio
